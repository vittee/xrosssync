#include "ChannelStrip.h"
#include "psram.h"

namespace xr18 {

ChannelStrip::ChannelStrip(
    channels::Channel ch,
    params::StringParam& name,
    params::IntParam& color,
    params::EnumParam& on,
    params::FaderParam& fader,
    params::LerpParam* pan,
    params::EnumParam& solosw
) :
    m_ch(ch),
    m_name(&name),
    m_color(&color),
    m_on(&on),
    m_fader(&fader),
    m_pan(pan),
    m_solosw(&solosw)
{

}

void ChannelStrip::onEvent(ChannelEventFn cb) {
    m_eventCallback = std::move(cb);

    m_name->onChange([this]() {
        m_eventCallback(ParamId::Name, m_name);
    });

    m_color->onChange([this]() {
        m_eventCallback(ParamId::Color, m_color);
    });

    m_on->onChange([this]() {
        m_eventCallback(ParamId::On, m_on);
    });

    m_fader->onChange([this]() {
        m_eventCallback(ParamId::Fader, m_fader);
    });

    if (m_pan) {
        m_pan->onChange([this]() {
            m_eventCallback(ParamId::Pan, m_pan);
        });
    }

    m_solosw->onChange([this]() {
        m_eventCallback(ParamId::Solosw, m_solosw);
    });

}

ChannelStrip* ChannelStrip::from(channels::InputChannel& ch, params::EnumParam& solosw) {
    return psram_new<ChannelStrip>(
        ch,
        ch.config().name(),
        ch.config().color(),
        ch.mix().on(),
        ch.mix().fader(),
        &ch.mix().pan(),
        solosw
    );
}

ChannelStrip* ChannelStrip::from(channels::ReturnChannel& ch, params::EnumParam& solosw) {
    return psram_new<ChannelStrip>(
        ch,
        ch.config().name(),
        ch.config().color(),
        ch.mix().on(),
        ch.mix().fader(),
        &ch.mix().pan(),
        solosw
    );
}

ChannelStrip* ChannelStrip::from(channels::BusChannel& ch, params::EnumParam& solosw) {
    return psram_new<ChannelStrip>(
        ch,
        ch.config().name(),
        ch.config().color(),
        ch.mix().on(),
        ch.mix().fader(),
        &ch.mix().pan(),
        solosw
    );
}

ChannelStrip* ChannelStrip::from(channels::FxSendChannel& ch, params::EnumParam& solosw) {
    return psram_new<ChannelStrip>(
        ch,
        ch.config().name(),
        ch.config().color(),
        ch.mix().on(),
        ch.mix().fader(),
        nullptr,
        solosw
    );
}

ChannelStrip* ChannelStrip::from(channels::DCAChannel& ch, params::EnumParam& solosw) {
    return psram_new<ChannelStrip>(
        ch,
        ch.config().name(),
        ch.config().color(),
        ch.on(),
        ch.fader(),
        nullptr,
        solosw
    );
}

ChannelStrip* ChannelStrip::from(channels::MainChannel& ch, params::EnumParam& solosw) {
    return psram_new<ChannelStrip>(
        ch,
        ch.config().name(),
        ch.config().color(),
        ch.mix().on(),
        ch.mix().fader(),
        &ch.mix().pan(),
        solosw
    );
}

} // namespace xr18
