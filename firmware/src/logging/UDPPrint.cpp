#include "UDPPrint.h"

void UDPPrint::setDestination(IPAddress ip, uint16_t port) {
    m_ip = ip;
    m_port = port;

    m_queue = xQueueCreate(128, sizeof(LogPacket));

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<UDPPrint*>(inst)->senderTask();
    }, "udpprint", 4096, this, 2, &m_taskHandle, 1);
}

size_t UDPPrint::write(uint8_t c) {
    if (m_len >= kBufSize) {
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
        size_t space = kBufSize - m_len;
        size_t chunk = size - written;

        if (chunk > space) {
            chunk = space;
        }

        memcpy(m_buf + m_len, buffer + written, chunk);
        m_len += chunk;
        written += chunk;

        if (m_len >= kBufSize) {
            flush();
        }
    }

    if (size > 0 && buffer[size - 1] == '\n') {
        flush();
    }

    return written;
}

int UDPPrint::availableForWrite() {
    return static_cast<int>(kBufSize - m_len);
}

void UDPPrint::flush() {
    if (m_len == 0 || m_flushing || m_queue == nullptr) {
        return;
    }

    m_flushing = true;

    LogPacket pkt{};
    pkt.len = m_len;
    memcpy(pkt.data, m_buf, m_len);
    m_len = 0;

    xQueueSend(m_queue, &pkt, 0);
    xTaskNotifyGive(m_taskHandle);

    m_flushing = false;
}

void UDPPrint::senderTask() {
    LogPacket pkt{};

    while (true) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        if (xQueueReceive(m_queue, &pkt, 0)) {
            if (m_ip != INADDR_NONE) {
                m_udp.writeTo(pkt.data, pkt.len, m_ip, m_port);
                vTaskDelay(1);
            }
        }
    }
}
