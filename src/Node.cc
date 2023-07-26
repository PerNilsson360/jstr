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
#include <nlohmann/json.hpp>
#include <Jstr.hh>

namespace {
    using namespace Jstr::Xpath;
    
void
getString(const nlohmann::json& json, std::string& r) {
    if (json.is_string()) {
        r += json.get<std::string>(); // Dont want quotation marks you get with dump
    }else if (json.is_primitive()) {
        r += json.dump();       // TODO: check is number NAN "NaN" which is xml syntax
    } else {
        for (const nlohmann::json& j: json) {
            getString(j, r);
        }
    }
}

}

namespace Jstr {
namespace Xpath {

Node::Node() : _parent(nullptr), _json(nullptr) {}

Node::Node(const Node* parent, const std::string& name, const nlohmann::json& json) :
    _parent(parent), _json(&json), _name(name) {
}

Node::~Node() {
}
    
const Node*
Node::getRoot() const {
    if (_parent == nullptr) {
        return this;
    } else {
        return _parent->getRoot();
    }
}

const Node*
Node::getParent() const {
    return _parent;
}

const nlohmann::json&
Node::getJson() const {
    return *_json;
}

bool
Node::isValue() const {
    return false;
}

double
Node::getNumber() const {
    const nlohmann::json& j = getJson();
    if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_string()) {
        const std::string& s = j.get<std::string>();
        return s.empty() ? NAN : std::stod(s);
    } else if (j.is_boolean()) {
        return j.get<bool>() ? 1 : 0;
    } else {
        return NAN;
    }
}

bool
Node::getBoolean() const {
    const nlohmann::json& j = getJson();
    if (j.is_number()) {
        double d = j.get<double>();
        return (d == 0 || d == NAN) ? false : true;
    } else if (j.is_string()) {
        const std::string& s = j.get<std::string>();
        return !s.empty();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else {
        return true;            // object or object in an array
    }   
}

std::string
Node::getString() const {
    std::string r;
    ::getString(getJson(), r);
    return r;
}

const std::string&
Node::getLocalName() const {
    return _name;
}

bool
Node::isArrayChild() const {
    return false;
}

void
Node::getAncestors(std::vector<const Node*>& result) const {
    for (const Node* parent = getParent(); parent != nullptr; parent = parent->getParent()) {
        result.emplace_back(parent);
    }
}

}
}
