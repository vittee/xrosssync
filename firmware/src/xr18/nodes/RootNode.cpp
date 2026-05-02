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
    m_aux("aux", &rtn),
    bus("bus", this),
    fxsend("fxsend", this),
    m_lr("lr", this),
    dca("dca", this),
    m_config(this),
    m_state(this),
    m_action(this)
{
    type = NodeType::Root;
    root = this;

    m_channels.reserve(16);
    for (int i = 0; i < 16; i++) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02d", i + 1);
        m_channels.emplace_back(String(buf), &ch);
    }

    m_fxReturns.reserve(4);
    for (int i = 0; i < 4; i++) {
        m_fxReturns.emplace_back(String(i + 1), &rtn);
    }

    m_buses.reserve(6);
    for (int i = 0; i < 4; i++) {
        m_buses.emplace_back(String(i + 1), &bus);
    }

    m_fxSends.reserve(4);
    for (int i = 0; i < 4; i++) {
        m_fxSends.emplace_back(String(i + 1), &fxsend);
    }

    m_dcas.reserve(4);
    for (int i = 0; i < 4; i++) {
        m_dcas.emplace_back(String(i + 1), &dca);
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
