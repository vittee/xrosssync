#include "Widget.h"
#include <esp_log.h>

namespace ui {

Widget::Widget(int16_t x, int16_t y, int16_t w, int16_t h, lgfx::color_depth_t colorDepth)
    : BaseWidget(x, y, w, h)
{
    m_sprite.setColorDepth(colorDepth);

    if (!m_sprite.createSprite(w, h)) {
        ESP_LOGE("Widget", "createSprite failed (%dx%d, depth=%d)", w, h, (int)colorDepth);
    }
}

void Widget::draw(LovyanGFX& dst, int16_t offsetX, int16_t offsetY) {
    if (m_dirty) {
        render();
        clearDirty();
    }

    m_sprite.pushSprite(&dst, m_x + offsetX, m_y + offsetY);
}

} // namespace ui
