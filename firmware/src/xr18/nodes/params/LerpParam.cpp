#include "LerpParam.h"

namespace xr18 {
namespace nodes {
namespace params {

String LerpParam::formatValue() const {
    char buf[32];
    snprintf(buf, sizeof(buf), format.c_str(), getValue());
    return String(buf);
}

float LerpParam::getValue() const {
    float range = targetMax - targetMin;
    float progress = (float)value / (float)max;
    return (progress * range) + targetMin;
}

} // namespace params
} // namespace nodes
} // namespace xr18
