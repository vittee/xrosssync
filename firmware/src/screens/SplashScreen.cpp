#include "SplashScreen.h"

SplashScreen::SplashScreen(int16_t w, int16_t h)
    : ui::Screen(w, h),
    m_xross(0, 0, "XRoss", &lgfx::fonts::FreeSansBold18pt7b, lgfx::color_depth_t::palette_1bit),
    m_sync(m_xross.w() + 4, 0, "Sync", &lgfx::fonts::FreeSansBold18pt7b, lgfx::color_depth_t::palette_1bit),
    m_row(0, 0, 0, 0)
{
    m_xross.setPaletteColor(0, TFT_BLACK);
    m_xross.setPaletteColor(1, TFT_WHITE);

    m_sync.setPaletteColor(0, TFT_BLACK);
    m_sync.setPaletteColor(1, TFT_ORANGE);

    m_row.add(&m_xross);
    m_row.add(&m_sync);

    auto rw = m_xross.w() + 4 + m_sync.w();
    auto rh = m_xross.h();

    m_row.setBoundBox(
        (w - rw) / 2,
        (h - rh) / 2,
        rw,
        rh
    );

    add(&m_row);
}
