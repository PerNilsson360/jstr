#include <iostream>
#include "ArrayNode.hh"
#include "LeafNode.hh"

namespace Jstr {
namespace Xpath {

ArrayNode::ArrayNode(const Node* parent,
                     const std::string& name,
                     const nlohmann::json& json,
                     int64_t i) : ObjectNode(parent, name, json), _i(i) {
}

bool
ArrayNode::isValue() const {
    const nlohmann::json& j = getJson();
    return j.is_primitive();
}

const nlohmann::json&
ArrayNode::getJson() const {
    return (*_json)[_i];
}

bool
ArrayNode::isArrayChild() const {
    return true;
}

void
ArrayNode::instantiateChildren() const {
    if (_children == nullptr) {
        _children = new std::vector<const Node*>();
        const nlohmann::json& j = getJson();
        if (j.is_object()) {
            for (const auto& item : j.items()) {
                for (const auto& item : j.items()) {
                    addChildNode(item.key(), item.value());
                }
            }
        }
    }
}

}
}
