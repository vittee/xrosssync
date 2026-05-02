#pragma once

#include "xr18/nodes/Node.h"
#include "xr18/nodes/Nodes.h"
#include "xr18/nodes/params/StringParam.h"
#include "xr18/nodes/params/IntParam.h"
#include "xr18/nodes/params/EnumParam.h"
#include "xr18/nodes/params/LerpParam.h"
#include "xr18/nodes/params/FaderParam.h"

namespace xr18 {
namespace nodes {
namespace channels
{

class Mix : public Node {
public:
    Mix(Node* parent)
        :
        Node("mix", parent),
        on("on", this, params::kOffOn),
        fader("fader", this, 1023)
    {

    }
private:
    params::EnumParam on;
    params::FaderParam fader;
};

class MixWithLR : public Mix {
public:
    MixWithLR(Node* parent)
        :
        Mix(parent),
        lr("lr", this, params::kOffOn)
    {

    }
private:
    params::EnumParam lr;
};

class MixWithPan : public Mix {
public:
    MixWithPan(Node* parent)
        :
        Mix(parent),
        pan("pan", this, 100, -100.0f, 100.0f, "%+f")
    {

    }
private:
    params::LerpParam pan;
};

class MixWithLRAndPan : public MixWithLR {
public:
    MixWithLRAndPan(Node* parent)
        :
        MixWithLR(parent),
        pan("pan", this, 100, -100.0f, 100.0f, "%+f")
    {

    }
private:
    params::LerpParam pan;
};

} // namespace channels
} // namespace nodes
} // namespace xr18
