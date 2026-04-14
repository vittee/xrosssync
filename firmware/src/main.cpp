#include "App.h"

static App app;

void setup()
{
    Serial.begin(115200);
    if (!app.init()) {
        return;
    }

    // Remove main loop task, we will create tasks separately
    vTaskDelete(nullptr);
}

void loop()
{
    // do nothing
}
