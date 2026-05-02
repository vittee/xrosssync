#include <Arduino.h>
#include "RootNode.h"

namespace xr18 {
namespace nodes {

RootNode::RootNode(Osc& osc)
    :
    Node(""),
    osc(osc),
    ch("ch", this),
    rtn("rtn", this),
    aux("aux", &rtn),
    bus("bus", this),
    fxsend("fxsend", this),
    lr("lr", this),
    dca("dca", this),
    config(this)
{
    type = NodeType::Root;
    root = this;

    channels.reserve(16);
    for (int i = 0; i < 16; i++) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02d", i + 1);
        channels.emplace_back(String(buf), &ch);
    }

    fxReturns.reserve(4);
    for (int i = 0; i < 4; i++) {
        fxReturns.emplace_back(String(i + 1), &rtn);
    }

    buses.reserve(6);
    for (int i = 0; i < 4; i++) {
        buses.emplace_back(String(i + 1), &bus);
    }

    fxSends.reserve(4);
    for (int i = 0; i < 4; i++) {
        fxSends.emplace_back(String(i + 1), &fxsend);
    }

    dcas.reserve(4);
    for (int i = 0; i < 4; i++) {
        dcas.emplace_back(String(i + 1), &dca);
    }

    buildPath();
}

int nextIndex(String& s, int fromIndex) {
    int result = s.indexOf('/', fromIndex);

    if (result == -1) {
        result = s.indexOf(' ', fromIndex);
    }

    return result;
}

Node* RootNode::find(const char* path, int len) {
    if (len <= 0) {
        return this;
    }

    String s_path = path;

    int l = 0;
    Node* current = this;

    while (l != -1) {
        int r = nextIndex(s_path, l + 1);
        String segment = (r == -1) ? s_path.substring(l + 1) : s_path.substring(l + 1, r);

        // ESP_LOGD("find node", "Segment: %s", segment.c_str());

        if (segment.isEmpty()) {
            break;
        }

        Node* found = nullptr;
        for (auto node : current->getChildren()) {
            // ESP_LOGD("find node", "node name: %s", node->getName().c_str());

            if (node->getName() == segment) {
                found = node;
                break;
            }
        }

        if (!found) {
            return nullptr;
        }

        current = found;
        l = r;
    }

    return current;
}

} // namespace nodes
} // namespace xr18
