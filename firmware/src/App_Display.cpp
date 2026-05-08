#include "App.h"

App::Display::Display() {
    {
        auto cfg = m_bus.config();
        cfg.pin_wr = 8;
        cfg.pin_rd = 9;
        cfg.pin_rs = 7;
        cfg.pin_d0 = 39;
        cfg.pin_d1 = 40;
        cfg.pin_d2 = 41;
        cfg.pin_d3 = 42;
        cfg.pin_d4 = 45;
        cfg.pin_d5 = 46;
        cfg.pin_d6 = 47;
        cfg.pin_d7 = 48;
        m_bus.config(cfg);
    }

    {
        auto cfg = m_panel.config();
        cfg.pin_cs = 6;
        cfg.pin_rst = 5;
        cfg.pin_busy = -1;
        cfg.offset_rotation = 1;
        cfg.offset_x = 35;
        cfg.readable = false;
        cfg.invert = true;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = false;
        cfg.panel_width = 170;
        cfg.panel_height = 320;
        m_panel.config(cfg);
    }

    {
        auto cfg = m_backlight.config();
        cfg.pin_bl = 38;
        cfg.invert = false;
        cfg.freq = 22000;
        cfg.pwm_channel = 7;
        m_backlight.config(cfg);
    }

    {
        auto cfg = m_touch.config();
        cfg.x_min = 0;
        cfg.x_max = 170;
        cfg.y_min = 0;
        cfg.y_max = 320;
        cfg.pin_rst = 21; // Set this for touch and backlight to work altogether
        cfg.pin_int = PIN_TOUCH_INT;
        cfg.pin_scl = 17;
        cfg.pin_sda = 18;
        m_touch.config(cfg);
    }

    m_panel.setBus(&m_bus);
    m_panel.setLight(&m_backlight);
    m_panel.setTouch(&m_touch);
    this->setPanel(&m_panel);
}

void App::Display::selfTest() {
    int colors[3]{TFT_RED, TFT_GREEN, TFT_BLUE};
    for (int i = 0; i < 3; i++) {
        fillScreen(colors[i]);
        delay(100);
    }

    drawTestPattern();
    delay(1000);
}

void App::Display::drawGrad(int y, int h, uint16_t r, uint16_t g, uint16_t b) {
    int x1 = 0;
    int x2 = 0;
    for (int i = 0; i <= 256; i++) {
        x2 = i * width() / 256;
        if (x1 < x2) {
            fillRect(x1, y, x2 - x1, h, color565(r * i >> 8, g * i >> 8, b * i >> 8));
            x1 = x2;
        }
    }
}

void App::Display::drawTestPattern() {
    auto w = width();
    auto h = height();

    drawGrad(0, h, 255, 0, 0);
    delay(75);
    drawGrad(0, h, 0, 255, 0);
    delay(75);
    drawGrad(0, h, 0, 0, 255);
    delay(75);
    drawGrad(0, h, 255, 255, 0);
    delay(75);
    drawGrad(0, h, 255, 0, 255);
    delay(75);
    drawGrad(0, h, 0, 255, 255);
    delay(75);
    drawGrad(0, h, 255, 255, 255);
    delay(75);

    auto bw = w / 7;
    fillScreen(TFT_BLACK);
    fillRect(bw * 0 + 2, 0, bw, h, TFT_WHITE);
    fillRect(bw * 1 + 2, 0, bw, h, TFT_YELLOW);
    fillRect(bw * 2 + 2, 0, bw, h, TFT_CYAN);
    fillRect(bw * 3 + 2, 0, bw, h, TFT_GREEN);
    fillRect(bw * 4 + 2, 0, bw, h, TFT_MAGENTA);
    fillRect(bw * 5 + 2, 0, bw, h, TFT_RED);
    fillRect(bw * 6 + 2, 0, bw, h, TFT_BLUE);
}
