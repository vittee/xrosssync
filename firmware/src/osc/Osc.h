#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>
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

    bool send(OSCMessage& msg, TickType_t timeout, IPAddress destination = INADDR_NONE);
    bool receive(OSCMessage& msg, TickType_t timeout);
private:
    void run();

    bool receivePacket();
    bool sendPacket();

    bool running = false;

    IPAddress ipAddr;
    uint16_t port = 10024;

    QueueHandle_t rxQueue;
    QueueHandle_t txQueue;

    WiFiUDP udp;
    TaskHandle_t taskHandle;

    static uint8_t blackhole[64];
};