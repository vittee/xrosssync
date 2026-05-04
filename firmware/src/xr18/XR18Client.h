#pragma once

#include <Arduino.h>
#include <vector>

#include "osc/Osc.h"
#include "nodes/RootNode.h"

namespace xr18 {

struct MixerInfo {
    IPAddress ip;
    String name;
    String model;
    String version;
};

class XR18Client {
public:
    XR18Client();

    inline void setAddress(IPAddress ip, uint16_t port = 10024)  {
        osc.setAddress(ip, port);
    }

    inline bool send(OSCMessage& msg, TickType_t timeout) {
        return osc.send(msg, timeout, 1);
    }

    void start();

    void search();
private:
    void task();

    void stopSearching();

    void synchronize();

    void heartbeat();

    bool receive();

    void handleSTATUS(OSCMessage &msg);

    void handleXINFO(OSCMessage &msg);

    bool running = false;
    TaskHandle_t taskHandle;

    bool connected = false;
    unsigned long lastHeartbeat = 0;
    unsigned long lastVitalSign = 0;

    bool searching = false;
    unsigned long searchStartTime = 0;

    std::vector<MixerInfo> mixers{1};

    nodes::RootNode rootNode;

    Osc osc;
};

}