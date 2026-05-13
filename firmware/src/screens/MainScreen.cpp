#include <WiFi.h>
#include "MainScreen.h"
#include "App.h"
#include "psram.h"

MainScreen::MainScreen(App* app, int16_t w, int16_t h)
    : ui::Screen(w, h),
    m_app(app),
    m_statusBar(app, w, kStatusBarHeight),
    m_panel(app, 0, kStatusBarHeight + 2, w, h - kStatusBarHeight - 4)
{
    add(&m_statusBar);
    add(&m_panel);

    updatePageLabel();
}

MainScreen::StatusBar::StatusBar(App* app, int16_t w, int16_t h)
    : ui::Container(0, 0, w, h),
    m_app(app),
    m_infoLabel(kXMargin, 0, w - kWifiIconW - kPageLabelW - kXMargin, h, "", &lgfx::fonts::Font0, lgfx::color_depth_t::palette_1bit),
    m_wifiIcon(w - kWifiIconW - kXMargin, 0, kWifiIconW, h),
    m_pageLabel(w - kWifiIconW - kXMargin - kPageLabelW - kXMargin, 0, kPageLabelW, h, "", &lgfx::fonts::Font0, lgfx::color_depth_t::palette_1bit)
{
    m_infoLabel.setPaletteColor(0, kBgColor);
    m_infoLabel.setPaletteColor(1, TFT_WHITE);
    m_infoLabel.setVAlign(ui::widgets::Label::VAlign::Middle);
    m_infoLabel.setTextColor(1);
    m_infoLabel.setBackgroundColor(0);

    m_pageLabel.setPaletteColor(0, kBgColor);
    m_pageLabel.setPaletteColor(1, TFT_CYAN);
    m_pageLabel.setVAlign(ui::widgets::Label::VAlign::Middle);
    m_pageLabel.setHAlign(ui::widgets::Label::HAlign::Right);
    m_pageLabel.setTextColor(1);
    m_pageLabel.setBackgroundColor(0);

    m_wifiIcon.setBackgroundColor(kBgColor);

    add(&m_infoLabel);
    add(&m_wifiIcon);
    add(&m_pageLabel);

    setStatus({ Status::Type::NoWifi });
}

void MainScreen::StatusBar::setStatus(const Status& status) {
    m_status = status;
    m_cycleIndex = 0;
    m_lastCycleMs = millis();

    switch (status.type) {
        case Status::Type::NoWifi:
            m_infoLabel.setText("No WiFi");
            break;

        case Status::Type::NotConencted:
            m_infoLabel.setText("Not connected");
            break;

        case Status::Type::Searching:
            m_infoLabel.setText("Searching...");
            break;

        case Status::Type::Connected:
            updateStatusLabel();
            return;
    }

    updateRSSI();
}

#ifdef XROSSSYNC_DEBUG
static constexpr uint8_t kStatusLines = 5;
#else
static constexpr uint8_t kStatusLines = 4;
#endif

void MainScreen::StatusBar::updateStatusLabel() {
    auto mixerInfo = m_app->client().mixerInfo().value();

    switch (m_cycleIndex % kStatusLines) {
        case 0:
            m_infoLabel.setText(mixerInfo.name);
            break;
        case 1:
            m_infoLabel.setText(mixerInfo.model);
            break;
        case 2:
            m_infoLabel.setText(mixerInfo.ip.toString());
            break;
        case 3:
            m_infoLabel.setText("Via " + WiFi.SSID());
            break;

        #ifdef XROSSSYNC_DEBUG
        case 4:
            m_infoLabel.setText("Heap: " + String(ESP.getHeapSize() / 1024.0f, 2) + "KiB, Free: " + String(ESP.getFreeHeap() / 1024.0f, 2) + "KiB");
            break;
        #endif
    }
}

void MainScreen::StatusBar::updateRSSI() {
    m_wifiIcon.setRSSI(m_status.type != Status::Type::NoWifi ? WiFi.RSSI() : -100);
}

void MainScreen::StatusBar::updatePageLabel(int pageNumber, int count) {
    m_pageLabel.setText(String(pageNumber) + "/" + String(count));
}

bool MainScreen::StatusBar::isDirty() {
    bool result = ui::Container::isDirty();

    if (m_status.type == Status::Type::Connected) {
        auto now = millis();

        if (now - m_lastWiFiMs >= 1e3) {
            m_lastWiFiMs = now;
            updateRSSI();
            result = true;
        }

        if (now - m_lastCycleMs >= 5e3) {
            m_lastCycleMs = now;
            m_cycleIndex = (m_cycleIndex + 1) % kStatusLines;
            updateStatusLabel();
            result = true;
        }
    }

    return result;
}

MainScreen::MixerPanel::MixerPanel(App* app, int16_t x, int16_t y, int16_t w, int16_t h)
    : Container(x, y, w, h)
{
    // TODO: Ideally we should have a list of strip configs somewhere
    // then only create 8 instances of ChannelStripPanel (and the actual ChannelStrip) for a page

    auto constexpr count = kChannelPerPage * 5;
    m_strips.reserve(count);

    int16_t stripW = w / kChannelPerPage;

    for (uint8_t i = 0; i < count; i++) {
        void* mem = heap_caps_malloc(sizeof(ui::widgets::ChannelStripPanel), MALLOC_CAP_SPIRAM);
        auto panel = psram_new<ui::widgets::ChannelStripPanel>(
            (i % kChannelPerPage) * stripW, 0,
            stripW,
            h, app->client().createChannelStrip(
                static_cast<xr18::ChannelStrip::StripIndex>(i)
            )
        );

        m_strips.emplace_back(panel);
    }

    setPage(0);
}

MainScreen::MixerPanel::~MixerPanel() {
    for (uint8_t i = 0; i < m_strips.size(); i++) {
        psram_delete(m_strips[i]);
    }
}

void MainScreen::MixerPanel::setPage(int page) {
    ESP_LOGD("MainScreen", "setPage(%d)", page);

    if (page >= 0 && page < pageCount()) {
        clear();

        for (int i = 0; i < kChannelPerPage; i++) {
            int index = page * kChannelPerPage + i;
            auto strip = m_strips[index];

            if (strip) {
                add(strip);
            }
        }

        m_page = page;
    }
}