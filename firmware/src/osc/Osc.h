#pragma once

#include <Arduino.h>
#include <AsyncUDP.h>
#include <OSCMessage.h>

class Osc {
public:
    Osc();
    ~Osc();

    inline void setAddress(const char* ipAddress, uint16_t port = 10024);
    void setAddress(IPAddress ipAddress, uint16_t port = 10024);

    IPAddress getAddress() const { return this->ipAddr; }

    void start();
    void stop();

    inline bool send(OSCMessage& msg, TickType_t timeout, uint8_t retries = 1) {
        return send(msg, timeout, INADDR_NONE, retries);
    }

    bool send(OSCMessage& msg, TickType_t timeout, IPAddress destination = INADDR_NONE, uint8_t retries = 1);
    bool receive(OSCMessage& msg, TickType_t timeout);

private:
    void onPacket(AsyncUDPPacket& packet);
    void txTask();
    bool sendPacket();

    bool running = false;

    IPAddress ipAddr;
    uint16_t port = 10024;

    QueueHandle_t rxQueue;
    QueueHandle_t txQueue;

    AsyncUDP udp;
    TaskHandle_t txTaskHandle;

};
