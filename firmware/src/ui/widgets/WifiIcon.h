#pragma once

#include "ui/Widget.h"

namespace ui {
namespace widgets {

class WifiIcon : public Widget {
public:
    WifiIcon(int16_t x, int16_t y, int16_t w, int16_t h);

    void setRSSI(int8_t rssi = -100);
    void setBackgroundColor(int32_t color);

protected:
    void render() override;

private:
    int8_t m_rssi = -100;
    int32_t m_bgColor = 0;

    static constexpr lgfx::rgb565_t kColorConnected{0x07E0};
    static constexpr lgfx::rgb565_t kColorDim{0x4208};
};

} // namespace widgets
} // namespace ui
