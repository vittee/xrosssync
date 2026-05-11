#pragma once

#include <optional>
#include <WString.h>
#include <IPAddress.h>
#include "ui/Screen.h"
#include "ui/Container.h"
#include "ui/widgets/Label.h"
#include "ui/widgets/WifiIcon.h"
#include "ui/widgets/ChannelStripPanel.h"

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

    bool isDirty() override {
        return ui::Screen::isDirty() || true;
    }

    inline void setStatus(const Status& status) {
        m_statusBar.setStatus(status);
    }

    inline void nextPage() { m_panel.nextPage(); };

    inline void prevPage() { m_panel.prevPage(); };

    inline void firstPage() { m_panel.setPage(0); };

    inline int page() const { return m_panel.page(); }

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
        static constexpr uint16_t kBgColor = lgfx::color565(8, 8, 8);
    };

    class MixerPanel : public ui::Container {
    public:
        MixerPanel(App* app, int16_t x, int16_t y, int16_t w, int16_t h);
        ~MixerPanel();

        bool isDirty() override { return true; }

        int pageCount() const { return (m_strips.size() + kChannelPerPage - 1) / kChannelPerPage; }

        void setPage(int page);

        void nextPage() { setPage(m_page + 1); };

        void prevPage() { setPage(m_page - 1); };

        int page() const { return m_page; }

    private:
        int m_page = 0;

        static constexpr uint8_t kChannelPerPage = 8;

        std::vector<ui::widgets::ChannelStripPanel*> m_strips;
    };

    App* m_app;
    StatusBar m_statusBar;
    MixerPanel m_panel;
};
