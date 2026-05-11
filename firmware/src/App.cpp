#include <WiFi.h>
#include "App.h"
#include "screens/SplashScreen.h"
#include "screens/MainScreen.h"

App::App() {

}

bool App::init() {
    ESP_LOGI(kLogTag, "Starting");

    if (!m_display.init()) {
        ESP_LOGE(kLogTag, "Display initialization failed");
        return false;
    }

    delay(100);
    m_display.setRotation(0);
    m_display.setBrightness(255);

    m_display.selfTest();
    m_display.fillScreen(TFT_BLACK);

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
        m_splashScreen = new SplashScreen(m_display.width(), m_display.height());
        m_screen = m_splashScreen;

        xTaskCreatePinnedToCore([](void* inst) {
            static_cast<App*>(inst)->uiTask();
        }, "ui_task", 16384, this, 1, nullptr, 0);
    }

    m_appEventQueue = xQueueCreate(8, sizeof(AppEvent));

    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                postAppEvent({ AppEvent::Type::WiFiDisconnected });
                break;
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                postAppEvent({ AppEvent::Type::WiFiConnected });
                break;
        }
    });

    WiFi.mode(WIFI_STA);
    WiFi.begin();

    m_client.onEvent([this](const xr18::XR18Client::Event& e) {
        switch (e.type) {
            case xr18::XR18Client::Event::Type::SearchStarted:
                postAppEvent({ AppEvent::Type::MixerSearchStarted });
                break;

            case xr18::XR18Client::Event::Type::SearchStopped:
                postAppEvent({ AppEvent::Type::MixerSearchStop });
                break;

            case xr18::XR18Client::Event::Type::Connected:
                postAppEvent({ AppEvent::Type::MixerConnected });
                break;

            case xr18::XR18Client::Event::Type::Disconnected:
                postAppEvent({ AppEvent::Type::MixerDisconnected });
                break;

            case xr18::XR18Client::Event::Type::Synchronized:
                postAppEvent({ AppEvent::Type::MixerSynchronized });
                break;
        }
    });

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<App*>(inst)->appTask();
    }, "app_task", 8192, this, 1, nullptr, 1);

    m_client.start();

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
            m_screen->render(m_display);
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

        TickType_t delay = pdMS_TO_TICKS(100);

        switch (curr) {
            case AppState::Init:
                curr = stateInit(transited, delay);
                break;
            case AppState::Normal:
                curr = stateNormal(transited, delay);
                break;
            case AppState::SelectMixer:
                curr = stateSelectMixer(transited, delay);
                break;
            case AppState::WifiConfig:
                curr = stateWifiConfig(transited, delay);
                break;
        }

        vTaskDelay(delay);
    }
}

// State implementations → App_States.cpp
