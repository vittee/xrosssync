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
    Channel(String name, Node* parent, NodeType type)
        :
        Node(name, parent)
    {
        this->type = type;
    }
};

class InputChannel : public Channel {
public:
    InputChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::Input),
        mix(this),
        config(this)
    {

    }

private:
    InputConfig config;
    MixWithLRAndPan mix;
};

class ReturnChannel : public Channel {
public:
    ReturnChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::Return),
        mix(this),
        config(this)
    {

    }

private:
    ReturnConfig config;
    MixWithLRAndPan mix;
};

class BusChannel : public Channel {
public:
    BusChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::Bus),
        config(this),
        mix(this)
    {

    }

private:
    Config config;
    MixWithLRAndPan mix;
};

class FxSendChannel : public Channel {
public:
    FxSendChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::FxSend),
        config(this),
        mix(this)
    {

    }

private:
    Config config;
    Mix mix;
};

class MainChannel : public Channel {
public:
    MainChannel(String name, Node* parent)
        :
        Channel(name, parent, NodeType::LR),
        config(this),
        mix(this)
    {

    }

private:
    Config config;
    MixWithPan mix;
};

/**
 * A special channel type which acting like a Mix itself, but also has Config
 */
class DCAChannel : Node {
public:
    DCAChannel(String name, Node* parent)
        :
        Node(name, parent),
        on("on", this, params::kOffOn),
        fader("fader", this, 1023),
        config(this)
    {

    }
private:
    params::EnumParam on;
    params::FaderParam fader;
    Config config;
};

} // namespace channels
} // namespace nodes
} // namespace xr18
