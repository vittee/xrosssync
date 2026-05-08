#include <WiFi.h>
#include "App.h"
#include "screens/MainScreen.h"

#ifdef XROSSSYNC_DEBUG
#include "logging/dbp.h"
#endif

namespace {
    static constexpr auto kLogTag = "App";
}

App::AppState App::stateInit(bool transited, TickType_t& delay) {
    static constexpr uint32_t kSplashMs = 2000;
    static TickType_t startTime = 0;

    delay = pdMS_TO_TICKS(50);

    if (transited) {
        startTime = xTaskGetTickCount();
    }

    if (xTaskGetTickCount() - startTime >= pdMS_TO_TICKS(kSplashMs)) {
        return AppState::Normal;
    }

    return AppState::Init;
}

App::AppState App::stateNormal(bool transited, TickType_t& delay) {
    static constexpr uint8_t kMaxWifiFailures = 5;
    static uint8_t wifiFailures = 0;
    static bool clientStarted = false;

    delay = pdMS_TO_TICKS(200);

    if (transited) {
        wifiFailures = 0;
        clientStarted = false;

#ifdef XROSSSYNC_DEBUG
        udpPrint.startLogging(WiFi.broadcastIP(), 5005);
#endif

        display.fillScreen(TFT_BLACK);
        setScreen(new MainScreen(display.width(), display.height()));
        delete m_splashScreen;
        m_splashScreen = nullptr;
    }

    AppEvent event;
    while (xQueueReceive(m_appEventQueue, &event, 0) == pdTRUE) {
        switch (event.type) {
            case AppEventType::WiFiConnected:
                wifiFailures = 0;
                if (!clientStarted) {
                    clientStarted = true;
                    client.start();
                    client.search();
                }
                break;

            case AppEventType::WiFiDisconnected:
                wifiFailures++;
                if (wifiFailures >= kMaxWifiFailures) {
                    return AppState::WifiConfig;
                }
                WiFi.reconnect();
                break;

            case AppEventType::MixersFound:
                if (client.mixers().size() > 1) {
                    return AppState::SelectMixer;
                }
                break;
        }
    }

    return AppState::Normal;
}

App::AppState App::stateSelectMixer(bool transited, TickType_t& delay) {
    delay = pdMS_TO_TICKS(100);

    if (transited) {
        // TODO: setScreen(new MixerSelectionScreen(...))
        ESP_LOGI(kLogTag, "SelectMixer state — screen not yet implemented");
    }

    return AppState::SelectMixer;
}

App::AppState App::stateWifiConfig(bool transited, TickType_t& delay) {
    delay = pdMS_TO_TICKS(500);

    if (transited) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("XRossSync");
        // TODO: setScreen(new NoWiFiScreen(...))
        ESP_LOGI(kLogTag, "WifiConfig state — AP: %s", WiFi.softAPIP().toString().c_str());
    }

    return AppState::WifiConfig;
}
