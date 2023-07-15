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

#ifndef _VALUE_HH_
#define _VALUE_HH_

#include <vector>
#include <string>
#include "Node.hh"

class Value {
public:
    enum Type {
        Number,
        Bool,
        String,
        NodeSet
    };
    Value();
    Value(const Value& xd);
    Value(Value&& xd);
    Value(double d);
    Value(bool b);
    Value(const std::string& s);
    Value(const std::string& name, const nlohmann::json& json);
    Value(const Node& node);
    Value(const std::vector<Node>& ns);
    Value& operator=(const Value& xd);
    Value& operator=(Value&& xd);
    ~Value();
    Type getType() const;
    /**
     * @return true if this object represents a literal or a single node with a value.
     */
    bool isValue() const;
    double getNumber() const;
    bool getBool() const;
    /**
     * Primitiv values are converted to strings. For node sets the "string value"
     * of the first node is returned. If the node set is empty the empty string is
     * returned.
     * @return a string representation of the value.
     */
    std::string getString() const;
    /**
     * Returns the XML "string value" of the data.
     * Primitive values are interpreted as XML text nodes.
     * Objects and arrays are mapped to elements.
     * "The string-value of an element node is the concatenation of the string-values of
     *  all text node descendants of the element node in document order."
     * @return a string representation of the value.
     */
    std::string getStringValue() const;
    const std::vector<Node>& getNodeSet() const;
    Value getNodeSetSize() const;
    Value getLocalName() const;
    Value getRoot() const;
    bool operator==(const Value& xd) const;
    bool operator!=(const Value& xd) const;
    bool operator<(const Value& xd) const;
    bool operator<=(const Value& xd) const;
    bool operator>(const Value& xd) const;
    bool operator>=(const Value& xd) const;
private:
    void checkOrderingRelationArgs(const Value& xd) const;
    void assign(const Value& xd);
    void exchange(Value&& xd);
    void clear();
    Type _type;
    union Data {
        double n;
        bool b;
        std::string* s;
        std::vector<Node>* ns;
    } _d;
};

inline
bool
operator==(const Value& xd, double d) {
    const std::vector<Node>& ns = xd.getNodeSet();
    for (const Node& l : xd.getNodeSet()) {
        if (l.getNumber() == d) {
            return true;
        }
    }
    return false;
}

inline
bool
operator==(const Value& xd, const std::string& s) {
    const std::vector<Node>& ns = xd.getNodeSet();
    for (const Node& l : xd.getNodeSet()) {
        const std::string& ls = l.getString();
        if (ls == s) {
            return true;
        }
    }
    return false;
}

inline
bool
operator==(const Value& xd, bool b) {
    for (const Node& l : xd.getNodeSet()) {
        if (l.getBool() == b) {
            return true;
        }
    }
    return false;
}

inline
bool
operator!=(const Value& xd, double d) {
    const std::vector<Node>& ns = xd.getNodeSet();
    for (const Node& l : xd.getNodeSet()) {
        if (l.getNumber() != d) {
            return true;
        }
    }
    return false;
}

inline
bool
operator!=(const Value& xd, const std::string& s) {
    const std::vector<Node>& ns = xd.getNodeSet();
    for (const Node& l : xd.getNodeSet()) {
        const std::string& ls = l.getString();
        if (ls != s) {
            return true;
        }
    }
    return false;
}

inline
bool
operator!=(const Value& xd, bool b) {
    for (const Node& l : xd.getNodeSet()) {
        if (l.getBool() != b) {
            return true;
        }
    }
    return false;
}

inline
std::ostream&
operator<<(std::ostream& os, const Value& xd) {
    Value::Type type = xd.getType();
    switch(type) {
    case Value::Number:
        os << "Number: " << xd.getNumber();
        break;
    case Value::Bool:
        os << "Bool: " << xd.getBool();
        break;
    case Value::String:
        os << "String: " << xd.getString();
        break;
    case Value::NodeSet: {
        os << xd.getNodeSet();
    }
        break;
    default:
        os << "Uknown type";
        break;
    }
    return os;
}


#endif
