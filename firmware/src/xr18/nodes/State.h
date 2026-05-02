#pragma once

#include "Node.h"
#include "Nodes.h"
#include "params/EnumParam.h"

namespace xr18 {
namespace nodes {

class State : public Node {
public:
    State(Node* parent)
        :
        Node("-stat", parent),
        m_solo("solo", this, params::kOffOn),
        solosw("solosw", this),
        m_solosw_aux("17", &solosw, params::kOffOn),
        m_solosw_main("50", &solosw, params::kOffOn)
    {
        char buf[3]{};

        m_solosw_inputs.reserve(16);
        for (int i = 0; i < 16; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 1);
            m_solosw_inputs.emplace_back(String(buf), &solosw, params::kOffOn);
        }

        m_solosw_fxreturns.reserve(4);
        for (int i = 0; i < 4; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 18);
            m_solosw_fxreturns.emplace_back(String(buf), &solosw, params::kOffOn);
        }

        m_solosw_buses.reserve(6);
        for (int i = 0; i < 6; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 40);
            m_solosw_buses.emplace_back(String(buf), &solosw, params::kOffOn);
        }

        m_solosw_dcas.reserve(4);
        for (int i = 0; i < 4; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 51);
            m_solosw_dcas.emplace_back(String(buf), &solosw, params::kOffOn);
        }
    }

    inline const params::EnumParam& solo() const { return m_solo; }
    inline const std::vector<params::EnumParam>& soloswInputs() const { return m_solosw_inputs; }
    inline const params::EnumParam& solosw_aux() const { return m_solosw_aux; }
    inline const std::vector<params::EnumParam>& soloswFxReturns() const { return m_solosw_fxreturns; }
    inline const std::vector<params::EnumParam>& soloswBuses() const { return m_solosw_buses; }
    inline const params::EnumParam& solosw_main() const { return m_solosw_main; }
    inline const std::vector<params::EnumParam>& soloswDcas() const { return m_solosw_dcas; }

private:
    params::EnumParam m_solo;
    Nodes solosw;

    std::vector<params::EnumParam> m_solosw_inputs;
    params::EnumParam m_solosw_aux;
    std::vector<params::EnumParam> m_solosw_fxreturns;
    std::vector<params::EnumParam> m_solosw_buses;
    params::EnumParam m_solosw_main;
    std::vector<params::EnumParam> m_solosw_dcas;
};

} // namespace nodes
} // namespace xr18
