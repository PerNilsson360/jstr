#include <algorithm>
#include <iostream>

#include "LeafNode.hh"
#include "ArrayNode.hh"
#include "ObjectNode.hh"

namespace Jstr {
namespace Xpath {
    
ObjectNode::ObjectNode(const Node* parent, const std::string& name, const nlohmann::json& json) :
    Node(parent, name, json), _children(nullptr) {
}

ObjectNode::~ObjectNode() {
    if (_children != nullptr) {
        for (const Node* n : *_children) {
            delete n;
        }
        delete _children;
    }

}
    
void
ObjectNode::getChild(const std::string& name, std::vector<const Node*>& result) const {
    instantiateChildren();
    for (const Node* n : *_children) {
        if (n->getLocalName() == name) {
            result.emplace_back(n);
        }
    }
}

void
ObjectNode::getChildren(std::vector<const Node*>& result) const {
    instantiateChildren();
    std::copy(_children->begin(), _children->end(), std::back_inserter(result));
}
    
void
ObjectNode::getSubTreeNodes(std::vector<const Node*>& result) const {
    getChildren(result);
    for (const Node* n : *_children) {
        n->getSubTreeNodes(result);
    }
}

void
ObjectNode::search(const std::string& name, std::vector<const Node*>& result) const {
    getChild(name, result);
    for (const Node* n : *_children) {
        n->search(name, result);
    }
}

void
ObjectNode::addChildNode(const std::string& name, const nlohmann::json& child) const {
    if (child.is_array()) {
        for (size_t i = 0, size = child.size(); i < size; i++) {
            Node* n = new ArrayNode(this, name, child, i);
            _children->emplace_back(n);
        }
    } else if (child.is_object()) {
        Node* n = new ObjectNode(this, name, child);
        _children->emplace_back(n);
    } else {
        Node* n = new LeafNode(this, name, child);
        _children->emplace_back(n);
    }
}
    
void
ObjectNode::instantiateChildren() const {
    if (_children == nullptr) {
        _children = new std::vector<const Node*>();
        const nlohmann::json& j = getJson();
        for (const auto& item : j.items()) {
            addChildNode(item.key(), item.value());
        }
    }
}
    
}
}
