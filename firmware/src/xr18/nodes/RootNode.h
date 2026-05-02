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

    inline const std::vector<channels::InputChannel>& channels() const { return m_channels; }
    inline const channels::ReturnChannel& aux() const { return m_aux; }
    inline const std::vector<channels::ReturnChannel>& fxReturns() const { return m_fxReturns; }
    inline const std::vector<channels::BusChannel>& buses() const { return m_buses; }
    inline const std::vector<channels::FxSendChannel>& fxSends() const { return m_fxSends; }
    inline const std::vector<channels::DCAChannel>& dcas() const { return m_dcas; }
    inline const channels::MainChannel& lr() const { return m_lr; }
    inline const Config& config() const { return m_config; }
    inline const State& state() const { return m_state; }
    inline const Action& action() const { return m_action; }

private:
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
