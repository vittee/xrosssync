#include "App.h"

namespace {
    static constexpr auto kLogTag = "App";
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
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        auto touch = display.touch();
        auto cfg = touch->config();

        uint8_t reg = 0x02;
        uint8_t buffer[5];

        if (lgfx::i2c::transactionWriteRead(cfg.i2c_port, cfg.i2c_addr, &reg, 1, buffer, sizeof(buffer), cfg.freq).has_value()) {
            auto count = buffer[0];
            auto eventFlag = static_cast<TouchFlag>(buffer[1] >> 6);

            if (eventFlag != TouchFlag::Up && count != 1) {
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
                    if (isHome) {
                        if (lastInputEvent.type == InputEventType::Touch) {
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
                        continue;
                    }

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
