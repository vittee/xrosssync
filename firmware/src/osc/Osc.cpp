#include "Osc.h"
#include "BufferPrint.h"

namespace {
    static constexpr int kMaxPacketSize = 512;
    static constexpr auto kLogTag = "Osc";

    struct RawOsc {
        uint8_t data[kMaxPacketSize];
        size_t size;
    };

    struct OutgoingOsc : RawOsc {
        IPAddress dst;
        uint16_t port;
        uint8_t retries;
    };
}

Osc::Osc()
{
    rxQueue = xQueueCreate(96, sizeof(RawOsc));
    txQueue = xQueueCreate(32, sizeof(OutgoingOsc));
}

Osc::~Osc() {
    stop();
}

void Osc::setAddress(const char* ipAddress, uint16_t port) {
    IPAddress addr(ipAddress);
    setAddress(addr, port);
}

void Osc::setAddress(IPAddress ipAddress, uint16_t port) {
    this->ipAddr = ipAddress;
    this->port = port;
}

void Osc::start() {
    if (running) return;

    running = true;

    udp.listen(0);
    udp.onPacket([this](AsyncUDPPacket packet) {
        onPacket(packet);
    });

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<Osc*>(inst)->txTask();
    }, "osc_tx", 8192, this, 2, &txTaskHandle, 1);
}

void Osc::stop() {
    if (!running) return;

    running = false;
    udp.close();
    vTaskDelete(txTaskHandle);
    vQueueDelete(rxQueue);
    vQueueDelete(txQueue);

    txTaskHandle = nullptr;
}

void Osc::onPacket(AsyncUDPPacket& packet) {
    size_t size = packet.length();

    if (size > kMaxPacketSize) {
        ESP_LOGW(kLogTag, "Discarding large packet (%d bytes)", size);
        return;
    }

    RawOsc raw{};
    raw.size = size;
    memcpy(raw.data, packet.data(), size);

    if (!xQueueSend(rxQueue, &raw, 0)) {
        ESP_LOGW(kLogTag, "rxQueue full, packet dropped");
    }
}

void Osc::txTask() {
    while (running) {
        sendPacket();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(nullptr);
}

bool Osc::sendPacket() {
    OutgoingOsc pkt{};

    if (!xQueueReceive(txQueue, &pkt, 0)) {
        return false;
    }

    if (pkt.dst == INADDR_NONE) {
        return false;
    }

    size_t sent = udp.writeTo(pkt.data, pkt.size, pkt.dst, pkt.port);

    if (sent == 0 && pkt.retries > 0) {
        pkt.retries--;
        xQueueSendToFront(txQueue, &pkt, 0);
    }

    return sent > 0;
}

bool Osc::send(OSCMessage& msg, TickType_t timeout, IPAddress destination, uint8_t retries) {
    if (msg.hasError()) {
        return false;
    }

    OutgoingOsc pkt{
        .dst = destination != INADDR_NONE ? destination : ipAddr,
        .port = port,
        .retries = retries
    };

    if (pkt.dst == INADDR_NONE) {
        return false;
    }

    BufferPrint p(pkt.data, kMaxPacketSize);
    msg.send(p);
    pkt.size = p.size();

    return xQueueSend(txQueue, &pkt, timeout) == pdTRUE;
}

bool Osc::receive(OSCMessage& msg, TickType_t timeout) {
    RawOsc raw{};
    if (!xQueueReceive(rxQueue, &raw, timeout)) {
        return false;
    }

    msg.fill(raw.data, raw.size);
    return !msg.hasError();
}

