#include "XR18Client.h"

namespace xr18 {

XR18Client::XR18Client() {

}

void XR18Client::start() {
    if (running) return;

    running = true;

    xTaskCreatePinnedToCore([](void* inst) {
        static_cast<XR18Client*>(inst)->task();
    }, "xr18_task", 8192, this, 1, &taskHandle, 1);

    osc.start();
}

void XR18Client::task() {
    while (running) {
        OSCMessage msg;
        osc.receive(msg, portMAX_DELAY);

        ESP_LOGD("XR18", "Got %s", msg.getAddress());

        vTaskDelay(500);
    }
}

}