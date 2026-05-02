#pragma once

#include <vector>
#include "Node.h"
#include "Nodes.h"
#include "channels/Channel.h"
#include "Config.h"
#include "osc/Osc.h"

namespace xr18 {
namespace nodes {

class RootNode : public Node {
public:
    RootNode(Osc& osc);

    Node* find(const char* path, int len);

private:
    Osc& osc;
    Nodes ch;
    Nodes rtn;
    Nodes bus;
    Nodes fxsend;
    Nodes dca;

    std::vector<channels::InputChannel> channels;
    channels::ReturnChannel aux;
    std::vector<channels::ReturnChannel> fxReturns;
    std::vector<channels::BusChannel> buses;
    std::vector<channels::FxSendChannel> fxSends;
    std::vector<channels::DCAChannel> dcas;
    channels::MainChannel lr;

    Config config;
};

} // namespace nodes
} // namespace xr18
