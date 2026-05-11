#pragma once

#include <vector>
#include "BaseWidget.h"

namespace ui {

using namespace lgfx;

class Container : public BaseWidget {
public:
    Container(int16_t x, int16_t y, int16_t w, int16_t h);

    void add(BaseWidget* child);
    void remove(BaseWidget* child);
    void clear();

    bool isDirty() override;
    void draw(LovyanGFX& dst, int16_t offsetX = 0, int16_t offsetY = 0) override;

protected:
    std::vector<BaseWidget*> m_children;
};

} // namespace ui
