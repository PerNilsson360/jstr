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

#ifndef _NODE_HH_
#define _NODE_HH_

#include <memory>
#include <nlohmann/json.hpp>

class Node {
public:
    Node();
    Node(const std::string& name, const nlohmann::json& json, int64_t i = -1);
    Node(const Node* parent, const std::string& name, const nlohmann::json& json, int64_t i = -1);
    Node(const Node& node);
    Node& operator=(const Node& node);
    Node getRoot() const;
    const Node* getParent() const;
    const nlohmann::json& getJson() const;
    bool isValue() const;
    double getNumber() const;
    bool getBool() const;
    std::string getString() const;
    const std::string& getLocalName() const;
    bool isArrayChild() const;
    void getAncestors(std::vector<Node>& result) const;
    void getChild(const std::string& name, std::vector<Node>& result) const;
    void getChildren(std::vector<Node>& result) const;
    void getSubTreeNodes(std::vector<Node>& result) const;
    void search(const std::string& name, std::vector<Node>& result) const;
    bool operator==(const Node& r) const;
private:
    void assign(const Node& node);
    std::unique_ptr<const Node> _parent;
    std::string _name;
    const nlohmann::json* _json;
    int64_t _i;
};

inline
std::ostream&
operator<<(std::ostream& os, const Node& n) {
    os << n.getJson().dump();
    return os;
}

inline
std::ostream&
operator<<(std::ostream& os, const std::vector<Node>& ns) {
    os << "[";
    bool first(true);
    std::string separator;
    for (const Node& n : ns) {
        os << separator << n;
        if (first) {
            separator = ", ";
        }
    }
    os << "]";
    return os;
}


#endif
