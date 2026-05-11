#include "FaderParam.h"

namespace xr18 {
namespace nodes {
namespace params {


String FaderParam::formatValue() const {
    auto v = getFloatValue();
    if ((v > -0.01f) && (v < 0.01f)) {
        return "0.0";
    }

    if (v <= -90.0f) {
        return "-oo";
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%.1f", v);
    return String(buf);
}

float FaderParam::getFloatValue() const {
    if (m_value >= (m_max + 1) / 2.0f) {
        return ((m_value * 40.0f) / m_max) - 30.0f;
    }

    if (m_value >= (m_max + 1) / 4.0f) {
        return ((m_value * 80.0f) / m_max) - 50.0f;
    }

    if (m_value >= (m_max + 1) / 16.0f) {
        return ((m_value * 160.0f) / m_max) - 70.0f;
    }

    if (m_value > 0) {
        return ((m_value * 480.0f) / m_max) - 90.0f;
    }

    return -144.0f;
}

void FaderParam::setFloatValue(float newValue) {
    if (newValue >= -10.0f) {
        setValue(((newValue + 30.0f) * m_max / 40.0f) + 0.5f);
        return;
    }

    if (newValue >= -30.0f) {
        setValue(((newValue + 50.0f) * m_max / 80.0f) + 0.5f);
        return;
    }

    if (newValue >= -60.0f) {
        setValue(((newValue + 70.0f) * m_max / 160.0f) + 0.5f);
        return;
    }

    if (newValue >= -90.0f) {
        setValue(((newValue + 90.0f) * m_max / 480.0f) + 0.5f);
        return;
    }

    setValue(0);
}


} // namespace params
} // namespace nodes
} // namespace xr18
