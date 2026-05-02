#include "StringParam.h"

namespace xr18 {
namespace nodes {
namespace params {

bool StringParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isString(index)) {
        return false;
    }

    char buf[256];
    int length = msg.getString(index, buf, sizeof(buf));

    value = String(buf, length);

    ESP_LOGD("String Param", "Applying OSC [%s] %s", name.c_str(), value.c_str());
    return true;
}

} // namespace params
} // namespace nodes
} // namespace xr18
