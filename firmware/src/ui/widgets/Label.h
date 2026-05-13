#pragma once

#include <WString.h>
#include "ui/Widget.h"

namespace ui {
namespace widgets {

class Label : public Widget {
public:
    enum class HAlign { Left, Center, Right };
    enum class VAlign { Top, Middle, Bottom };

    Label(
        int16_t x, int16_t y, const String& text,
        const IFont* font = nullptr,
        color_depth_t colorDepth = color_depth_t::rgb565_2Byte
    );

    Label(
        int16_t x, int16_t y, int16_t w, int16_t h,
        const String& text = "",
        const IFont* font = nullptr,
        color_depth_t colorDepth = color_depth_t::rgb565_2Byte
    );

    void setText(const String& text);
    const String& text() const { return m_text; }

    void setFont(const IFont* font);
    void setTextSize(float size);
    void setHAlign(HAlign align);
    void setVAlign(VAlign align);
    void setTextColor(int32_t color);
    void setBackgroundColor(int32_t color);
    void setPaletteColor(uint8_t index, lgfx::rgb565_t color);

protected:
    void render() override;

private:
    static int16_t measureWidth(const String& text, const lgfx::IFont* font);
    static int16_t measureHeight(const lgfx::IFont* font);

    String m_text;
    const IFont* m_font;
    HAlign m_hAlign = HAlign::Left;
    VAlign m_vAlign = VAlign::Top;
    int32_t m_textColor = TFT_WHITE;
    int32_t m_bgColor = TFT_BLACK;
};

} // namespace widgets
} // namespace ui
