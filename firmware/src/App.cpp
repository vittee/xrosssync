#include <WiFi.h>
#include "App.h"
#include "screens/SplashScreen.h"
#include "screens/MainScreen.h"

namespace {
    static constexpr auto kLogTag = "App";
}

App::App() {

}

bool App::init() {
    ESP_LOGI(kLogTag, "Starting");

    if (!display.init()) {
        ESP_LOGE(kLogTag, "Display initialization failed");
        return false;
    }

    delay(100);
    display.setRotation(0);
    display.setBrightness(255);

    display.selfTest();
    display.fillScreen(TFT_BLACK);

    WiFi.mode(WIFI_STA);
    WiFi.begin();

    {
        buttonEventQueue = xQueueCreate(32, sizeof(ButtonEvent));

        xTaskCreatePinnedToCore([](void* inst) {
            static_cast<App*>(inst)->buttonTask();
        }, "button_task", 8192, this, 2, nullptr, 1);
    }

    {
        pinMode(PIN_BUTTON_1, INPUT_PULLUP);
        pinMode(PIN_BUTTON_2, INPUT_PULLUP);

        attachInterruptArg(buttons[0].pin, buttonISR, (void*)&buttons[0], CHANGE);
        attachInterruptArg(buttons[1].pin, buttonISR, (void*)&buttons[1], CHANGE);
    }

    {
        xTaskCreatePinnedToCore([](void* inst) {
            static_cast<App*>(inst)->touchTask();
        }, "touch_task", 8192, this, 2, &touchTaskHandle, 1);

        attachInterruptArg(PIN_TOUCH_INT, touchISR, (void*)this, FALLING);
    }

    {
        inputEventQueue = xQueueCreate(32, sizeof(InputEvent));

        xTaskCreatePinnedToCore([](void* inst) {
            static_cast<App*>(inst)->handleInput();
        }, "input_handler_task", 8192, this, 1, nullptr, 1);
    }

    {
        m_splashScreen = new SplashScreen(display.width(), display.height());
        m_screen = m_splashScreen;

        xTaskCreatePinnedToCore([](void* inst) {
            static_cast<App*>(inst)->uiTask();
        }, "ui_task", 4096, this, 1, nullptr, 0);
    }

    m_appEventQueue = xQueueCreate(8, sizeof(AppEvent));

    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                postAppEvent({ AppEventType::WiFiConnected });
                break;

            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                postAppEvent({ AppEventType::WiFiDisconnected });
                break;
        }
    });

    client.onEvent([this](const xr18::XR18Client::Event& e) {
        switch (e.type) {
            case xr18::XR18Client::Event::Type::SearchStarted:
                ESP_LOGI(kLogTag, "Searching for mixer...");
                break;

            case xr18::XR18Client::Event::Type::SearchStopped:
                postAppEvent({ AppEventType::MixersFound });
                break;

            case xr18::XR18Client::Event::Type::Connected:
                ESP_LOGI(kLogTag, "Connected to %s (%s)", e.info.mixer->name.c_str(), e.info.mixer->ip.toString().c_str());
                break;

            case xr18::XR18Client::Event::Type::Disconnected:
                ESP_LOGI(kLogTag, "Disconnected");
                break;

            case xr18::XR18Client::Event::Type::Synchronized:
                ESP_LOGI(kLogTag, "Synchronized");
                break;

            case xr18::XR18Client::Event::Type::StripChanged:
                ESP_LOGI(kLogTag, "Strip[%d] param=%d value=%s", e.strip.index, e.strip.paramId, e.strip.param->formatValue().c_str());
                break;
        }
    });

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<App*>(inst)->appTask();
    }, "app_task", 8192, this, 1, nullptr, 1);

    return true;
}

void App::setScreen(ui::Screen* screen) {
    m_pendingScreen.store(screen);
}

void App::postAppEvent(AppEvent event) {
    xQueueSend(m_appEventQueue, &event, 0);
}

void App::uiTask() {
    for (;;) {
        if (auto* pending = m_pendingScreen.exchange(nullptr)) {
            m_screen = pending;
        }

        if (m_screen) {
            m_screen->render(display);
        }

        vTaskDelay(pdMS_TO_TICKS(1000/30)); // 30fps
    }
}

void App::appTask() {
    AppState curr = AppState::Init;
    AppState prev = static_cast<AppState>(0xFF);

    for (;;) {
        bool transited = (curr != prev);
        prev = curr;

        switch (curr) {
            case AppState::Init:
                curr = stateInit(transited);
                break;
            case AppState::Normal:
                curr = stateNormal(transited);
                break;
            case AppState::SelectMixer:
                curr = stateSelectMixer(transited);
                break;
            case AppState::WifiConfig:
                curr = stateWifiConfig(transited);
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

App::AppState App::stateInit(bool transited) {
    static constexpr uint32_t kSplashMs = 2000;
    static TickType_t startTime = 0;

    if (transited) {
        startTime = xTaskGetTickCount();
    }

    if (xTaskGetTickCount() - startTime >= pdMS_TO_TICKS(kSplashMs)) {
        return AppState::Normal;
    }

    return AppState::Init;
}

App::AppState App::stateNormal(bool transited) {
    static constexpr uint8_t kMaxWifiFailures = 5;
    static uint8_t wifiFailures = 0;
    static bool clientStarted = false;

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

App::AppState App::stateSelectMixer(bool transited) {
    if (transited) {
        // TODO: setScreen(new MixerSelectionScreen(...))
        ESP_LOGI(kLogTag, "SelectMixer state — screen not yet implemented");
    }

    return AppState::SelectMixer;
}

App::AppState App::stateWifiConfig(bool transited) {
    if (transited) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP("XRossSync");
        // TODO: setScreen(new NoWiFiScreen(...))
        ESP_LOGI(kLogTag, "WifiConfig state — AP: %s", WiFi.softAPIP().toString().c_str());
    }

    return AppState::WifiConfig;
}


