#include "Screen.h"

namespace ui {

Screen::Screen(int16_t w, int16_t h)
    : Container(0, 0, w, h)
{

}

void Screen::render(lgfx::LovyanGFX& dst) {
    if (!isDirty()) {
        return;
    }

    draw(dst);
}

} // namespace ui
