#include "FloatParam.h"

namespace xr18 {
namespace nodes {
namespace params {

bool FloatParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isFloat(index)) {
        return false;
    }

    auto floatValue = msg.getFloat(index);

    int v = (int)((m_max + 0.5) * floatValue);
    if (v > m_max) v = m_max;

    m_value = v;

    ESP_LOGD("FloatParam", "Applying OSC [%s] %d => ", name.c_str(), m_value, formatValue().c_str());
    return true;
}

} // namespace params
} // namespace nodes
} // namespace xr18