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

    auto& channels = m_rootNode.channels();
    for (int i = static_cast<uint8_t>(ChannelStrip::StripIndex::Ch01); i <= static_cast<uint8_t>(ChannelStrip::StripIndex::Ch16); i++) {
        m_channelStrips.emplace_back(ChannelStrip::from(
            channels[i],
            m_rootNode.state().soloswAt(solowswIndices[i])
        ));
    }

    for (int i = 0; i < (int)m_channelStrips.size(); i++) {
        auto stripIndex = static_cast<ChannelStrip::StripIndex>(i);
        m_channelStrips[i].onEvent([this, stripIndex](ChannelStrip::ParamId paramId, params::Param* p) {
            if (m_eventCallback) {
                m_eventCallback(Event::stripChanged(stripIndex, paramId, p));
            }
        });
    }
}

void XR18Client::start() {
    if (running) return;

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
        m_eventCallback(Event::searchStarted());
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
            connected = false;
            if (m_eventCallback) {
                m_eventCallback(Event::disconnected());
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
    ESP_LOGD(kLogTag, "Autoconnect connected=%d, ip=%s, mixers found=%d", connected, osc.getAddress().toString().c_str(), m_mixers.size());

    if (m_eventCallback) {
        m_eventCallback(Event::searchStopped(m_mixers.size()));
    }

    if (!connected && m_mixers.size() > 0) {
        connected = true;
        setAddress(m_mixers.front().ip);

        if (m_eventCallback) {
            m_eventCallback(Event::connected(m_mixers.front()));
        }

        heartbeat();
        synchronize();
    }
}

void XR18Client::synchronize() {
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
        m_eventCallback(Event::synchronized());
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

    if (!searching && !connected) {
        connected = true;
        if (m_eventCallback) {
            m_eventCallback(Event::connected(m_mixers.front()));
        }
        synchronize();
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

}