#pragma once

#include <optional>
#include <WString.h>
#include <IPAddress.h>
#include "ui/Screen.h"
#include "ui/Container.h"
#include "ui/widgets/Label.h"
#include "ui/widgets/WifiIcon.h"

class App;

class MainScreen : public ui::Screen {
public:
    static constexpr int16_t kStatusBarHeight = 18;

    struct Status {
        enum class Type : uint8_t {
            NoWifi,
            NotConencted,
            Searching,
            Connected,
        };

        Type type = Type::NoWifi;
    };

    MainScreen(App* app, int16_t w, int16_t h);

    inline void setStatus(const Status& status) {
        m_statusBar.setStatus(status);
    }

private:
    class StatusBar : public ui::Container {
    public:
        StatusBar(App* app, int16_t w, int16_t h);

        void setStatus(const Status& status);

        bool isDirty() override;

    private:
        void updateStatusLabel();

        void updateRSSI();

        ui::widgets::Label m_infoLabel;
        ui::widgets::WifiIcon m_wifiIcon;

        App* m_app;
        Status m_status;
        uint8_t  m_cycleIndex = 0;
        uint32_t m_lastCycleMs = 0;
        uint32_t m_lastWiFiMs = 0;

        static constexpr int16_t kWifiWidth = 20;
        static constexpr int16_t kXMargin = 2;
        static constexpr uint16_t kBgColor = lgfx::v1::color565(8, 8, 8);
    };

    class Panel : public ui::Widget {
    public:
        Panel(int16_t x, int16_t y, int16_t w, int16_t h)
            : Widget(x, y, w, h)
        {

        }

        void render() override {
            int32_t color[8]{
                TFT_RED, TFT_GREEN, TFT_BLUE, TFT_MAGENTA,
                TFT_MAROON, TFT_GREENYELLOW, TFT_SKYBLUE, TFT_PURPLE
            };

            auto count = 8;
            auto chw = 320 / count;

            for (int i = 0; i < count; i++) {
                m_sprite.drawRect(i * chw, 0, chw, m_h, color[i]);
            }
        }
    };

    App* m_app;
    StatusBar m_statusBar;
    Panel m_panel;
};
