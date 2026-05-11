#include "Container.h"

namespace ui {

Container::Container(int16_t x, int16_t y, int16_t w, int16_t h)
    : BaseWidget(x, y, w, h)
{

}

void Container::add(BaseWidget* child) {
    m_children.push_back(child);
}

void Container::clear() {
    m_children.clear();
}

bool Container::isDirty() {
    bool result = false;

    for (auto* child : m_children) {
        if (child->isDirty()) {
            result = true;
        }
    }

    return result;
}

void Container::draw(LovyanGFX& dst, int16_t offsetX, int16_t offsetY) {
    for (auto* child : m_children) {
        child->draw(dst, m_x + offsetX, m_y + offsetY);
    }
}

} // namespace ui
