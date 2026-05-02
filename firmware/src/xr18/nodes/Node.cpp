#include "Node.h"

#include "params/IntParam.h"
#include "params/StringParam.h"

namespace xr18 {
namespace nodes {

void Node::buildPath() {
    String newPath("");

    if (parent != nullptr) {
        newPath = parent->path;
    }

    if (!name.isEmpty()) {
        newPath += "/" + name;
    }

    path = newPath;

    for (auto* child : children) {
        child->buildPath();
    }
}

bool Node::applyOsc(OSCMessage& msg, int) {
    ESP_LOGD("Node", "Applying OSC children=%d, argc=%d", children.size(), msg.size());

    if (children.size() == 0) {
        return false;
    }

    int argc = msg.size();

    for (int i = 0; i < children.size(); i++) {
        if (i >= argc) {
            return false;
        }

        Node* node = children.at(i);

        ESP_LOGD("Node", "children(%d), path=%s, type=%d", i, node->path.c_str(), node->type);

        if (node->type != NodeType::Params) {
            return false;
        }

        auto param = static_cast<params::Param*>(node);

        auto paramType = param->getParamType();

        ESP_LOGD("Node", "param=%d, argType=%c", paramType, msg.getType(i));

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