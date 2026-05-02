#pragma once

#include "FloatParam.h"

namespace xr18 {
namespace nodes {
namespace params {

class FaderParam : public FloatParam {
public:
    FaderParam(String name, nodes::Node* parent, int max)
        : FloatParam(name, parent, max)
    {
        paramType = ParamType::Fader;
    }

    virtual String formatValue() const override;

    virtual float getValue() const override;
};

} // namespace params
} // namespace nodes
} // namespace xr18