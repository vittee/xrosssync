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

    valueChanged();
}

bool IntParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isInt(index)) {
        return false;
    }

    m_value = msg.getInt(index);

    ESP_LOGD("Int Param", "Applying OSC [%s] %d", m_path.c_str(), m_value);
    return true;
}

void IntParam::buildOsc(OSCMessage& msg) {
    msg.empty();
    msg.setAddress(m_path.c_str());
    msg.set(0, m_value);
}


} // namespace params
} // namespace nodes
} // namespace xr18
