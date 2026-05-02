#include "IntParam.h"

namespace xr18 {
namespace nodes {
namespace params {

void IntParam::setValue(int value) {
    if (value < m_min) {
        value = m_min;
    }

    if (value > m_max) {
        value = m_max;
    }

    m_value = value;

    // TODO: Send OSC
}

bool IntParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isInt(index)) {
        return false;
    }

    m_value = msg.getInt(index);

    ESP_LOGD("Int Param", "Applying OSC [%s] %d", name.c_str(), m_value);
    return true;
}


} // namespace params
} // namespace nodes
} // namespace xr18
