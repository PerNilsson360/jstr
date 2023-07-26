#include <iostream>
#include "LeafNode.hh"

namespace Jstr {
namespace Xpath {

LeafNode::LeafNode(const Node* parent, const std::string& name, const nlohmann::json& json) :
    Node(parent, name, json) {
}

bool
LeafNode::isValue() const {
    return true;
}

void
LeafNode::getChild(const std::string& name, std::vector<const Node*>& result) const {
}

void
LeafNode::getChildren(std::vector<const Node*>& result) const {
}

void
LeafNode::getSubTreeNodes(std::vector<const Node*>& result) const {
}

void
LeafNode::search(const std::string& name, std::vector<const Node*>& result) const {
}

}
}
