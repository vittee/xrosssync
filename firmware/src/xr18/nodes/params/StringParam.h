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

    virtual void buildOsc(OSCMessage& msg) override;

    virtual String formatValue() const override { return m_value; }
private:
    String m_value;
};

} // namespace params
} // namespace nodes
} // namespace xr18
