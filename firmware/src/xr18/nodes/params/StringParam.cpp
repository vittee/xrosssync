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

    m_value = String(buf, length);

    notify();
    return true;
}

void StringParam::buildOsc(OSCMessage& msg) {
    msg.empty();
    msg.setAddress(m_path.c_str());
    msg.set(0, m_value.c_str());
}

} // namespace params
} // namespace nodes
} // namespace xr18
