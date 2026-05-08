#pragma once

#include "ui/Screen.h"
#include "ui/Container.h"
#include "ui/widgets/Label.h"

class MainScreen : public ui::Screen {
public:
    static constexpr int16_t kStatusBarHeight = 18;

    MainScreen(int16_t w, int16_t h);

private:
    ui::widgets::Label m_statusBar;
    ui::Container m_panel;
};
