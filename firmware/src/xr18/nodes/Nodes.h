#pragma once

#include "Node.h"

namespace xr18 {
namespace nodes {

class Nodes : public Node {
public:
    Nodes(String name, Node* parent)
        : Node(name, parent)
    {
        type = NodeType::Nodes;
    }

private:

};

} // namespace nodes
} // namespace xr18
