#pragma once

#include "Param.h"

namespace xr18 {
namespace nodes {
namespace params {

class StringParam : public Param {
public:
    StringParam(String name, nodes::Node* parent)
        : Param(name, parent)
    {
        paramType = ParamType::String;
    }

    virtual bool applyOsc(OSCMessage& msg, int index) override;

    virtual String formatValue() const override { return value; }
private:
    String value;
};

} // namespace params
} // namespace nodes
} // namespace xr18
