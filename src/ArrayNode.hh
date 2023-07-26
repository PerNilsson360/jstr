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

#ifndef _ARRAY_NODE_HH_
#define _ARRAY_NODE_HH_

#include "ObjectNode.hh"

namespace Jstr {
namespace Xpath {

class ArrayNode : public ObjectNode {
public:
    ArrayNode() = delete;
    ArrayNode(const Node* parent, const std::string& name, const nlohmann::json& json, int64_t i);
    ArrayNode(const ArrayNode& node) = delete;
    ArrayNode& operator=(const ArrayNode& node) = delete;
    bool isValue() const override;
    const nlohmann::json& getJson() const override;
    bool isArrayChild() const override;
    //void getSubTreeNodes(std::vector<const Node*>& result) const override;
private:
    void instantiateChildren() const;
    int64_t _i;
};

}
}

#endif
