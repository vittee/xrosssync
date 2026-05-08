#pragma once

#include <WString.h>
#include "../Widget.h"

namespace ui {
namespace widgets {

class Label : public Widget {
public:
    enum class HAlign { Left, Center, Right };
    enum class VAlign { Top, Middle, Bottom };

    Label(
        int16_t x, int16_t y, const String& text,
        const lgfx::IFont* font = nullptr,
        lgfx::color_depth_t colorDepth = lgfx::color_depth_t::rgb565_2Byte
    );

    Label(
        int16_t x, int16_t y, int16_t w, int16_t h,
        const String& text = "",
        const lgfx::IFont* font = nullptr,
        lgfx::color_depth_t colorDepth = lgfx::color_depth_t::rgb565_2Byte
    );

    void setText(const String& text);
    const String& text() const { return m_text; }

    void setFont(const lgfx::IFont* font);
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
    const lgfx::IFont* m_font;
    HAlign m_hAlign = HAlign::Left;
    VAlign m_vAlign = VAlign::Top;
    int32_t m_textColor = TFT_WHITE;
    int32_t m_bgColor = TFT_BLACK;
};

} // namespace widgets
} // namespace ui
