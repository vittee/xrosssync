#pragma once

#include "xr18/nodes/Node.h"
#include "xr18/nodes/Nodes.h"
#include "Config.h"
#include "Mix.h"

namespace xr18 {
namespace nodes {
namespace channels
{

class Channel : public Node {
public:
    Channel(String nodeName, Node* parent, NodeType type)
        :
        Node(nodeName, parent)
    {
        this->m_type = type;
    }

    inline const String& internalName() const { return m_internalName; }

protected:
    String m_internalName;
};

class InputChannel : public Channel {
public:
    InputChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::Input),
        m_mix(this),
        m_config(this)
    {
    }

    inline InputConfig& config() { return m_config; }
    inline MixWithLRAndPan& mix() { return m_mix; }

private:
    InputConfig m_config;
    MixWithLRAndPan m_mix;
};

class ReturnChannel : public Channel {
public:
    ReturnChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::Return),
        m_mix(this),
        m_config(this)
    {
    }

    inline ReturnConfig& config() { return m_config; }
    inline MixWithLRAndPan& mix() { return m_mix; }

private:
    ReturnConfig m_config;
    MixWithLRAndPan m_mix;
};

class BusChannel : public Channel {
public:
    BusChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::Bus),
        m_config(this),
        m_mix(this)
    {
    }

    inline Config& config() { return m_config; }
    inline const Config& config() const { return m_config; }
    inline MixWithLRAndPan& mix() { return m_mix; }
    inline const MixWithLRAndPan& mix() const { return m_mix; }

private:
    Config m_config;
    MixWithLRAndPan m_mix;
};

class FxSendChannel : public Channel {
public:
    FxSendChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::FxSend),
        m_config(this),
        m_mix(this)
    {
    }

    inline Config& config() { return m_config; }
    inline Mix& mix() { return m_mix; }

private:
    Config m_config;
    Mix m_mix;
};

class MainChannel : public Channel {
public:
    MainChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::LR),
        m_config(this),
        m_mix(this)
    {
    }

    inline Config& config() { return m_config; }
    inline MixWithPan& mix() { return m_mix; }

private:
    Config m_config;
    MixWithPan m_mix;
};

/**
 * A special channel type which acting like a Mix itself, but also has Config
 */
class DCAChannel : Node {
public:
    DCAChannel(String name, Node* parent)
        :
        Node(name, parent),
        m_on("on", this, params::kOffOn),
        m_fader("fader", this, 1023),
        m_config(this)
    {
        this->m_type = NodeType::DCA;
    }

    inline params::EnumParam& on() { return m_on; }
    inline params::FaderParam& fader() { return m_fader; }
    inline Config& config() { return m_config; }

private:
    params::EnumParam m_on;
    params::FaderParam m_fader;
    Config m_config;
};

} // namespace channels
} // namespace nodes
} // namespace xr18