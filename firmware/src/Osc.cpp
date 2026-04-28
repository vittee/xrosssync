#include "Osc.h"
#include "BufferPrint.h"

namespace {
    static constexpr int kMaxPacketSize = 256;
    static constexpr auto kLogTag = "Osc";

    struct RawOsc {
        uint8_t data[kMaxPacketSize];
        size_t size;
    };
}

Osc::Osc()
{
    rxQueue = xQueueCreate(8, sizeof(RawOsc));
    txQueue = xQueueCreate(8, sizeof(RawOsc));
}

void Osc::begin(const char* ipAddress, uint16_t port) {
    if (this->ipAddr != INADDR_NONE) {
        return;
    }

    this->ipAddr = ipAddress;
    this->port = port;

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<Osc*>(inst)->run();
    }, "osc_task", 8192, this, 3, &taskHandle, 1);
}

void Osc::run() {
    for (;;) {
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
    RawOsc raw{};
    int count = 4;
    while (count-- > 0 && xQueueReceive(txQueue, &raw, 0)) {
        udp.beginPacket(ipAddr, port);
        udp.write(raw.data, raw.size);
        udp.endPacket();
    }
}

bool Osc::send(OSCMessage& msg, TickType_t timeout) {
    RawOsc raw{};
    BufferPrint p(raw.data, kMaxPacketSize);
    msg.send(p);
    raw.size = p.size();

    return xQueueSend(txQueue, &raw, timeout) == pdTRUE;
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