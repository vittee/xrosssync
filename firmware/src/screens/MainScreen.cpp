#include "MainScreen.h"

MainScreen::MainScreen(int16_t w, int16_t h)
    : ui::Screen(w, h),
    m_statusBar(0, 0, w, kStatusBarHeight, "XRossSync", &fonts::Font2),
    m_panel(0, kStatusBarHeight, w, h - kStatusBarHeight)
{
    m_statusBar.setTextColor(TFT_WHITE);
    m_statusBar.setBackgroundColor(TFT_DARKGRAY);

    add(&m_statusBar);
    add(&m_panel);
}
