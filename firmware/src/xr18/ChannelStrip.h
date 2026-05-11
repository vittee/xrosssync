#pragma once

#include <functional>
#include "nodes/Node.h"
#include "nodes/channels/Channel.h"
#include "nodes/params/StringParam.h"
#include "nodes/params/IntParam.h"
#include "nodes/params/EnumParam.h"
#include "nodes/params/FaderParam.h"
#include "nodes/params/LerpParam.h"

namespace xr18 {

using namespace nodes;

class ChannelStrip {
public:
    enum class StripIndex : uint8_t {
        Ch01 = 0, Ch02, Ch03, Ch04, Ch05, Ch06, Ch07, Ch08, Ch09, Ch10, Ch11, Ch12, Ch13, Ch14, Ch15, Ch16,
        AuxReturn,
        FxReturn1, FxReturn2, FxReturn3, FxReturn4,
        Bus1, Bus2, Bus3, Bus4, Bus5, Bus6,
        FxSend1, FxSend2, FxSend3, FxSend4,
        DCA1, DCA2, DCA3, DCA4,
        LR,
        MAX = LR + 1
    };

    enum class ParamId : uint8_t {
        Name,
        Color,
        On,
        Fader,
        Pan,
        Solosw,
    };

    typedef std::function<void(ParamId, params::Param*)> ChannelEventFn;

    static ChannelStrip from(channels::InputChannel& ch, params::EnumParam& solosw);
    static ChannelStrip from(channels::ReturnChannel& ch, params::EnumParam& solosw);
    static ChannelStrip from(channels::BusChannel& ch, params::EnumParam& solosw);
    static ChannelStrip from(channels::FxSendChannel& ch, params::EnumParam& solosw);
    static ChannelStrip from(channels::DCAChannel& ch, params::EnumParam& solosw);
    static ChannelStrip from(channels::MainChannel& ch, params::EnumParam& solosw);

    void onEvent(ChannelEventFn cb);

    NodeType channelType() const { return m_type; }

    inline params::StringParam& name() const { return *m_name; }
    inline params::IntParam& color() { return *m_color; }
    inline params::EnumParam& on() { return *m_on; }
    inline params::FaderParam& fader() { return *m_fader; }
    inline bool hasPan() const { return m_pan != nullptr; }
    inline params::LerpParam& pan() { return *m_pan; }
    inline params::EnumParam& solosw() { return *m_solosw; }

private:
    ChannelStrip(
        NodeType type,
        params::StringParam& name,
        params::IntParam& color,
        params::EnumParam& on,
        params::FaderParam& fader,
        params::LerpParam* pan,
        params::EnumParam& solosw
    );

    NodeType m_type;
    params::StringParam* m_name;
    params::IntParam* m_color;
    params::EnumParam* m_on;
    params::FaderParam* m_fader;
    params::LerpParam* m_pan;
    params::EnumParam* m_solosw;
    ChannelEventFn m_eventCallback;
};

static constexpr uint8_t solowswIndices[(uint8_t)ChannelStrip::StripIndex::MAX] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,  // Ch01-16 → solosw 01-16
    16,                                                    // AuxReturn → solosw 17
    17, 18, 19, 20,                                        // FxReturn1-4 → solosw 18-21
    39, 40, 41, 42, 43, 44,                                // Bus1-6 → solosw 40-45
    45, 46, 47, 48,                                        // FxSend1-4 → solosw 46-49
    50, 51, 52, 53,                                        // DCA1-4 → solosw 51-54
    49                                                     // LR → solosw 50
};

} // namespace xr18
