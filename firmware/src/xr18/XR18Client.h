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
            struct {
                const MixerInfo *mixer;
            } info;

            struct {
                ChannelStrip::StripIndex index;
                ChannelStrip::ParamId param;
                nodes::params::Param *ptr;
            } strip;

            struct {
                uint8_t mixersFound;
            } search;
        };

    private:
        friend class XR18Client;

        static Event searchStarted() { return { Type::SearchStarted }; }

        static Event searchStopped(uint8_t mixersFound) {
            Event e;
            e.type = Type::SearchStopped;
            e.search = { mixersFound };
            return e;
        }

        static Event connected(const MixerInfo &m) {
            Event e;
            e.type = Type::Connected;
            e.info = {&m};
            return e;
        }

        static Event disconnected() { return { Type::Disconnected }; }

        static Event synchronized() { return { Type::Synchronized }; }

        static Event stripChanged(ChannelStrip::StripIndex s, ChannelStrip::ParamId p, nodes::params::Param *ptr) {
            Event e;
            e.type = Type::StripChanged;
            e.strip = {s, p, ptr};
            return e;
        }
    };

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

    void onEvent(std::function<void(const Event&)> cb);

    inline std::vector<MixerInfo> mixers() const { return m_mixers; }

    inline nodes::RootNode& rootNode() { return m_rootNode; }

    inline ChannelStrip& channelStrip(ChannelStrip::StripIndex index);

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

    std::vector<MixerInfo> m_mixers{1};

    nodes::RootNode m_rootNode;
    std::vector<ChannelStrip> m_channelStrips;
    std::function<void(const Event&)> m_eventCallback;

    Osc osc;
};

inline ChannelStrip& XR18Client::channelStrip(ChannelStrip::StripIndex index) {
    return m_channelStrips[static_cast<uint8_t>(index)];
}

inline void XR18Client::onEvent(std::function<void(const Event&)> cb) {
    m_eventCallback = std::move(cb);
}

}