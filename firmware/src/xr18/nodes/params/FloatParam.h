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

    virtual String formatValue() const override = 0;

    virtual float getValue() const = 0;
};

} // namespace params
} // namespace nodes
} // namespace xr18
