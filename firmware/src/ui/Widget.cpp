#include "Widget.h"

namespace ui {

Widget::Widget(int16_t x, int16_t y, int16_t w, int16_t h)
    : BaseWidget(x, y, w, h)
{
    m_sprite.createSprite(w, h);
}

void Widget::draw(LovyanGFX& dst, int16_t offsetX, int16_t offsetY) {
    if (m_dirty) {
        render();
        clearDirty();
    }

    m_sprite.pushSprite(&dst, m_x + offsetX, m_y + offsetY);
}

} // namespace ui
