#include <Arduino.h>
#include "osc/Osc.h"

namespace xr18 {

class XR18Client {
public:
    XR18Client();

    inline void setAddress(IPAddress ip, uint16_t port = 10024)  {
        osc.setAddress(ip, port);
    }

    inline bool send(OSCMessage& msg, TickType_t timeout) {
        return osc.send(msg, timeout);
    }

    void start();
private:
    void task();

    bool running = false;
    TaskHandle_t taskHandle;

    Osc osc;
};

}