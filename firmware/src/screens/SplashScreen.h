#pragma once

#include "ui/Screen.h"
#include "ui/Container.h"
#include "ui/widgets/Label.h"

class SplashScreen : public ui::Screen {
public:
    SplashScreen(int16_t w, int16_t h);

private:
    ui::widgets::Label m_xross;
    ui::widgets::Label m_sync;
    ui::Container m_row;
};
