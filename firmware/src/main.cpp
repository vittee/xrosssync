#include "App.h"

static App app;

void setup()
{
#ifdef XROSSSYNC_DEBUG
    esp_log_level_set("*", ESP_LOG_DEBUG);
    Serial.begin(115200);
#else
    esp_log_level_set("*", ESP_LOG_NONE);
#endif

    auto ok = app.init();

    if (!ok) {
        ESP_LOGE("MAIN", "Error init");
    }

    // Remove main loop task, we will create tasks separately
    vTaskDelete(nullptr);
}

void loop()
{
    // do nothing
}
