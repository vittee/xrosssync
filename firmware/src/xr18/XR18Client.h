#pragma once

#include <Arduino.h>
#include <vector>

#include <functional>
#include "osc/Osc.h"
#include "nodes/RootNode.h"
#include "ChannelStrip.h"

namespace xr18 {

class XR18Client {
public:

    struct MixerInfo {
        IPAddress ip;
        String name;
        String model;
        String version;
    };

    struct StripChanged {
        ChannelStrip::StripIndex index;
        ChannelStrip::ParamId paramId;
        nodes::params::Param *param;
    };

    struct Event {
    public:
        enum class Type : uint8_t {
            SearchStarted,
            SearchStopped,
            Connected,
            Disconnected,
            Synchronized,
            StripChanged,
        };

        Type type;

        union {
            StripChanged strip;
        };
    };

public:
    XR18Client();

    inline void setAddress(IPAddress ip, uint16_t port = 10024)  {
        osc.setAddress(ip, port);
    }

    inline bool send(OSCMessage& msg, TickType_t timeout) {
        return osc.send(msg, timeout, 1);
    }

    inline bool connected() const { return m_connected; }

    void start();

    void search();

    void onEvent(std::function<void(const Event&)> cb);

    inline std::vector<MixerInfo> mixers() const { return m_mixers; }

    inline std::optional<MixerInfo> mixerInfo() const { return m_mixerInfo; }

    inline nodes::RootNode& rootNode() { return m_rootNode; }

    inline ChannelStrip& channelStrip(ChannelStrip::StripIndex index) {
        return m_channelStrips[static_cast<uint8_t>(index)];
    }

private:
    void task();

    void stopSearching();

    void synchronize(MixerInfo &info);

    void heartbeat();

    bool receive();

    void handleSTATUS(OSCMessage &msg);

    void handleXINFO(OSCMessage &msg);

    bool running = false;
    TaskHandle_t taskHandle;

    bool m_connected = false;
    unsigned long lastHeartbeat = 0;
    unsigned long lastVitalSign = 0;

    bool searching = false;
    unsigned long searchStartTime = 0;

    std::vector<MixerInfo> m_mixers{1};
    std::optional<MixerInfo> m_mixerInfo;

    nodes::RootNode m_rootNode;
    std::vector<ChannelStrip> m_channelStrips;
    std::function<void(const Event&)> m_eventCallback;

    Osc osc;
};

}