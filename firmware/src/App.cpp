#include <WiFi.h>
#include "App.h"
#include "screens/SplashScreen.h"

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
    display.setFont(&fonts::Font2);
    display.setTextColor(TFT_WHITE, 0);

    WiFi.mode(WIFI_STA);
    WiFi.begin();

    display.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        display.print('.');
        delay(100);
    }

#ifdef XROSSSYNC_DEBUG
    udpPrint.setDestination(WiFi.broadcastIP(), 5005);

    struct LogMessage {
        char buf[256];
    };

    static QueueHandle_t logQueue = xQueueCreate(8, sizeof(LogMessage));

    xTaskCreatePinnedToCore([](void*) {
        LogMessage msg;
        for (;;) {
            if (xQueueReceive(logQueue, &msg, portMAX_DELAY)) {
                udpPrint.print(msg.buf);
            }
        }
    }, "log_task", 2048, nullptr, 1, nullptr, 0);

    esp_log_set_vprintf([](const char* fmt, va_list args) -> int {
        if (!logQueue) {
            return 0;
        }

        LogMessage msg;
        int len = vsnprintf(msg.buf, sizeof(msg.buf), fmt, args);
        xQueueSend(logQueue, &msg, 0);
        return len;
    });
#endif

    buttonEventQueue = xQueueCreate(32, sizeof(ButtonEvent));

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<App*>(inst)->buttonTask();
    }, "button_task", 8192, this, 2, nullptr, 1);

    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);

    attachInterruptArg(buttons[0].pin, buttonISR, (void*)&buttons[0], CHANGE);
    attachInterruptArg(buttons[1].pin, buttonISR, (void*)&buttons[1], CHANGE);

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<App*>(inst)->touchTask();
    }, "touch_task", 8192, this, 2, &touchTaskHandle, 1);

    attachInterruptArg(PIN_TOUCH_INT, touchISR, (void*)this, FALLING);

    inputEventQueue = xQueueCreate(32, sizeof(InputEvent));

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<App*>(inst)->handleInput();
    }, "input_handler_task", 8192, this, 1, nullptr, 1);

    display.fillScreen(TFT_BLACK);

    m_screen = new SplashScreen(display.width(), display.height());

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<App*>(inst)->uiTask();
    }, "ui_task", 4096, this, 1, nullptr, 0);

    client.onEvent([this](const xr18::XR18Client::Event& e) {
        switch (e.type) {
            case xr18::XR18Client::Event::Type::SearchStarted:
                ESP_LOGI(kLogTag, "Searching for mixer...");
                break;

            case xr18::XR18Client::Event::Type::SearchStopped:
                ESP_LOGI(kLogTag, "Search stopped, mixers found: %d", client.mixers().size());
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

    client.start();
    client.search();

    return true;
}

void App::setScreen(ui::Screen* screen) {
    m_pendingScreen.store(screen);
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




    }
}



    }

}







        }
    }

}









            }

        }
}

