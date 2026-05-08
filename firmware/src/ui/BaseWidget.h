#pragma once

#include <LovyanGFX.hpp>

namespace ui {

using namespace lgfx;

class BaseWidget {
public:
    BaseWidget(int16_t x, int16_t y, int16_t w, int16_t h);
    virtual ~BaseWidget() = default;

    int16_t x() const { return m_x; }
    int16_t y() const { return m_y; }
    int16_t w() const { return m_w; }
    int16_t h() const { return m_h; }

    void setPosition(int16_t x, int16_t y);
    void setSize(int16_t w, int16_t h);
    void setBoundBox(int16_t x, int16_t y, int16_t w, int16_t h);

    virtual bool isDirty() const { return m_dirty; }
    void invalidate();

    virtual void draw(LovyanGFX& dst, int16_t offsetX = 0, int16_t offsetY = 0) = 0;

protected:
    void clearDirty() { m_dirty = false; }

    int16_t m_x;
    int16_t m_y;
    int16_t m_w;
    int16_t m_h;

    bool m_dirty = true;
};

} // namespace ui
