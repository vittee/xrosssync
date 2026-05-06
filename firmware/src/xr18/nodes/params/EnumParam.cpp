#include "EnumParam.h"

namespace xr18 {
namespace nodes {
namespace params {

bool EnumParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isInt(index)) {
        return false;
    }

    m_value = msg.getInt(index);

    notify();
    return true;
}


} // namespace params
} // namespace nodes
} // namespace xr18
