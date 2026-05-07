#pragma once

#include <Print.h>
#include <AsyncUDP.h>
#include <IPAddress.h>

class UDPPrint : public Print {
public:
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    int availableForWrite() override;
    void flush() override;

    void setDestination(IPAddress ip, uint16_t port);

private:
    static constexpr size_t kBufSize = 256;

    struct LogPacket {
        uint8_t data[kBufSize];
        size_t len;
    };

    void senderTask();

    AsyncUDP m_udp;
    IPAddress m_ip = INADDR_NONE;
    uint16_t m_port;

    uint8_t m_buf[kBufSize];
    size_t m_len = 0;
    bool m_flushing = false;

    QueueHandle_t m_queue = nullptr;
    TaskHandle_t m_taskHandle = nullptr;
};

#ifdef XROSSSYNC_DEBUG
inline UDPPrint udpPrint;
#endif
