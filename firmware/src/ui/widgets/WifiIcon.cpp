#include "WifiIcon.h"

namespace ui {
namespace widgets {

WifiIcon::WifiIcon(int16_t x, int16_t y, int16_t w, int16_t h)
    : Widget(x, y, w, h, color_depth_t::palette_2bit)
{
    m_sprite.setPaletteColor(0, TFT_BLACK);
    m_sprite.setPaletteColor(1, color565(120, 120, 120));
    m_sprite.setPaletteColor(2, TFT_GREEN);
}

void WifiIcon::setRSSI(int8_t rssi) {
    m_rssi = rssi;
    invalidate();
}

void WifiIcon::setBackgroundColor(int32_t color) {
    m_sprite.setPaletteColor(0, color);
    invalidate();
}

void WifiIcon::render() {
    m_sprite.fillSprite(0);

    int16_t cx = m_w / 2;
    int16_t cy = m_h;

    m_sprite.fillArc(cx, cy, 13, 14, 180 + 55, -55, m_rssi >= -55 ? 2 : 1);
    m_sprite.fillArc(cx, cy, 10, 11, 180 + 55, -55, m_rssi >= -65 ? 2 : 1);
    m_sprite.fillArc(cx, cy, 7, 8, 180 + 55, -55, m_rssi >= -75 ? 2 : 1);
    m_sprite.fillCircle(cx, cy - 4, 1, m_rssi >= -90 ? 2 : 1);
}

} // namespace widgets
} // namespace ui
