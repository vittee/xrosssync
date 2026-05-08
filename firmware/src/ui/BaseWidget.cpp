#include "BaseWidget.h"

namespace ui {

BaseWidget::BaseWidget(int16_t x, int16_t y, int16_t w, int16_t h)
    : m_x(x), m_y(y), m_w(w), m_h(h)
{

}

void BaseWidget::setPosition(int16_t x, int16_t y) {
    m_x = x;
    m_y = y;
    invalidate();
}

void BaseWidget::setSize(int16_t w, int16_t h) {
    m_w = w;
    m_h = h;
    invalidate();
}

void BaseWidget::setBoundBox(int16_t x, int16_t y, int16_t w, int16_t h) {
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    invalidate();
}

void BaseWidget::invalidate() {
    m_dirty = true;
}

} // namespace ui