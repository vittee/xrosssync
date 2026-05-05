#pragma once

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
    };

    static ChannelStrip from(channels::InputChannel& ch);

    NodeType channelType() const { return m_type; }

    params::StringParam& name() { return *m_name; }
    params::IntParam& color() { return *m_color; }
    params::EnumParam& on() { return *m_on; }
    params::FaderParam& fader() { return *m_fader; }
    params::LerpParam& pan() { return *m_pan; }

private:
    ChannelStrip(
        NodeType type,
        params::StringParam& name,
        params::IntParam& color,
        params::EnumParam& on,
        params::FaderParam& fader,
        params::LerpParam& pan
    );

    NodeType m_type;
    params::StringParam* m_name;
    params::IntParam* m_color;
    params::EnumParam* m_on;
    params::FaderParam* m_fader;
    params::LerpParam* m_pan;
};

} // namespace xr18
