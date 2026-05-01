#pragma once

#include <Print.h>
#include <WiFiUdp.h>
#include <IPAddress.h>

class UDPPrint : public Print {
public:
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size) override;

    int availableForWrite() override;
    void flush() override;

    void setDestination(IPAddress ip, uint16_t port);

private:
    WiFiUDP m_udp;
    IPAddress m_ip = INADDR_NONE;
    uint16_t m_port;
    uint8_t m_buf[512];
    size_t m_bufSize = 512;
    size_t m_len = 0;
};

#ifdef XROSSSYNC_DEBUG
inline UDPPrint udpPrint;
#endif