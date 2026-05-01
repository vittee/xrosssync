#include "Osc.h"
#include "BufferPrint.h"

#include "logging/dbp.h"

namespace {
    static constexpr int kMaxPacketSize = 256;
    static constexpr auto kLogTag = "Osc";

    struct RawOsc {
        uint8_t data[kMaxPacketSize];
        size_t size;
    };

    struct OutgoingOsc : RawOsc {
        IPAddress dst;
        uint16_t port;
    };
}

Osc::Osc()
{
    rxQueue = xQueueCreate(8, sizeof(RawOsc));
    txQueue = xQueueCreate(8, sizeof(OutgoingOsc));
}

Osc::~Osc() {
    stop();
}

void Osc::setAddress(const char* ipAddress, uint16_t port) {
    setAddress(ipAddress, port);
}

void Osc::setAddress(IPAddress ipAddress, uint16_t port) {
    this->ipAddr = ipAddress;
    this->port = port;
}

void Osc::start() {
    if (running) return;

    running = true;

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<Osc*>(inst)->run();
    }, "osc_task", 8192, this, 3, &taskHandle, 1);
}

void Osc::stop() {
    if (!running) return;

    running = false;
    vTaskDelete(taskHandle);
    vQueueDelete(rxQueue);
    vQueueDelete(txQueue);

    taskHandle = 0;
}

void Osc::run() {
    while (running) {
        receivePacket();
        sendPacket();

        delay(5); // yield
    }
}

void Osc::receivePacket() {
    auto size = udp.parsePacket();
    if (size <= 0) {
        return;
    }

    if (size > kMaxPacketSize) {
        ESP_LOGW(kLogTag, "Discarding large packet (%d bytes)", size);
        while (udp.available()) {
            udp.read(blackhole, sizeof(blackhole));
        }
        return;
    }

    RawOsc raw{};
    raw.size = udp.read(raw.data, size);

    xQueueSend(rxQueue, &raw, 0);
}

void Osc::sendPacket() {
    OutgoingOsc pkt{};
    int count = 4;
    while (count-- > 0 && xQueueReceive(txQueue, &pkt, 0)) {

#ifdef XROSSSYNC_DEBUG
        ESP_LOGD(kLogTag, "Sending to: %s, size=%d", pkt.dst.toString().c_str(), pkt.size);
#endif

        if (pkt.dst != INADDR_NONE) {
            hexdump(pkt.data, pkt.size, dbp);
            udp.beginPacket(pkt.dst, port);
            udp.write(pkt.data, pkt.size);
            udp.endPacket();
        }
    }
}

bool Osc::send(OSCMessage& msg, TickType_t timeout, IPAddress destination ) {
    OutgoingOsc pkt{};
    pkt.dst = destination != INADDR_NONE ? destination : ipAddr;
    pkt.port = port;

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
    if (xQueueReceive(rxQueue, &raw, timeout) != pdTRUE) {
        return false;
    }

    msg.fill(raw.data, raw.size);
    return !msg.hasError();
}

uint8_t Osc::blackhole[64]{};