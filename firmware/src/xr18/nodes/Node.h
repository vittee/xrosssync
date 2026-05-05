#pragma once

#include <vector>
#include <WString.h>
#include <OSCMessage.h>

namespace xr18 {
namespace nodes {

enum class NodeType {
    Invalid,
    Root,
    Input,
    Return,
    Bus,
    FxSend,
    LR,
    DCA,
    Nodes,
    Params
};

class RootNode;

class Node {
public:
    Node(String name, Node* parent = nullptr)
        : m_name(name), parent(parent)
    {
        if (parent) {
            parent->m_children.emplace_back(this);
        }
    }

    inline const std::vector<Node*>& children() const { return m_children; }
    inline const String& name() const { return m_name; }
    inline const NodeType type() const { return m_type; }
    inline const String& path() const { return m_path; }

    RootNode* getRoot();

    virtual bool applyOsc(OSCMessage& msg, int index = 0);

    void buildPath();
protected:


    String m_name;
    NodeType m_type;
    //
    Node* parent = nullptr;
    std::vector<Node*> m_children;
    //
    String m_path;
};

} // namespace nodes
} // namespace xr18