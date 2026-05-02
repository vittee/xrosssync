#pragma once

#include "Node.h"
#include "params/EnumParam.h"

namespace xr18 {
namespace nodes {

class Action : public Node {
public:
    Action(Node* parent)
        :
        Node("-action", parent),
        m_clearsolo("clearsolo", this, { "-", "GO" })
    {
    }

    inline const params::EnumParam& clearsolo() const { return m_clearsolo; }

private:
    params::EnumParam m_clearsolo;
};

} // namespace nodes
} // namespace xr18