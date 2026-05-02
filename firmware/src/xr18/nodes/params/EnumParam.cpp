#include "EnumParam.h"

namespace xr18 {
namespace nodes {
namespace params {

bool EnumParam::applyOsc(OSCMessage& msg, int index) {
    if (!msg.isInt(index)) {
        return false;
    }

    m_value = msg.getInt(index);

    ESP_LOGD("Enum Param", "Applying OSC [%s] %d => %s", name.c_str(), m_value, formatValue().c_str());
    return true;
}


} // namespace params
} // namespace nodes
} // namespace xr18
