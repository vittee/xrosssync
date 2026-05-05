#include "ChannelStrip.h"

namespace xr18 {

ChannelStrip::ChannelStrip(
    NodeType type,
    params::StringParam& name,
    params::IntParam& color,
    params::EnumParam& on,
    params::FaderParam& fader,
    params::LerpParam& pan
) :
    m_type(type),
    m_name(&name),
    m_color(&color),
    m_on(&on),
    m_fader(&fader),
    m_pan(&pan)
{

}

ChannelStrip ChannelStrip::from(channels::InputChannel& ch) {
    return ChannelStrip(
        NodeType::Input,
        ch.config().name(),
        ch.config().color(),
        ch.mix().on(),
        ch.mix().fader(),
        ch.mix().pan()
    );
}

} // namespace xr18