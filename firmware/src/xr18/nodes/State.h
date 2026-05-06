#pragma once

#include <optional>
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
        m_solosw("solosw", this)
    {
        char buf[3]{};

        m_solosw_inputs.reserve(16);
        for (int i = 0; i < 16; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 1);
            m_solosw_inputs.emplace_back(String(buf), &m_solosw, params::kOffOn);
        }

        m_solosw_aux.emplace("17", &m_solosw, params::kOffOn);

        m_solosw_fxreturns.reserve(4);
        for (int i = 0; i < 4; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 18);
            m_solosw_fxreturns.emplace_back(String(buf), &m_solosw, params::kOffOn);
        }

        m_solosw_usbreturns.reserve(18);
        for (int i = 0; i < 18; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 22);
            m_solosw_usbreturns.emplace_back(String(buf), &m_solosw, params::kOffOn);
        }

        m_solosw_buses.reserve(6);
        for (int i = 0; i < 6; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 40);
            m_solosw_buses.emplace_back(String(buf), &m_solosw, params::kOffOn);
        }

        m_solosw_fxsends.reserve(4);
        for (int i = 0; i < 4; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 46);
            m_solosw_fxsends.emplace_back(String(buf), &m_solosw, params::kOffOn);
        }

        m_solosw_main.emplace("50", &m_solosw, params::kOffOn);

        m_solosw_dcas.reserve(4);
        for (int i = 0; i < 4; i++) {
            snprintf(buf, sizeof(buf), "%02d", i + 51);
            m_solosw_dcas.emplace_back(String(buf), &m_solosw, params::kOffOn);
        }
    }

    inline params::EnumParam& solo() { return m_solo; }

    inline params::EnumParam& soloswAt(uint8_t index) {
        return *static_cast<params::EnumParam*>(m_solosw.children()[index]);
    }

    inline std::vector<params::EnumParam>& soloswInputs() { return m_solosw_inputs; }
    inline params::EnumParam& solosw_aux() { return *m_solosw_aux; }
    inline std::vector<params::EnumParam>& soloswFxReturns() { return m_solosw_fxreturns; }
    inline std::vector<params::EnumParam>& soloswUsbReturns() { return m_solosw_usbreturns; }
    inline std::vector<params::EnumParam>& soloswBuses() { return m_solosw_buses; }
    inline std::vector<params::EnumParam>& soloswFxSends() { return m_solosw_fxsends; }
    inline params::EnumParam& solosw_main() { return *m_solosw_main; }
    inline std::vector<params::EnumParam>& soloswDcas() { return m_solosw_dcas; }
    inline Nodes& solosw() { return m_solosw; }

private:
    params::EnumParam m_solo;
    Nodes m_solosw;

    std::vector<params::EnumParam> m_solosw_inputs;
    std::optional<params::EnumParam> m_solosw_aux;
    std::vector<params::EnumParam> m_solosw_fxreturns;
    std::vector<params::EnumParam> m_solosw_usbreturns;
    std::vector<params::EnumParam> m_solosw_buses;
    std::vector<params::EnumParam> m_solosw_fxsends;
    std::optional<params::EnumParam> m_solosw_main;
    std::vector<params::EnumParam> m_solosw_dcas;
};

} // namespace nodes
} // namespace xr18
