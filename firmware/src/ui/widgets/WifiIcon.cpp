#include "WifiIcon.h"

namespace ui {
namespace widgets {

WifiIcon::WifiIcon(int16_t x, int16_t y, int16_t w, int16_t h)
    : Widget(x, y, w, h)
{

}

void WifiIcon::setRSSI(int8_t rssi) {
    m_rssi = rssi;
    invalidate();
}

void WifiIcon::setBackgroundColor(int32_t color) {
    if (m_bgColor == color) {
        return;
    }

    m_bgColor = color;
    invalidate();
}

void WifiIcon::render() {
    m_sprite.fillSprite(m_bgColor);

    int16_t cx = m_w / 2;
    int16_t cy = m_h;

    m_sprite.fillArc(cx, cy, 13, 14, 180 + 55, -55, m_rssi >= -55 ? kColorConnected : kColorDim);
    m_sprite.fillArc(cx, cy, 10, 11, 180 + 55, -55, m_rssi >= -65 ? kColorConnected : kColorDim);
    m_sprite.fillArc(cx, cy, 7, 8, 180 + 55, -55, m_rssi >= -75 ? kColorConnected : kColorDim);
    m_sprite.fillCircle(cx, cy - 4, 1, m_rssi >= -90 ? kColorConnected : kColorDim);
}

} // namespace widgets
} // namespace ui
