#include "ChannelStripPanel.h"
#include "psram.h"

namespace ui {
namespace widgets {

static constexpr uint16_t kPalette[1 << 4] = {
    // Channel colors
    color565(30, 30, 30),
    TFT_RED,
    TFT_GREEN,
    TFT_YELLOW,
    TFT_BLUE,
    TFT_MAGENTA,
    TFT_CYAN,
    TFT_WHITE,
    //
    TFT_DARKGREY,
    TFT_BLACK,
    0, // 11 - Reserved
    0, // 12 - Reserved
    0, // 13 - Reserved
    0, // 14 - Reserved
    0 // 15 - Reserved
};

static constexpr int kColorDarkGrey = 0;
static constexpr int kColorRed = 1;
static constexpr int kColorGreen = 2;
static constexpr int kColorYellow = 3;
static constexpr int kColorBlue = 4;
static constexpr int kColorMargenta = 5;
static constexpr int kColorCyan = 6;
static constexpr int kColorWhite = 7;
static constexpr int kColorGrey = 8;
static constexpr int kColorBlack = 9;

static constexpr int16_t kTopButtonsH = 12;
static constexpr int16_t kTopInfoH = 12;
static constexpr int16_t kTopBarH = kTopButtonsH + kTopInfoH;
static constexpr int16_t kNameW = 14;
static constexpr int16_t kFaderW = 4;
static constexpr int16_t kFaderTop = kTopBarH + 2;

ChannelStripPanel::ChannelStripPanel(int16_t x, int16_t y, int16_t w, int16_t h, xr18::ChannelStrip* strip)
    : Widget(x, y, w, h, color_depth_t::palette_4bit),
    m_strip(strip)
{
    m_sprite.setFont(&lgfx::fonts::Font0);

    for (int i = 0; i < 16; i++) {
        m_sprite.setPaletteColor(i, kPalette[i]);
    }

    m_nameSprite.setColorDepth(lgfx::color_depth_t::palette_4bit);
    m_nameSprite.setFont(&lgfx::fonts::Font2);

    if (m_strip) {
        renderNameSprite();

        m_strip->onEvent([this](xr18::ChannelStrip::ParamId paramId, xr18::nodes::params::Param*) {
            switch (paramId) {
                case xr18::ChannelStrip::ParamId::Name:
                case xr18::ChannelStrip::ParamId::Color:
                    renderNameSprite();
                    break;
            }

            invalidate();
        });
    }
}

ChannelStripPanel::~ChannelStripPanel() {
    if (m_strip) {
        psram_delete(m_strip);
    }
}

void ChannelStripPanel::renderNameSprite() {
    String nameStr = m_strip->internalName();
    String configName = m_strip->name().formatValue();

    if (strlen(configName.c_str())) {
        nameStr += " - " + configName;
    }

    auto textW = static_cast<int16_t>(m_nameSprite.textWidth(nameStr)) + 2;
    auto textH = static_cast<int16_t>(m_nameSprite.fontHeight()) + 2;

    overflowSize = max(0, textW - (m_h - kFaderTop));
    if (!overflowSize) {
        scrollPos = 0;
        scrollDir = -1;
        lastScroll = 0;
        scrollResumeAt = 0;
    }

    if (!m_nameSprite.createSprite(textW, textH)) {
        return;
    }

    for (int i = 0; i < 8; i++) {
        m_nameSprite.setPaletteColor(i, kPalette[i]);
    }

    auto colorCode = m_strip->color().value();
    auto colorIndex = colorCode & 7;
    bool inverted = colorCode & 8;
    auto fgColor = inverted ? colorIndex : (colorIndex == 0 || colorIndex == 1 || colorIndex == 4) ? kColorWhite : kColorDarkGrey;
    auto bgColor = !inverted ? colorIndex : (colorIndex == 0) ? kColorWhite : kColorDarkGrey;

    m_nameSprite.fillSprite(bgColor);
    m_nameSprite.setTextColor(fgColor);
    m_nameSprite.drawString(nameStr.c_str(), 1, 1);
    m_nameSprite.setPivot(m_nameSprite.width() / 2.0f, m_nameSprite.height() / 2.0f);
}

void ChannelStripPanel::render() {


    m_sprite.fillScreen(kColorBlack);
    m_sprite.drawFastVLine(m_w - 1, 0, m_h, kColorDarkGrey);

    if (!m_strip) {
        m_sprite.setTextDatum(textdatum_t::middle_center);
        m_sprite.setTextColor(kColorDarkGrey);
        m_sprite.drawString("N/A", m_w / 2, m_h / 2);
        return;
    }

    const bool on = m_strip->on().value() != 0;
    const bool soloed = m_strip->solosw().value() != 0;

    if (on) {
        m_sprite.fillRect(0, 0, m_w / 2 - 1, kTopButtonsH, kColorGreen);
        m_sprite.setTextDatum(textdatum_t::middle_center);
        m_sprite.setTextColor(kColorBlack);
        m_sprite.drawString("ON", m_w / 4, kTopButtonsH / 2);
    } else {
        m_sprite.fillRect(0, 0, m_w / 2 - 1, kTopButtonsH, kColorRed);
        m_sprite.setTextDatum(textdatum_t::middle_center);
        m_sprite.setTextColor(kColorWhite);
        m_sprite.drawString("M", m_w / 4, kTopButtonsH / 2);
    }

    {
        if (soloed) {
            m_sprite.drawRect(m_w / 2, 0, m_w / 2 - 1, kTopButtonsH, kColorYellow);
            m_sprite.setTextColor(kColorYellow);
        } else {
            m_sprite.drawRect(m_w / 2, 0, m_w / 2 - 1, kTopButtonsH, kColorGrey);
            m_sprite.setTextColor(kColorGrey);
        }

        m_sprite.setTextDatum(textdatum_t::middle_center);
        m_sprite.drawString("S", m_w * 3 / 4 + 1, kTopButtonsH / 2);
    }

    {
        String dBStr = m_strip->fader().formatValue();
        auto dBStrW = m_sprite.textWidth(dBStr);

        m_sprite.setTextColor(kColorWhite);
        m_sprite.setTextDatum(textdatum_t::middle_left);
        m_sprite.drawString(dBStr, 20 - dBStrW / 2, kTopButtonsH + kTopInfoH / 2);
    }

    auto trackX = kNameW + 1;
    auto trackH = m_h - kFaderTop;

    // fader track
    m_sprite.fillRect(trackX, kFaderTop, kFaderW, trackH, kColorDarkGrey);

    auto faderH = static_cast<int16_t>(m_strip->fader().getNormalizedValue() * trackH);

    if (faderH > 0) {
        // fader level
        m_sprite.fillRect(trackX, m_h - faderH, kFaderW, faderH, on ? kColorGreen : kColorGrey);
    }

    if (overflowSize) {
        auto now = ::millis();

        if (now - lastScroll > 50) {
            lastScroll = now;

            if ((int32_t)(now - scrollResumeAt) >= 0) {
                scrollPos += scrollDir;

                if (scrollDir < 0 && -scrollPos >= overflowSize) {
                    scrollDir = 1;
                    scrollResumeAt = now + 1500;
                }
                else if (scrollDir > 0 && scrollPos >= 0) {
                    scrollDir = -1;
                    scrollResumeAt = now + 1500;
                }
            }
        }
    }

    m_sprite.setPivot(kNameW / 2.0f, m_h - 2.0f - m_nameSprite.width() / 2.0f - scrollPos);
    m_sprite.setClipRect(0, kFaderTop, kNameW, m_h - kFaderTop);
    m_nameSprite.pushRotated(&m_sprite, -90, 0x0000);
    m_sprite.clearClipRect();
}

} // namespace widgets
} // namespace ui
