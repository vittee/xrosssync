#include "Label.h"

namespace ui {
namespace widgets {

Label::Label(int16_t x, int16_t y, const String& text, const lgfx::IFont* font, lgfx::color_depth_t colorDepth)
    : Label(x, y, measureWidth(text, font), measureHeight(font), text, font, colorDepth)
{

}

Label::Label(int16_t x, int16_t y, int16_t w, int16_t h, const String& text, const lgfx::IFont* font, lgfx::color_depth_t colorDepth)
    : Widget(x, y, w, h, colorDepth),
    m_text(text),
    m_font(font ? font: &lgfx::fonts::Font4)
{
}

void Label::setText(const String& text) {
    if (m_text == text) {
        return;
    }

    m_text = text;
    invalidate();
}

void Label::setFont(const lgfx::IFont* font) {
    m_font = font;
    invalidate();
}

void Label::setHAlign(HAlign align) {
    m_hAlign = align;
    invalidate();
}

void Label::setVAlign(VAlign align) {
    m_vAlign = align;
    invalidate();
}

void Label::setTextColor(uint32_t color) {
    m_textColor = color;
    invalidate();
}

void Label::setBackgroundColor(uint32_t color) {
    m_bgColor = color;
    invalidate();
}

void Label::setPaletteColor(uint8_t index, lgfx::rgb565_t color) {
    m_sprite.setPaletteColor(index, color);
    invalidate();
}

void Label::render() {
    static constexpr textdatum_t kDatumTable[3][3] = {
        { textdatum_t::top_left,    textdatum_t::top_center,    textdatum_t::top_right    },
        { textdatum_t::middle_left, textdatum_t::middle_center, textdatum_t::middle_right },
        { textdatum_t::bottom_left, textdatum_t::bottom_center, textdatum_t::bottom_right },
    };

    int16_t tx;
    switch (m_hAlign) {
        case HAlign::Left:
            tx = 0;
            break;
        case HAlign::Center:
            tx = m_w / 2;
            break;
        case HAlign::Right:
            tx = m_w;
            break;
    }

    int16_t ty;
    switch (m_vAlign) {
        case VAlign::Top:
            ty = 0;
            break;
        case VAlign::Middle:
            ty = m_h / 2;
            break;
        case VAlign::Bottom:
            ty = m_h;
            break;
    }

    m_sprite.setFont(m_font);
    m_sprite.setTextDatum(kDatumTable[(int)m_vAlign][(int)m_hAlign]);

    m_sprite.fillScreen(m_bgColor);
    m_sprite.setTextColor(m_textColor, m_bgColor);

    m_sprite.drawString(m_text.c_str(), tx, ty);
}

int16_t Label::measureWidth(const String& text, const lgfx::IFont* font) {
    LGFX_Sprite tmp;
    tmp.setFont(font);
    return tmp.textWidth(text.c_str());
}

int16_t Label::measureHeight(const lgfx::IFont* font) {
    LGFX_Sprite tmp;
    tmp.setFont(font);
    return tmp.fontHeight();
}

} // namespace widgets
} // namespace ui
