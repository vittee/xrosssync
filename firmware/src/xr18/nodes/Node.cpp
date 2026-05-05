#include "Node.h"
#include "RootNode.h"

#include "params/IntParam.h"
#include "params/StringParam.h"

namespace xr18 {
namespace nodes {

RootNode* Node::getRoot() {
    Node* current = this;

    while (current->parent != nullptr) {
        current = current->parent;
    }

    if (current->m_type == NodeType::Root) {
        return static_cast<RootNode*>(current);
    }

    return nullptr;
}

void Node::buildPath() {
    String newPath("");

    if (parent != nullptr) {
        newPath = parent->m_path;
    }

    if (!m_name.isEmpty()) {
        newPath += "/" + m_name;
    }

    m_path = newPath;

    for (auto* child : m_children) {
        child->buildPath();
    }
}

bool Node::applyOsc(OSCMessage& msg, int) {
    if (m_children.size() == 0) {
        return false;
    }

    int argc = msg.size();

    for (int i = 0; i < m_children.size(); i++) {
        if (i >= argc) {
            return false;
        }

        Node* node = m_children.at(i);

        if (node->m_type != NodeType::Params) {
            return false;
        }

        auto param = static_cast<params::Param*>(node);

        auto paramType = param->getParamType();

        switch (msg.getType(i)) {
            case 'i':
                if ((paramType != params::ParamType::Int) && (paramType != params::ParamType::Enum)) {
                    return false;
                }
                break;

            case 'f':
                if ((paramType < params::ParamType::Lerp) || (paramType > params::ParamType::Level)) {
                    return false;
                }
                break;

            case 's':
                if (paramType != params::ParamType::String) {
                    return false;
                }
                break;

            default:
                return false;

        }

        param->applyOsc(msg, i);
    }

    return true;
}

} // namespace nodes
} // namespace xr18