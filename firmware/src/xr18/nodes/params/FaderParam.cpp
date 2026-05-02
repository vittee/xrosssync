#include "FaderParam.h"

namespace xr18 {
namespace nodes {
namespace params {


String FaderParam::formatValue() const {
    auto v = getValue();
    if ((v > -0.01f) && (v < 0.01f)) {
        return "0.0";
    }

    if (v <= -90.0f) {
        return "-oo";
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%+5.1f", v);
    return String(buf);
}

float FaderParam::getValue() const {
    if (value >= (max + 1) / 2.0f) {
        return ((value * 40.0f) / max) - 30.0f;
    }

    if (value >= (max + 1) / 4.0f) {
        return ((value * 80.0f) / max) - 50.0f;
    }

    if (value >= (max + 1) / 16.0f) {
        return ((value * 160.0f) / max) - 70.0f;
    }

    if (value > 0) {
        return ((value * 480.0f) / max) - 90.0f;
    }

    return -144.0f;
}


} // namespace params
} // namespace nodes
} // namespace xr18
