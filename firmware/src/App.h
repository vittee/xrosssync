#pragma once

#include <atomic>
#include <Arduino.h>
#include <LovyanGFX.hpp>

#ifdef XROSSSYNC_DEBUG
#include "logging/dbp.h"
#endif

#include "xr18/XR18Client.h"
#include "ui/Screen.h"

class MainScreen;

class App {
public:
    App();

    bool init();

    inline xr18::XR18Client& client() { return *m_client; }

private:
    class Display : public lgfx::LGFX_Device {
    public:
        Display();

    private:
        friend class App;

        void selfTest();
        void drawGrad(int y, int h, uint16_t r, uint16_t g, uint16_t b);
        void drawTestPattern();

        lgfx::Bus_Parallel8 m_bus;
        lgfx::Panel_ST7789 m_panel;
        lgfx::Light_PWM m_backlight;
        lgfx::Touch_CST816S m_touch;
    };

    struct button_isr_t {
        App* app;
        int button;
        int pin;
    };

    struct ButtonEvent {
        int button;
        bool pressed;
    };

    enum class TouchFlag : uint8_t {
        Down = 0,
        Up = 1,
        Contact = 2
    };

    struct TouchPoint {
        int16_t x;
        int16_t y;
    };

    struct TouchEvent {
        TouchFlag flag;
        TouchPoint point;
    };

    struct HomeEvent {
        TouchFlag flag;
    };

    struct InputEvent {
        enum class Type {
            Button,
            Home,
            Touch
        };

        Type type;

        union {
            ButtonEvent button;
            HomeEvent home;
            TouchEvent touch;
        };
    };

private:

    void buttonTask();
    static void buttonISR(void* arg);

    void touchTask();
    static void touchISR(void* arg);

    enum class AppState : uint8_t {
        Init,
        Normal,
        SelectMixer,
        WifiConfig,
    };

    struct AppEvent {
        enum class Type : uint8_t {
            WiFiDisconnected,
            WiFiConnected,
            MixerSearchStarted,
            MixerSearchStop,
            MixerDisconnected,
            MixerConnected,
            MixerSynchronized,
            Input
        };

        Type type;

        union {
            InputEvent input;
        };

    };

    void handleInput();
    void uiTask();
    void appTask();
    void setScreen(ui::Screen* screen);
    void postAppEvent(AppEvent event);

    AppState stateInit(bool transited, TickType_t& delay);
    AppState stateNormal(bool transited, TickType_t& delay);
    AppState stateSelectMixer(bool transited, TickType_t& delay);
    AppState stateWifiConfig(bool transited, TickType_t& delay);

    Display m_display;
    ui::Screen* m_screen = nullptr;
    std::atomic<ui::Screen*> m_pendingScreen{nullptr};

    ui::Screen* m_splashScreen = nullptr;
    MainScreen* m_mainScreen = nullptr;
    QueueHandle_t m_appEventQueue = nullptr;

    QueueHandle_t buttonEventQueue;
    button_isr_t buttons[2]{
        { .app = this, .button = 1, .pin = PIN_BUTTON_1 },
        { .app = this, .button = 2, .pin = PIN_BUTTON_2 }
    };

    TaskHandle_t touchTaskHandle = nullptr;
    TouchPoint lastTouchPoint;

    QueueHandle_t inputEventQueue;
    InputEvent lastInputEvent{};

    xr18::XR18Client* m_client;

    static constexpr uint8_t PIN_BUTTON_1 = 0;
    static constexpr uint8_t PIN_BUTTON_2 = 14;

    static constexpr uint8_t PIN_TOUCH_INT = 16;

    static constexpr auto kLogTag = "App";
};