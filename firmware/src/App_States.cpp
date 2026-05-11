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

#ifdef XROSSSYNC_DEBUG
        udpPrint.startLogging(WiFi.broadcastIP(), 5005);
#endif

        m_display.fillScreen(TFT_BLACK);
        m_mainScreen = new MainScreen(this, m_display.width(), m_display.height());
        setScreen(m_mainScreen);
        delete m_splashScreen;
        m_splashScreen = nullptr;
    }

    AppEvent event;
    while (xQueueReceive(m_appEventQueue, &event, 0)) {
        switch (event.type) {
            case AppEvent::Type::WiFiConnected:
                wifiFailures = 0;

                if (!m_client.connected()) {
                    m_client.search();
                }

                break;

            case AppEvent::Type::WiFiDisconnected:
                wifiFailures++;

                if (m_mainScreen) {
                    m_mainScreen->setStatus({ MainScreen::Status::Type::NoWifi });
                }

                if (wifiFailures >= kMaxWifiFailures) {
                    return AppState::WifiConfig;
                }

                WiFi.disconnect();
                WiFi.begin();
                break;

            case AppEvent::Type::MixerSearchStarted:
                if (m_mainScreen) {
                    m_mainScreen->setStatus({ MainScreen::Status::Type::Searching });
                }
                break;

            case AppEvent::Type::MixerSearchStop: {
                auto count = m_client.mixers().size();
                if (count > 1) {
                    return AppState::SelectMixer;
                }

                if (count < 1) {
                    m_client.search();
                }

                break;
            }

            case AppEvent::Type::MixerDisconnected:
                if (m_mainScreen) {
                    m_mainScreen->setStatus({ MainScreen::Status::Type::NotConencted });
                }
                break;

            case AppEvent::Type::MixerConnected:
                if (m_mainScreen) {
                    m_mainScreen->setStatus({ MainScreen::Status::Type::Connected });
                }
                break;

            case AppEvent::Type::MixerSynchronized:
                break;

            case AppEvent::Type::Input:
                switch (event.input.type) {
                    case InputEvent::Type::Button:
                        if (m_mainScreen) {
                            if (event.input.button.pressed) {
                                switch (event.input.button.button) {
                                    case 1:
                                        m_mainScreen->prevPage();
                                        break;

                                    case 2:
                                        m_mainScreen->nextPage();
                                        break;
                                }
                            }
                        }
                        break;

                    case InputEvent::Type::Home:
                        if (m_mainScreen) {
                            m_mainScreen->firstPage();
                        }
                        break;

                    case InputEvent::Type::Touch:
                        ESP_LOGI(kLogTag, "Touch flag=%d, x=%d, y=%d", static_cast<uint8_t>(event.input.touch.flag), event.input.touch.point.x, event.input.touch.point.y);
                        break;
                }
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
