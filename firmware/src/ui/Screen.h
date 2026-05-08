#pragma once

#include "Container.h"

namespace ui {

using namespace lgfx;

class Screen : public Container {
public:
    Screen(int16_t w, int16_t h);

    void render(LovyanGFX& dst);
};

} // namespace ui
