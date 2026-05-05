#pragma once

#include "Node.h"

namespace xr18 {
namespace nodes {

class Nodes : public Node {
public:
    Nodes(String name, Node* parent)
        : Node(name, parent)
    {
        m_type = NodeType::Nodes;
    }

private:

};

} // namespace nodes
} // namespace xr18
