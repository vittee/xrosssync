#include "UDPPrint.h"

#include <string.h>

UDPPrint::UDPPrint()
{
    m_buf = new uint8_t[m_bufSize];
}

UDPPrint::~UDPPrint() {
    delete[] m_buf;
}

void UDPPrint::setDestination(IPAddress ip, uint16_t port) {
    m_ip = ip;
    m_port = port;
}

size_t UDPPrint::write(uint8_t c) {
    if (m_len >= m_bufSize) {
        flush();
    }
    m_buf[m_len++] = c;
    if (c == '\n') {
        flush();
    }
    return 1;
}

size_t UDPPrint::write(const uint8_t *buffer, size_t size) {
    size_t written = 0;
    while (written < size) {
        size_t space = m_bufSize - m_len;
        size_t chunk = size - written;
        if (chunk > space) {
            chunk = space;
        }
        memcpy(m_buf + m_len, buffer + written, chunk);
        m_len += chunk;
        written += chunk;
        if (m_len >= m_bufSize) {
            flush();
        }
    }
    // auto-flush when the write ends with a newline
    if (size > 0 && buffer[size - 1] == '\n') {
        flush();
    }
    return written;
}

int UDPPrint::availableForWrite() {
    return static_cast<int>(m_bufSize - m_len);
}

void UDPPrint::flush() {
    if (m_len == 0) {
        return;
    }

    if (m_ip != INADDR_NONE) {
        m_udp.beginPacket(m_ip, m_port);
        m_udp.write(m_buf, m_len);
        m_udp.endPacket();
    }

    m_len = 0;
}
