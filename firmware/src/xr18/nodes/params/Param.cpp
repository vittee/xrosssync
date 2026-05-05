#include "Param.h"
#include "xr18/nodes/RootNode.h"

namespace xr18 {
namespace nodes {
namespace params {

void Param::onChange(std::function<void()> cb) {
    m_callbacks.push_back(std::move(cb));
}

void Param::notify() {
    for (auto& cb : m_callbacks) {
        cb();
    }
}

void Param::valueChanged() {
    ESP_LOGD("Param", "Value changed");

    auto root = getRoot();
    if (root != nullptr) {
        OSCMessage msg;
        buildOsc(msg);

        ESP_LOGD("Param", "Value changed, msg=%s", msg.getAddress());

        root->osc.send(msg, 0, 2);
    }

    notify();
}

} // namespace params
} // namespace nodes
} // namespace xr18