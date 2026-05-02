#include "XR18Client.h"
#include "WiFi.h"

namespace {
    static constexpr auto kLogTag = "XR18";
}

namespace xr18 {

XR18Client::XR18Client() {

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
    mixers.clear();
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
    ESP_LOGD(kLogTag, "Autoconnect connected=%d, ip=%s, mixers found=%d", connected, osc.getAddress().toString().c_str(), mixers.size());
    if (!connected && mixers.size() > 0) {
        connected = true;
        setAddress(mixers.front().ip);
        heartbeat();
        synchronize();
    }
}

void XR18Client::synchronize() {
}

void XR18Client::heartbeat() {
    {
        OSCMessage msg("/status");
        send(msg, pdMS_TO_TICKS(1));
    }

    {
        OSCMessage msg("/xremotenfb");
        send(msg, pdMS_TO_TICKS(1));
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

    ESP_LOGD(kLogTag, "Got %s", msg.getAddress());

    return true;
}

void XR18Client::handleSTATUS(OSCMessage &msg) {

#ifdef XROSSSYNC_DEBUG
    char ip[16]{};
    char name[32]{};

    msg.getString(1, ip, sizeof(ip));
    msg.getString(2, name, sizeof(name));

    ESP_LOGD(kLogTag, "Got Status, connected=%d, searching=%d, ip=%s, name=%s", connected, searching, ip, name);
#endif

    if (!searching && !connected) {
        connected = true;
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

    auto it = std::find_if(mixers.begin(), mixers.end(), [&](const MixerInfo& m) {
        return m.ip == info.ip;
    });

    if (it == mixers.end()) {
        mixers.emplace_back(info);
    }
}

}