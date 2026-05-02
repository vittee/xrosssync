#pragma once

#include "xr18/nodes/Node.h"

namespace xr18 {
namespace nodes {
namespace params {

static const std::vector<const char*> kOffOn = { "Off", "On" };

enum class ParamType {
    String,
    Int,
    Enum,
    Lerp,
    Logarithm,
    Fader,
    Level
};

class Param : public Node {
public:
    Param(String name, Node* parent)
        : Node(name, parent)
    {
        type = NodeType::Params;
    }

    ParamType getParamType() const { return paramType; }

    virtual String formatValue() const = 0;

protected:
    ParamType paramType;
};

} // namespace params
} // namespace nodes
} // namespace xr18
