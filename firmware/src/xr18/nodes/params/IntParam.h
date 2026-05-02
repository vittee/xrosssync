#pragma once

#include "Param.h"

namespace xr18 {
namespace nodes {
namespace params {

class IntParam : public Param {
public:
    IntParam(String name, nodes::Node* parent, int min, int max)
        : Param(name, parent), value(min), min(min), max(max)
    {
        paramType = ParamType::Int;
    }

    virtual bool applyOsc(OSCMessage& msg, int index) override;

    virtual String formatValue() const override { return String(value); }
protected:
    int value;
    int min;
    int max;
};

} // namespace params
} // namespace nodes
} // namespace xr18
