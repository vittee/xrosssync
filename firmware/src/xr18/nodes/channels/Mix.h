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
        m_on("on", this, params::kOffOn),
        m_fader("fader", this, 1023)
    {
    }

    inline params::EnumParam& on() { return m_on; }
    inline params::FaderParam& fader() { return m_fader; }

private:
    params::EnumParam m_on;
    params::FaderParam m_fader;
};

class MixWithLR : public Mix {
public:
    MixWithLR(Node* parent)
        :
        Mix(parent),
        m_lr("lr", this, params::kOffOn)
    {
    }

    inline params::EnumParam& lr() { return m_lr; }

private:
    params::EnumParam m_lr;
};

class MixWithPan : public Mix {
public:
    MixWithPan(Node* parent)
        :
        Mix(parent),
        m_pan("pan", this, 100, -100.0f, 100.0f, "%+f")
    {
    }

    inline params::LerpParam& pan() { return m_pan; }

private:
    params::LerpParam m_pan;
};

class MixWithLRAndPan : public MixWithLR {
public:
    MixWithLRAndPan(Node* parent)
        :
        MixWithLR(parent),
        m_pan("pan", this, 100, -100.0f, 100.0f, "%+f")
    {
    }

    inline params::LerpParam& pan() { return m_pan; }

private:
    params::LerpParam m_pan;
};

} // namespace channels
} // namespace nodes
} // namespace xr18