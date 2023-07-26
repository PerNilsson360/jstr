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

#include <sstream>
#include <iostream>
#include <stdexcept>

#include "Jstr.hh"
#include "ObjectNode.hh"

namespace Jstr {
namespace Xpath {

Env::Env(const Value& context) : _context(context) {
    const std::vector<const Node*>& nodeSet = context.getNodeSet();
    if (context.getType() == Value::NodeSet) {
        _root.reset(new Value(context.getRoot()));
        if (nodeSet.size() != 1) {
            throw std::runtime_error("Env::Env context node set must have size 1.");
        }
    }
}

const Value&
Env::getCurrent() const {
    return _context;
}

const Value&
Env::getRoot() const {
    if (!_root) {
        throw std::runtime_error("Env::Env context node is not node set, i.e. no root present");
    }
    const Value* root = _root.get();
    return *root;
}
    
void
Env::addVariable(const std::string& name, const Value& val) {
    if (!_vals.emplace(name, val).second) {
        std::stringstream ss;
        ss << "Env::addVariable " << name << " is allready added.";
        throw std::runtime_error(ss.str());
    }
}

const Value&
Env::getVariable(const std::string& name) const {
    std::map<std::string, Value>::const_iterator i = _vals.find(name);
    if (i == _vals.end()) {
        std::stringstream ss;
        ss << "Env::getVariable can not find: " << name;
        throw std::runtime_error(ss.str());
    }
    return i->second;
}

}
}
