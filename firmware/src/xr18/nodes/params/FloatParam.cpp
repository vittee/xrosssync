#include "FloatParam.h"

namespace xr18 {
namespace nodes {
namespace params {

bool FloatParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isFloat(index)) {
        return false;
    }

    auto floatValue = msg.getFloat(index);

    int v = (int)((max + 0.5) * floatValue);
    if (v > max) v = max;

    value = v;

    ESP_LOGD("FloatParam", "Applying OSC [%s] %d => ", name.c_str(), value, formatValue().c_str());
    return true;
}

} // namespace params
} // namespace nodes
} // namespace xr18