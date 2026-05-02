#include "IntParam.h"

namespace xr18 {
namespace nodes {
namespace params {

bool IntParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isInt(index)) {
        return false;
    }

    value = msg.getInt(index);

    ESP_LOGD("Int Param", "Applying OSC [%s] %d", name.c_str(), value);
    return true;
}


} // namespace params
} // namespace nodes
} // namespace xr18
