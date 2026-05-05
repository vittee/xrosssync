#pragma once

#include <functional>
#include <vector>
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
        m_type = NodeType::Params;
    }

    ParamType getParamType() const { return paramType; }

    virtual bool applyOsc(OSCMessage& msg, int index) = 0;

    virtual void buildOsc(OSCMessage& msg) = 0;

    virtual String formatValue() const = 0;

    void onChange(std::function<void()> cb);

protected:
    void notify();
    void valueChanged();

    ParamType paramType;

private:
    friend class nodes::Node;

    std::vector<std::function<void()>> m_callbacks;
};

} // namespace params
} // namespace nodes
} // namespace xr18
