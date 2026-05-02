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
    Nodes,
    Params
};

class RootNode;

class Node {
public:
    Node(String name, Node* parent = nullptr)
        : name(name), parent(parent)
    {
        if (parent) {
            parent->children.emplace_back(this);
            root = parent->root;
        }
    }

    const std::vector<Node*>& getChildren() const { return children; }
    const String& getName() const { return name; }
    const String& getPath() const { return path; }

    virtual bool applyOsc(OSCMessage& msg, int index = 0);
protected:
    void buildPath();

    String name;
    NodeType type;
    //
    Node* parent = nullptr;
    RootNode* root = nullptr;
    std::vector<Node*> children;
    //
    String path;
};

} // namespace nodes
} // namespace xr18