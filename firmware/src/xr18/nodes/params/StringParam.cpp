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

void StringParam::buildOsc(OSCMessage& msg) {
    msg.empty();
    msg.setAddress(path.c_str());
    msg.set(0, value.c_str());
}

} // namespace params
} // namespace nodes
} // namespace xr18
