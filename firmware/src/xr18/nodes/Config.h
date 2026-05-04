#pragma once

#include "Node.h"
#include "params/EnumParam.h"

namespace xr18 {
namespace nodes {

class Config : public Node {
public:
    Config(Node* parent)
        :
        Node("config", parent),
        mute("mute", this)
    {
        m_muteGroups.reserve(4);
        for (int i = 0; i < 4; i++) {
            m_muteGroups.emplace_back(String(i + 1), &mute, params::kOffOn);
        }
    }

    inline std::vector<params::EnumParam>& muteGroups() { return m_muteGroups; }

private:
    Node mute;

    std::vector<params::EnumParam> m_muteGroups;
};

} // namespace nodes
} // namespace xr18
