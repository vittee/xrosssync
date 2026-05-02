#include "LerpParam.h"

namespace xr18 {
namespace nodes {
namespace params {

String LerpParam::formatValue() const {
    char buf[32];
    snprintf(buf, sizeof(buf), format.c_str(), getFloatValue());
    return String(buf);
}

float LerpParam::getFloatValue() const {
    float range = targetMax - targetMin;
    float progress = (float)m_value / (float)m_max;
    return (progress * range) + targetMin;
}

void LerpParam::setFloatValue(float newValue) {
    float range = targetMax - targetMin;
    setValue(((newValue - targetMin) * m_max / range) + 0.5);
}

} // namespace params
} // namespace nodes
} // namespace xr18
