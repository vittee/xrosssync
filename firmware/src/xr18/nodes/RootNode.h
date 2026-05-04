#pragma once

#include <vector>
#include "Node.h"
#include "Nodes.h"
#include "channels/Channel.h"
#include "Config.h"
#include "State.h"
#include "Action.h"
#include "osc/Osc.h"

namespace xr18 {
namespace nodes {

class RootNode : public Node {
public:
    RootNode(Osc& osc);

    Node* find(const char* path, int len);

    inline std::vector<channels::InputChannel>& channels() { return m_channels; }
    inline channels::ReturnChannel& aux() { return m_aux; }
    inline std::vector<channels::ReturnChannel>& fxReturns() { return m_fxReturns; }
    inline std::vector<channels::BusChannel>& buses() { return m_buses; }
    inline std::vector<channels::FxSendChannel>& fxSends() { return m_fxSends; }
    inline std::vector<channels::DCAChannel>& dcas() { return m_dcas; }
    inline channels::MainChannel& lr() { return m_lr; }
    inline Config& config() { return m_config; }
    inline State& state() { return m_state; }
    inline Action& action() { return m_action; }

private:
    friend class params::Param;

    Osc& osc;
    Nodes ch;
    Nodes rtn;
    Nodes bus;
    Nodes fxsend;
    Nodes dca;

    std::vector<channels::InputChannel> m_channels;
    channels::ReturnChannel m_aux;
    std::vector<channels::ReturnChannel> m_fxReturns;
    std::vector<channels::BusChannel> m_buses;
    std::vector<channels::FxSendChannel> m_fxSends;
    std::vector<channels::DCAChannel> m_dcas;
    channels::MainChannel m_lr;

    Config m_config;
    State m_state;
    Action m_action;
};

} // namespace nodes
} // namespace xr18
