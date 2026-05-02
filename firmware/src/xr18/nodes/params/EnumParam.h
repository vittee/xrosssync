#pragma once

#include "IntParam.h"

namespace xr18 {
namespace nodes {
namespace params {

class EnumParam : public IntParam {
public:
    EnumParam(String name, nodes::Node* parent, std::vector<const char*> names)
        : IntParam(name, parent, 0, names.size()), names(names)
    {
        paramType = ParamType::Enum;
    }

    virtual bool applyOsc(OSCMessage& msg, int index) override;

    virtual String formatValue() const override { return String(names[value]); }
private:
    std::vector<const char*> names;
};

} // namespace params
} // namespace nodes
} // namespace xr18
