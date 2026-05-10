#include "XR18Client.h"
#include "WiFi.h"

namespace {
    static constexpr auto kLogTag = "XR18";
}

namespace xr18 {

XR18Client::XR18Client()
    : m_rootNode(osc)
{
    m_channelStrips.reserve(static_cast<uint8_t>(ChannelStrip::StripIndex::MAX));

    auto& state = m_rootNode.state();

    auto& inputs = m_rootNode.channels();
    for (int i = static_cast<uint8_t>(ChannelStrip::StripIndex::Ch01); i <= static_cast<uint8_t>(ChannelStrip::StripIndex::Ch16); i++) {
        m_channelStrips.emplace_back(ChannelStrip::from(
            inputs[i],
            state.soloswAt(solowswIndices[i])
        ));
    }

    {
        m_channelStrips.emplace_back(ChannelStrip::from(
            m_rootNode.aux(),
            state.soloswAt(solowswIndices[static_cast<uint8_t>(ChannelStrip::StripIndex::AuxReturn)])
        ));
    }

    auto& fxReturns = m_rootNode.fxReturns();
    for (int i = static_cast<uint8_t>(ChannelStrip::StripIndex::FxReturn1); i <= static_cast<uint8_t>(ChannelStrip::StripIndex::FxReturn4); i++) {
        m_channelStrips.emplace_back(ChannelStrip::from(
            fxReturns[i - static_cast<uint8_t>(ChannelStrip::StripIndex::FxReturn1)],
            state.soloswAt(solowswIndices[i])
        ));
    }

    auto& buses = m_rootNode.buses();
    for (int i = static_cast<uint8_t>(ChannelStrip::StripIndex::Bus1); i <= static_cast<uint8_t>(ChannelStrip::StripIndex::Bus6); i++) {
        m_channelStrips.emplace_back(ChannelStrip::from(
            buses[i - static_cast<uint8_t>(ChannelStrip::StripIndex::Bus1)],
            state.soloswAt(solowswIndices[i])
        ));
    }

    auto& fxSends = m_rootNode.fxSends();
    for (int i = static_cast<uint8_t>(ChannelStrip::StripIndex::FxSend1); i <= static_cast<uint8_t>(ChannelStrip::StripIndex::FxSend4); i++) {
        m_channelStrips.emplace_back(ChannelStrip::from(
            fxSends[i - static_cast<uint8_t>(ChannelStrip::StripIndex::FxSend1)],
            state.soloswAt(solowswIndices[i])
        ));
    }

    auto& dcas = m_rootNode.dcas();
    for (int i = static_cast<uint8_t>(ChannelStrip::StripIndex::DCA1); i <= static_cast<uint8_t>(ChannelStrip::StripIndex::DCA4); i++) {
        m_channelStrips.emplace_back(ChannelStrip::from(
            dcas[i - static_cast<uint8_t>(ChannelStrip::StripIndex::DCA1)],
            state.soloswAt(solowswIndices[i])
        ));
    }

    {
        m_channelStrips.emplace_back(ChannelStrip::from(
            m_rootNode.lr(),
            state.soloswAt(solowswIndices[static_cast<uint8_t>(ChannelStrip::StripIndex::LR)])
        ));
    }

    // register event handler for each strip
    for (int i = 0; i < (int)m_channelStrips.size(); i++) {
        auto stripIndex = static_cast<ChannelStrip::StripIndex>(i);
        m_channelStrips[i].onEvent([this, stripIndex](ChannelStrip::ParamId paramId, params::Param* param) {
            if (m_eventCallback) {
                m_eventCallback({
                    .type = Event::Type::StripChanged,
                    .strip = {
                        .index = stripIndex,
                        .paramId = paramId,
                        .param = param
                    }
                });
            }
        });
    }
}

void XR18Client::start() {
    if (running) {
        return;
    }

    running = true;

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<XR18Client*>(inst)->task();
    }, "xr18_task", 8192, this, 1, &taskHandle, 1);

    osc.start();
}

void XR18Client::search() {
    searchStartTime = millis();
    searching = true;
    m_mixers.clear();

    if (m_eventCallback) {
        m_eventCallback({ Event::Type::SearchStarted });
    }

    OSCMessage msg("/xinfo");
    osc.send(msg, 0, WiFi.broadcastIP());
}

void XR18Client::task() {
    while (running) {
        auto now = millis();

        if ((now - lastHeartbeat) >= 2e3) {
            heartbeat();
        }

        if ((now - lastVitalSign) >= 10e3) {
            lastVitalSign = now;

            if (m_connected) {
                m_connected = false;
                m_mixerInfo.reset();

                if (m_eventCallback) {
                    m_eventCallback({ Event::Type::Disconnected });
                }
            }
        }

        receive();

        if (searching) {
            auto now = millis();

            if ((now - searchStartTime) >= 5e3) {
                // Search timeout
                searching = false;
                stopSearching();
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void XR18Client::stopSearching() {
    if (m_eventCallback) {
        m_eventCallback({ Event::Type::SearchStopped });
    }

    if (!m_connected && m_mixers.size() > 0) {
        setAddress(m_mixers.front().ip);
        heartbeat();
        synchronize(m_mixers.front());
    }
}

void XR18Client::synchronize(MixerInfo &info) {
    m_connected = true;

    m_mixerInfo.emplace(info);

    if (m_eventCallback) {
        m_eventCallback({ Event::Type::Connected });
    }

    OSCMessage msg;

    for (auto& ch : m_rootNode.channels()) {
        msg.empty();
        msg.setAddress(ch.config().path().c_str());
        send(msg, portMAX_DELAY);

        msg.empty();
        msg.setAddress(ch.mix().path().c_str());
        send(msg, portMAX_DELAY);
    }

    {
        auto aux = m_rootNode.aux();

        msg.empty();
        msg.setAddress(aux.config().path().c_str());
        send(msg, portMAX_DELAY);

        msg.empty();
        msg.setAddress(aux.mix().path().c_str());
        send(msg, portMAX_DELAY);
    }

    for (auto& ch : m_rootNode.fxReturns()) {
        msg.empty();
        msg.setAddress(ch.config().path().c_str());
        send(msg, portMAX_DELAY);

        msg.empty();
        msg.setAddress(ch.mix().path().c_str());
        send(msg, portMAX_DELAY);
    }

    for (auto& ch : m_rootNode.buses()) {
        msg.empty();
        msg.setAddress(ch.config().path().c_str());
        send(msg, portMAX_DELAY);

        msg.empty();
        msg.setAddress(ch.mix().path().c_str());
        send(msg, portMAX_DELAY);
    }

    for (auto& ch : m_rootNode.fxSends()) {
        msg.empty();
        msg.setAddress(ch.config().path().c_str());
        send(msg, portMAX_DELAY);

        msg.empty();
        msg.setAddress(ch.mix().path().c_str());
        send(msg, portMAX_DELAY);
    }

    for (auto& ch : m_rootNode.dcas()) {
        msg.empty();
        msg.setAddress(ch.config().path().c_str());
        send(msg, portMAX_DELAY);
    }

    {
        auto lr = m_rootNode.lr();

        msg.empty();
        msg.setAddress(lr.config().path().c_str());
        send(msg, portMAX_DELAY);

        msg.empty();
        msg.setAddress(lr.mix().path().c_str());
        send(msg, portMAX_DELAY);
    }

    {
        msg.empty();
        msg.setAddress(m_rootNode.state().solo().path().c_str());
        send(msg, portMAX_DELAY);
    }

    {
        msg.empty();
        msg.setAddress(m_rootNode.state().solosw().path().c_str());
        send(msg, portMAX_DELAY);
    }

    if (m_eventCallback) {
        m_eventCallback({ Event::Type::Synchronized });
    }
}

void XR18Client::heartbeat() {
    {
        OSCMessage msg("/status");
        osc.send(msg, pdMS_TO_TICKS(1), 0);
    }

    {
        OSCMessage msg("/xremotenfb");
        osc.send(msg, pdMS_TO_TICKS(1), 0);
    }

    lastHeartbeat = millis();
}

bool XR18Client::receive() {
    OSCMessage msg;

    if (!osc.receive(msg, 0)) {
        return false;
    }

    lastVitalSign = millis();

    String address = msg.getAddress();

    if (address == "/status") {
        handleSTATUS(msg);
        return true;
    }

    if (address == "/xinfo") {
        handleXINFO(msg);
        return true;
    }

    auto node = m_rootNode.find(msg.getAddress(), msg.getAddressLength());

    if (node) {
        node->applyOsc(msg);
    }

    return true;
}

void XR18Client::handleSTATUS(OSCMessage &msg) {

#ifdef XROSSSYNC_DEBUG
    char ip[16]{};
    char name[32]{};

    msg.getString(1, ip, sizeof(ip));
    msg.getString(2, name, sizeof(name));
#endif

    if (!searching && !m_connected) {
        synchronize(m_mixers.front());
    }
}

void XR18Client::handleXINFO(OSCMessage &msg) {
    char ip[16]{};
    char name[32]{};
    char model[32]{};
    char version[32]{};

    msg.getString(0, ip, sizeof(ip));
    msg.getString(1, name, sizeof(name));
    msg.getString(2, model, sizeof(model));
    msg.getString(3, version, sizeof(version));

    MixerInfo info{
        .ip = ip,
        .name = name,
        .model = model,
        .version = version
    };

    auto it = std::find_if(m_mixers.begin(), m_mixers.end(), [&](const MixerInfo& m) {
        return m.ip == info.ip;
    });

    if (it == m_mixers.end()) {
        m_mixers.emplace_back(info);
    }
}

void XR18Client::onEvent(std::function<void(const Event&)> cb) {
    m_eventCallback = std::move(cb);
}

}