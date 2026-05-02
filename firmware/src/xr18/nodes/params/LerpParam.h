#pragma once

#include "FloatParam.h"

namespace xr18 {
namespace nodes {
namespace params {

class LerpParam : public FloatParam {
public:
    LerpParam(String name, nodes::Node* parent, int max, float targetMin, float targetMax, String format)
        :
        FloatParam(name, parent, max),
        targetMin(targetMin),
        targetMax(targetMax),
        format(format)
    {

    }

    virtual String formatValue() const override;

    virtual float getValue() const override;

private:
    float targetMin;
    float targetMax;

    String format;
};

} // namespace params
} // namespace nodes
} // namespace xr18