// MIT license
//
// Copyright 2023 Per Nilsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

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
