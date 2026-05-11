#pragma once

#include "ui/Widget.h"
#include "xr18/ChannelStrip.h"

namespace ui {
namespace widgets {

class ChannelStripPanel : public ui::Widget {
public:
    ChannelStripPanel(int16_t x, int16_t y, int16_t w, int16_t h, xr18::ChannelStrip* index);

    ~ChannelStripPanel();

    bool isDirty() override { return ui::Widget::isDirty() | overflowSize > 0; }
protected:
    void render() override;

private:
    void renderNameSprite();

    xr18::ChannelStrip* m_strip = nullptr;
    LGFX_Sprite m_nameSprite;
    int scrollPos = 0;
    int overflowSize = 0;
    int scrollDir = -1;
    uint32_t lastScroll = 0;
    uint32_t scrollResumeAt = 0;
};

} // namespace widgets
} // namespace ui
