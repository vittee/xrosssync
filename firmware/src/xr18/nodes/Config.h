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
        muteGroups.reserve(4);
        for (int i = 0; i < 4; i++) {
            muteGroups.emplace_back(String(i + 1), &mute, params::kOffOn);
        }
    }
private:
    Node mute;

    std::vector<params::EnumParam> muteGroups;
};

} // namespace nodes
} // namespace xr18
