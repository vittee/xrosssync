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

    notify();
    return true;
}

void FloatParam::buildOsc(OSCMessage& msg) {
    msg.empty();
    msg.setAddress(m_path.c_str());
    msg.set(0, getNormalizedValue());
}
} // namespace params
} // namespace nodes
} // namespace xr18