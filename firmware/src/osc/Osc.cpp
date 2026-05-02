#include "Osc.h"
#include "BufferPrint.h"

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
    rxQueue = xQueueCreate(32, sizeof(RawOsc));
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
        int cost = 1;

        cost += receivePacket();
        cost += sendPacket();

        vTaskDelay(pdMS_TO_TICKS(cost * 2 + 1)); // yield
    }
}

bool Osc::receivePacket() {
    auto size = udp.parsePacket();
    if (size <= 0) {
        return false;
    }

    if (size > kMaxPacketSize) {
        ESP_LOGW(kLogTag, "Discarding large packet (%d bytes)", size);
        while (udp.available()) {
            udp.read(blackhole, sizeof(blackhole));
        }

        return false;
    }

    RawOsc raw{};
    raw.size = udp.read(raw.data, size);

#ifdef XROSSSYNC_DEBUG
        ESP_LOGD(kLogTag, "Receiving from: %s, size=%d", udp.remoteIP().toString().c_str(), raw.size);
#endif

    xQueueSend(rxQueue, &raw, 0);

    return true;
}

bool Osc::sendPacket() {
    OutgoingOsc pkt{};

    if (!xQueueReceive(txQueue, &pkt, 0)) {
        return false;
    }

    if (pkt.dst == INADDR_NONE) {
        return false;
    }

#ifdef XROSSSYNC_DEBUG
    ESP_LOGD(kLogTag, "Sending to: %s, size=%d", pkt.dst.toString().c_str(), pkt.size);
#endif

    bool sent = false;
    if (udp.beginPacket(pkt.dst, port)) {
        udp.write(pkt.data, pkt.size);
        sent = udp.endPacket();
    }

    if (!sent) {
        xQueueSendToFront(txQueue, &pkt, 0);
    }

#ifdef XROSSSYNC_DEBUG
    ESP_LOGD(kLogTag, "-------- Sent(%d) --------", sent);
#endif

    return sent;
}

bool Osc::send(OSCMessage& msg, TickType_t timeout, IPAddress destination ) {
    if (msg.hasError()) {
        return false;
    }

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