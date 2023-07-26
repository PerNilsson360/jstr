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

#ifndef _OBJECT_NODE_HH_
#define _OBJECT_NODE_HH_

#include <vector>
#include <Jstr.hh>

namespace Jstr {
namespace Xpath {
    
class ObjectNode : public Node {
public:
    ObjectNode() = delete;
    ObjectNode(const Node* parent, const std::string& name, const nlohmann::json& json);
    ObjectNode(const ObjectNode& node) = delete;
    ~ObjectNode();
    ObjectNode& operator=(const ObjectNode& node) = delete;
    void getChild(const std::string& name, std::vector<const Node*>& result) const override;
    void getChildren(std::vector<const Node*>& result) const override;
    void getSubTreeNodes(std::vector<const Node*>& result) const override;
    void search(const std::string& name, std::vector<const Node*>& result) const override;
protected:
    void addChildNode(const std::string& name, const nlohmann::json& child) const; 
    mutable std::vector<const Node*>* _children;
private:
    virtual void instantiateChildren() const;
};

}
}
#endif
