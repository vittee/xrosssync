#include "MainScreen.h"
#include "App.h"
#include <WiFi.h>

MainScreen::MainScreen(App* app, int16_t w, int16_t h)
    : ui::Screen(w, h),
    m_app(app),
    m_statusBar(app, w, kStatusBarHeight),
    m_panel(0, kStatusBarHeight + 2, w, h - kStatusBarHeight - 4)
{
    add(&m_statusBar);
    add(&m_panel);
}

MainScreen::StatusBar::StatusBar(App* app, int16_t w, int16_t h)
    : ui::Container(0, 0, w, h),
    m_app(app),
    m_infoLabel(kXMargin, 0, w - kWifiWidth, h, "", &lgfx::fonts::Font0),
    m_wifiIcon(w - kWifiWidth - kXMargin, 0, kWifiWidth, h)
{
    m_infoLabel.setVAlign(ui::widgets::Label::VAlign::Middle);
    m_infoLabel.setTextColor(TFT_WHITE);
    m_infoLabel.setBackgroundColor(kBgColor);

    m_wifiIcon.setBackgroundColor(kBgColor);

    add(&m_infoLabel);
    add(&m_wifiIcon);

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