#pragma once

#include "IntParam.h"

namespace xr18 {
namespace nodes {
namespace params {

class FloatParam : public IntParam {
public:
    FloatParam(String name, nodes::Node* parent, int max)
        : IntParam(name, parent, 0, max)
    {

    }

    virtual bool applyOsc(OSCMessage& msg, int index) override;

    virtual void buildOsc(OSCMessage& msg) override;

    virtual String formatValue() const override = 0;

    virtual float getFloatValue() const = 0;

    virtual void setFloatValue(float newValue) = 0;

    float getNormalizedValue() const { return (float)m_value / (float)m_max; }
};

} // namespace params
} // namespace nodes
} // namespace xr18
