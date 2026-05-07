#pragma once

#include "BaseWidget.h"

namespace ui {

using namespace lgfx;

class Widget : public BaseWidget {
public:
    Widget(int16_t x, int16_t y, int16_t w, int16_t h);

    void draw(LovyanGFX& dst, int16_t offsetX = 0, int16_t offsetY = 0) override;

protected:
    virtual void render() = 0;

    LGFX_Sprite m_sprite;
};

} // namespace ui
