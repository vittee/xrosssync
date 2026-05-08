#include "App.h"

#include <WiFi.h>

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

App::Display::Display() {
    {
        auto cfg = m_bus.config();
        cfg.pin_wr = 8;
        cfg.pin_rd = 9;
        cfg.pin_rs = 7;
        cfg.pin_d0 = 39;
        cfg.pin_d1 = 40;
        cfg.pin_d2 = 41;
        cfg.pin_d3 = 42;
        cfg.pin_d4 = 45;
        cfg.pin_d5 = 46;
        cfg.pin_d6 = 47;
        cfg.pin_d7 = 48;
        m_bus.config(cfg);
    }

    {
        auto cfg = m_panel.config();
        cfg.pin_cs = 6;
        cfg.pin_rst = 5;
        cfg.pin_busy = -1;
        cfg.offset_rotation = 1;
        cfg.offset_x = 35;
        cfg.readable = false;
        cfg.invert = true;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = false;
        cfg.panel_width = 170;
        cfg.panel_height = 320;
        m_panel.config(cfg);
    }

    {
        auto cfg = m_backlight.config();
        cfg.pin_bl = 38;
        cfg.invert = false;
        cfg.freq = 22000;
        cfg.pwm_channel = 7;
        m_backlight.config(cfg);
    }

    {
        auto cfg = m_touch.config();
        cfg.x_min = 0;
        cfg.x_max = 170;
        cfg.y_min = 0;
        cfg.y_max = 320;
        cfg.pin_rst = 21; // Set this for touch and backlight to work altogether
        cfg.pin_int = PIN_TOUCH_INT;
        cfg.pin_scl = 17;
        cfg.pin_sda = 18;
        m_touch.config(cfg);
    }

    m_panel.setBus(&m_bus);
    m_panel.setLight(&m_backlight);
    m_panel.setTouch(&m_touch);
    this->setPanel(&m_panel);
}

void App::Display::selfTest() {
    int colors[3]{TFT_RED, TFT_GREEN, TFT_BLUE};
    for (int i = 0; i < 3; i++) {
        fillScreen(colors[i]);
        delay(100);
    }

    drawTestPattern();
    delay(1000);
}

void App::Display::drawGrad(int y, int h, uint16_t r, uint16_t g, uint16_t b) {
    int x1 = 0;
    int x2 = 0;
    for (int i = 0; i <= 256; i++) {
        x2 = i * width() / 256;
        if (x1 < x2) {
            fillRect(x1, y, x2 - x1, h, color565(r * i >> 8, g * i >> 8, b * i >> 8));
            x1 = x2;
        }
    }
}

void App::Display::drawTestPattern()
{
    auto w = width();
    auto h = height();

    drawGrad(0, h, 255, 0, 0);
    delay(75);
    drawGrad(0, h, 0, 255, 0);
    delay(75);
    drawGrad(0, h, 0, 0, 255);
    delay(75);
    drawGrad(0, h, 255, 255, 0);
    delay(75);
    drawGrad(0, h, 255, 0, 255);
    delay(75);
    drawGrad(0, h, 0, 255, 255);
    delay(75);
    drawGrad(0, h, 255, 255, 255);
    delay(75);

    auto bw = w / 7;
    fillScreen(TFT_BLACK);
    fillRect(bw * 0 + 2, 0, bw, h, TFT_WHITE);
    fillRect(bw * 1 + 2, 0, bw, h, TFT_YELLOW);
    fillRect(bw * 2 + 2, 0, bw, h, TFT_CYAN);
    fillRect(bw * 3 + 2, 0, bw, h, TFT_GREEN);
    fillRect(bw * 4 + 2, 0, bw, h, TFT_MAGENTA);
    fillRect(bw * 5 + 2, 0, bw, h, TFT_RED);
    fillRect(bw * 6 + 2, 0, bw, h, TFT_BLUE);
}

// static
void IRAM_ATTR App::buttonISR(void* arg) {
    auto info = reinterpret_cast<button_isr_t*>(arg);

    ButtonEvent event = {
        .button = info->button,
        .pressed = digitalRead(info->pin) == LOW
    };

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(info->app->buttonEventQueue, &event, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void App::buttonTask() {
    constexpr TickType_t debounceTicks = pdMS_TO_TICKS(50);

    TickType_t lastChangeTime = 0;
    ButtonEvent event;

    for (;;) {
        if (xQueueReceive(buttonEventQueue, &event, portMAX_DELAY)) {
            auto now = xTaskGetTickCount();
            if (now - lastChangeTime >= debounceTicks) {
                lastChangeTime = now;

                InputEvent inputEvent = {};
                inputEvent.type = InputEventType::Button;
                inputEvent.button = event;

                xQueueSend(inputEventQueue, &inputEvent, portMAX_DELAY);
            }
        }
    }
}

// static
void IRAM_ATTR App::touchISR(void* arg) {
    auto app = reinterpret_cast<App*>(arg);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(app->touchTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void App::touchTask() {
    for (;;) {
        // Wait for touch
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        auto touch = display.touch();
        auto cfg = touch->config();

        uint8_t reg = 0x02;
        uint8_t buffer[5];

        if (lgfx::i2c::transactionWriteRead(cfg.i2c_port, cfg.i2c_addr, &reg, 1, buffer, sizeof(buffer), cfg.freq).has_value()) {
            auto count = buffer[0];
            auto eventFlag = static_cast<TouchFlag>(buffer[1] >> 6);

            if (eventFlag != TouchFlag::Up && count != 1) {
                // event other than UP should have count = 1, otherwise it might be error
                continue;
            }

            int16_t x = ((buffer[1] & 0x0F) << 8) | buffer[2];
            int16_t y = ((buffer[3] & 0x0F) << 8) | buffer[4];

            bool isHome = (x == 85) && (y == 360);

            lgfx::v1::touch_point_t lgfx_tp{ .x = x, .y = y, .size = 0, .id = 1 };
            display.panel()->convertRawXY(&lgfx_tp, 1);
            TouchPoint tp{ .x = lgfx_tp.x, .y = lgfx_tp.y };

            InputEvent inputEvent = {};

            inputEvent.type = isHome ? InputEventType::Home : InputEventType::Touch;

            switch (eventFlag) {
                case TouchFlag::Down:
                    lastTouchPoint = tp;
                    inputEvent.touch.flag = TouchFlag::Down;

                    if (!isHome) {
                        inputEvent.touch.point = tp;
                    }

                    break;

                case TouchFlag::Contact:
                    // Track non-home touch
                    if (isHome) {
                        if (lastInputEvent.type == InputEventType::Touch) {
                            // moving out of home button

                            // explicitly add the touch up and home down event
                            InputEvent explicitEvent = {};

                            explicitEvent.type = InputEventType::Touch;
                            explicitEvent.touch.flag = TouchFlag::Up;
                            explicitEvent.touch.point = tp;
                            xQueueSend(inputEventQueue, &explicitEvent, portMAX_DELAY);

                            explicitEvent.type = InputEventType::Home;
                            explicitEvent.home.flag = TouchFlag::Down;
                            xQueueSend(inputEventQueue, &explicitEvent, portMAX_DELAY);

                            lastInputEvent = explicitEvent;
                        }

                        continue;
                    }

                    if (lastInputEvent.type == InputEventType::Home) {
                        // moving out of home button

                        // explicitly add the touch down and home up event
                        InputEvent explicitEvent = {};

                        explicitEvent.type = InputEventType::Home;
                        explicitEvent.home.flag = TouchFlag::Up;
                        xQueueSend(inputEventQueue, &explicitEvent, portMAX_DELAY);

                        explicitEvent.type = InputEventType::Touch;
                        explicitEvent.touch.flag = TouchFlag::Down;
                        explicitEvent.touch.point = tp;
                        xQueueSend(inputEventQueue, &explicitEvent, portMAX_DELAY);

                        lastInputEvent = explicitEvent;
                    }

                    if ((lastTouchPoint.x == tp.x) && (lastTouchPoint.y == tp.y)) {
                        // not move
                        continue;
                    }

                    // Move

                    inputEvent.touch.flag = TouchFlag::Contact;
                    inputEvent.touch.point = tp;

                    lastTouchPoint = tp;

                    break;

                case TouchFlag::Up:
                    inputEvent.touch.flag = TouchFlag::Up;

                    if (!isHome) {
                        inputEvent.touch.point = lastTouchPoint;
                    }

                    break;
            }

            lastInputEvent = inputEvent;
            xQueueSend(inputEventQueue, &inputEvent, portMAX_DELAY);
        }
    }
}

void App::handleInput() {
    InputEvent event{};

    for (;;) {
        if (xQueueReceive(inputEventQueue, &event, portMAX_DELAY)) {
            switch (event.type) {
                case InputEventType::Button:
                    ESP_LOGI(kLogTag, "Button%d %d", event.button.button, event.button.pressed);
                    break;

                case InputEventType::Home:
                    ESP_LOGI(kLogTag, "Home %d", static_cast<uint8_t>(event.home.flag));
                    break;

                case InputEventType::Touch:
                    ESP_LOGI(kLogTag, "Touch flag=%d, x=%d, y=%d", static_cast<uint8_t>(event.touch.flag), event.touch.point.x, event.touch.point.y);
                    break;

            }
        }
    }
}