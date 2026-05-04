#pragma once

#include "Param.h"

namespace xr18 {
namespace nodes {
namespace params {

class IntParam : public Param {
public:
    IntParam(String name, nodes::Node* parent, int min, int max)
        : Param(name, parent), m_value(min), m_min(min), m_max(max)
    {
        paramType = ParamType::Int;
    }

    inline int value() const { return m_value; }

    void setValue(int value);

    virtual bool applyOsc(OSCMessage& msg, int index) override;

    virtual void buildOsc(OSCMessage& msg) override;

    virtual String formatValue() const override { return String(m_value); }
protected:
    int m_value;
    int m_min;
    int m_max;
};

} // namespace params
} // namespace nodes
} // namespace xr18
