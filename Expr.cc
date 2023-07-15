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
#include <sstream>
#include "Expr.hh"
#include "Value.hh"

namespace {
    
void
deleteExprs(const std::list<const Expr*>* l) {
    if (l != nullptr) {
        for (const Expr* e : *l) {
            delete e;
        }
        delete l;
    }
}
    
}

// UnaryExpr
UnaryExpr::UnaryExpr(const Expr* e) : _e(e) {}

// BinaryExpr
BinaryExpr::BinaryExpr(const Expr* l, const Expr* r) : _l(l), _r(r) {}

// StrExpr
StrExpr::StrExpr(const std::string& s) : _s(s) {
}

const std::string&
StrExpr::getString() const {
    return _s;
}

// MultiExpr
MultiExpr::MultiExpr(Expr* e) {
    if (e != nullptr) {
        _exprs.push_back(e);
    }
}

MultiExpr::~MultiExpr() {
    for (const Expr* e : _exprs) {
        delete e;
    }
    _exprs.clear();
}

void
MultiExpr::addFront(Expr* e) {
  _exprs.push_front(e);
}

void
MultiExpr::addBack(Expr* e) {
  _exprs.push_back(e);
}

std::list<Expr*>&
MultiExpr::getExprs() {
    return _exprs;
}

// Root
Value
Root::eval(const Value& d, size_t pos, bool firstStep) const {
    return d.getRoot();
}

namespace {
    
std::vector<Node> filter(const std::vector<Node>& ns, const std::vector<size_t>& keepIndexes) {
    std::vector<Node> result;
    for (size_t i : keepIndexes) {
        result.emplace_back(ns[i]);
    }
    return result;
}

void
addIfUnique(std::vector<Node>& ns, const Node& node) {
    bool found(false);
    for (const Node& n : ns) {
        if (n == node) {
            found = true;
        }
    }
    if (!found) {
        ns.emplace_back(node);
    }
}

void
addIfUnique(std::vector<Node>& result, std::vector<Node>& ns) {
    for (const Node& n : ns) {
        addIfUnique(result, n);
    }
}

bool
checkLocalName(const Node& n, const std::string& name) {
    return name.empty() || name == "*" || n.getLocalName() == name;
}

}

// Path
Path::Path(Expr* e) : MultiExpr(e) {}

Value
Path::eval(const Value& d, size_t pos, bool firstStep) const {
    std::vector<Node> result = d.getNodeSet();
    bool first(true);
    for (const Expr* e : _exprs) {
        const Value& tmp = e->eval(result, pos, first);
        result = tmp.getNodeSet();
        first = false;
    }
    return Value(result);
}


void
Path::addAbsoluteDescendant() {
    // The grammar ensures there is at least one step
    Expr* step = _exprs.front();
    Step* descendant;
    if (Step::isAllStep(step)) {
        Step* s = static_cast<Step*>(step);
        descendant = new DescendantAll(s->takePredicates());
        _exprs.pop_front();
       delete step;
    } else if (Step::isSelfOrParentStep(step)) {
        descendant = new DescendantAll(nullptr);
    } else {
        Step* s = static_cast<Step*>(step);
        const std::string& stepName = s->getString();
        descendant = new DescendantSearch(stepName, s->takePredicates());
        _exprs.pop_front();
       delete step;
    }
    _exprs.push_front(descendant);
    _exprs.push_front(new Root);
}

void
Path::addRelativeDescendant(Expr* step) {
    Step* descendant;
    if (Step::isAllStep(step)) {
        Step* s = static_cast<Step*>(step);
        descendant = new DescendantAll(s->takePredicates());
       delete step;
    } else if (Step::isSelfOrParentStep(step)) {
        descendant = new DescendantAll(nullptr);
        _exprs.push_back(step);
    } else {
        Step* s = static_cast<Step*>(step);
        const std::string& stepName = s->getString();
        descendant = new DescendantSearch(stepName, s->takePredicates());
       delete step;
    }
    _exprs.push_back(descendant);
}


// Step
Step::Step(const std::string& s, const std::list<const Expr*>* preds) :
    StrExpr(s), _preds(preds) {
}

Step::~Step() {
    deleteExprs(_preds);
}

Value
Step::eval(const Value& d, size_t pos, bool firstStep) const {
    std::vector<Node> result;
    const std::vector<Node>& ns = d.getNodeSet();
    if (!ns.empty()) {
        evalStep(pos, firstStep, ns, result);
        evalFilter(result);
    }
    return Value(result);
}

void
Step::evalFilter(std::vector<Node>& result) const {
    if (_preds != nullptr) {
        for (const Expr* e : *_preds) {
            std::vector<size_t> keepIndexes;
            for (int i = 0; i < result.size(); i++) {
                Value r = e->eval(result, i);
                if (r.getType() == Value::Number) {
                    if (i + 1 == r.getNumber()) {
                        keepIndexes.emplace_back(i);
                    }
                } else {
                    if (r.getBool()) {
                        keepIndexes.emplace_back(i);
                    }
                }
            }
            result = filter(result, keepIndexes);
        }
    }
}

const std::list<const Expr*>*
Step::takePredicates() {
    const std::list<const Expr*>* result = _preds;
    _preds = nullptr;
    return result;
}

Expr*
Step::create(const std::string& axisName,
             const std::string& nodeTest,
             std::list<const Expr*>* predicates) {
    if (axisName.empty()) {
        if (nodeTest == "..") {
            return new ParentStep("", predicates);
        } else if (nodeTest == ".") {
            return new SelfStep("", predicates);
        } else if (nodeTest == "*") {
            return new AllStep("", predicates);
        } else {
            return new ChildStep(nodeTest, predicates);
        }
    } else if (axisName == "ancestor") {
        return new AncestorStep(nodeTest, predicates);
    }
    else if (axisName == "ancestor-or-self") {
        return new AncestorSelfStep(nodeTest, predicates);
    } else if (axisName == "child") {
        if (nodeTest == "*") {
            return new AllStep(nodeTest, predicates);
        } else {
            return new ChildStep(nodeTest, predicates);
        }
    } else if (axisName == "descendant") {
        if (nodeTest == "*") {
            return new DescendantAll(predicates);
        } else {
            return new DescendantSearch(nodeTest, predicates);
        }
    } else if (axisName == "descendant-or-self") {
        if (nodeTest == "*") {
            return new DescendantOrSelfAll(predicates);
        } else {
            return new DescendantOrSelfSearch(nodeTest, predicates);
        }
    } else if (axisName == "following-sibling") {
        if (nodeTest == "*") {
            return new FollowingSiblingAll(predicates);
        } else {
            return new FollowingSiblingSearch(nodeTest, predicates);
        }
    } else if (axisName == "parent") {
        return new ParentStep(nodeTest, predicates);
    } else if (axisName == "self") {
        return new SelfStep(nodeTest, predicates);
    }else {
        throw std::runtime_error("Step::create not a supported step");
    }
}

bool Step::isAllStep(const Expr* step) {
    return dynamic_cast<const AllStep*>(step) != nullptr;
}
bool Step::isSelfOrParentStep(const Expr* step) {
    return
        dynamic_cast<const SelfStep*>(step) != nullptr ||
        dynamic_cast<const ParentStep*>(step) != nullptr;
}

AncestorStep::AncestorStep(const std::string& s, const std::list<const Expr*>* preds) :
    Step(s, preds) {
}

void
AncestorStep::evalStep(size_t pos,
                       bool firstStep,
                       const std::vector<Node>& nodeSet,
                       std::vector<Node>& result) const {
    std::vector<Node> tmp1;
    if (firstStep) {
        const Node& n = nodeSet[pos];
        n.getAncestors(tmp1);
    } else {
        for (const Node& n : nodeSet) {
            std::vector<Node> tmp2;
            n.getAncestors(tmp2);
            addIfUnique(tmp1, tmp2);
        }
    }
    // TODO Could be more efficient with search instead of filter
    if (_s != "*") {
        for (const Node& n : tmp1) {
            if (n.getLocalName() == _s) {
                result.emplace_back(n);
            }
        }
    } else {
        result = tmp1;
    }
}

AncestorSelfStep::AncestorSelfStep(const std::string& s,
                                   const std::list<const Expr*>* preds) :
    AncestorStep(s, preds) {
}

void
AncestorSelfStep::evalStep(size_t pos,
                           bool firstStep,
                           const std::vector<Node>& nodeSet,
                           std::vector<Node>& result) const {
    AncestorStep::evalStep(pos, firstStep, nodeSet, result);
    if (firstStep) {
        const Node& n = nodeSet[pos];
        if (checkLocalName(n, _s)) {
            result.emplace_back(n);
        }
    } else {
        for (const Node& n : nodeSet) {
            if (checkLocalName(n, _s)) { // TODO: dont need to check every iteration or?
                result.emplace_back(n);
            }
        }
    }
}

AllStep::AllStep(const std::string& s, const std::list<const Expr*>* preds) :
    Step(s, preds) {
}

void
AllStep::evalStep(size_t pos,
                  bool firstStep,
                  const std::vector<Node>& nodeSet,
                  std::vector<Node>& result) const {
    if (firstStep) {
        const Node& n = nodeSet[pos];
        n.getChildren(result);
    } else {
        for (const Node& n : nodeSet) {
            n.getChildren(result);
        }
    }
}

ChildStep::ChildStep(const std::string& s, const std::list<const Expr*>* preds) :
    Step(s, preds) {
}

void
ChildStep::evalStep(size_t pos,
                    bool firstStep,
                    const std::vector<Node>& nodeSet,
                    std::vector<Node>& result) const {
    if (firstStep) {
        const Node& n = nodeSet[pos];
        n.getChild(_s, result);
    } else {
        for (const Node& n : nodeSet) {
            n.getChild(_s, result);
        }
    }
}

ParentStep::ParentStep(const std::string& s, const std::list<const Expr*>* preds) :
    Step(s, preds) {
}

void
ParentStep::evalStep(size_t pos,
                     bool firstStep,
                     const std::vector<Node>& nodeSet,
                     std::vector<Node>& result) const {
    if (firstStep) {
        const Node& n = nodeSet[pos];
        const Node* parent = n.getParent();
        if (parent != nullptr && checkLocalName(*parent, _s)) {
            addIfUnique(result, Node(*parent));
        }
    } else {
        for (const Node& n : nodeSet) {
            const Node* parent = n.getParent();
            if (parent != nullptr && checkLocalName(*parent, _s)) {
                addIfUnique(result, Node(*parent));
            }
        }
    }
}

SelfStep::SelfStep(const std::string& s, const std::list<const Expr*>* preds) :
    Step(s, preds) {
}

void
SelfStep::evalStep(size_t pos,
                   bool firstStep,
                   const std::vector<Node>& nodeSet,
                   std::vector<Node>& result) const {
    if (firstStep) {
        const Node& n = nodeSet[pos];
        if (checkLocalName(n, _s)) {
            result = std::vector<Node>(1, nodeSet[pos]);
        }
    } else {
        if (_s.empty() || _s == "*") {
            result = nodeSet;
        } else {
            for (const Node& n : nodeSet) {
                if (n.getLocalName() == _s) {
                    result.push_back(n);
                }
            }
        }
    }
}

// Predicate
Predicate::Predicate(const Expr* e) : UnaryExpr(e) {}

Value
Predicate::eval(const Value& d, size_t pos, bool firstStep) const {
    return _e->eval(d, pos);
}

// Descendant
DescendantAll::DescendantAll(const std::list<const Expr*>* preds) : Step("", preds) {
}

void
DescendantAll::evalStep(size_t pos,
                        bool firstStep,
                        const std::vector<Node>& nodeSet,
                        std::vector<Node>& result) const {
    for (const Node& n : nodeSet) {
        n.getSubTreeNodes(result);
    }
}

DescendantOrSelfAll::DescendantOrSelfAll(const std::list<const Expr*>* preds) :
    DescendantAll(preds) {
}

void
DescendantOrSelfAll::evalStep(size_t pos,
                              bool firstStep,
                              const std::vector<Node>& nodeSet,
                              std::vector<Node>& result) const {
    
    for (const Node& n : nodeSet) {
        result.emplace_back(n);
    }
    DescendantAll::evalStep(pos, firstStep, nodeSet, result);
}


DescendantSearch::DescendantSearch(const std::string& s,
                                   const std::list<const Expr*>* preds) : Step(s, preds) {
}

void
DescendantSearch::evalStep(size_t pos,
                           bool firstStep,
                           const std::vector<Node>& nodeSet,
                           std::vector<Node>& result) const {
    for (const Node& n : nodeSet) {
        n.search(_s, result);
    }
}

DescendantOrSelfSearch::DescendantOrSelfSearch(const std::string& s,
                                               const std::list<const Expr*>* preds) :
    DescendantSearch(s, preds) {
}

void
DescendantOrSelfSearch::evalStep(size_t pos,
                                 bool firstStep,
                                 const std::vector<Node>& nodeSet,
                                 std::vector<Node>& result) const {
    for (const Node& n : nodeSet) {
        result.emplace_back(n);
    }
    DescendantSearch::evalStep(pos, firstStep, nodeSet, result);
}

// FollowingSibling
namespace {
size_t
findPosition(const Node& node, std::vector<Node>& children) {
    size_t i = 0;
    size_t size = children.size();
    for (; i < size; i++) {
        if (node == children[i]) {
            break;
        }
    }
    if (i > size) {
        throw std::runtime_error("findPosition: could not find child in node set.");
    }
    return i;
}
}

FollowingSiblingAll::FollowingSiblingAll(const std::list<const Expr*>* preds) :
    Step("", preds) {
}

void
FollowingSiblingAll::evalStep(size_t pos,
                               bool firstStep,
                               const std::vector<Node>& nodeSet,
                               std::vector<Node>& result) const {
    if (!firstStep) {
        // All nodes in this node set must have same parent right!
        pos = 0;
    }
    const Node& node = nodeSet[pos];
    const Node* parent = node.getParent();
    if (parent == nullptr) {
        return;
    }
    std::vector<Node> children;
    parent->getChildren(children);
    size_t position = findPosition(node, children) + 1; // skip the current node
    for (size_t i = position, size = children.size(); i < size; i++) {
        result.emplace_back(children[i]);
    }
}

FollowingSiblingSearch::FollowingSiblingSearch(const std::string& s,
                                               const std::list<const Expr*>* preds) :
    Step(s, preds) {
}

void
FollowingSiblingSearch::evalStep(size_t pos,
                                 bool firstStep,
                                 const std::vector<Node>& nodeSet,
                                 std::vector<Node>& result) const {
    if (!firstStep) {
        // All nodes in this node set must have same parent right!
        pos = 0;
    }
    const Node& node = nodeSet[pos];
    const Node* parent = node.getParent();
    if (parent == nullptr) {
        return;
    }
    std::vector<Node> children;
    parent->getChildren(children);
    size_t position = findPosition(node, children) + 1; // skip first node
    for (size_t i = position, size = children.size(); i < size; i++) {
        const Node& child = children[i];
        if (child.getLocalName() == _s) {
            result.emplace_back(child);
        }
    }
}

// Literal
Literal::Literal(const std::string& l) : StrExpr(l) {}

Value
Literal::eval(const Value& d, size_t pos, bool firstStep) const {
  return Value(_s);
}

// Number
Number::Number(double d) : _d(d) {}

Value
Number::eval(const Value& d, size_t pos, bool firstStep) const {
    return Value(_d);
}

// Fun
Fun::Fun(const std::string& name, const std::list<const Expr*>* args) :
    _name(name), _args(args) {
}

Fun::~Fun() {
    deleteExprs(_args);
}

Value
Fun::eval(const Value& d, size_t pos, bool firstStep) const {
    // TODO dont do string comparison
    if (_name == "last") {
        checkArgs(0);
        double size = d.getNodeSet().size();
        return Value(size);
    } else if (_name == "position") {
        checkArgs(0);
        double position = pos + 1;
        return Value(position);
    }else if (_name == "count") {
        checkArgs(1);
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return arg.getNodeSetSize();
    } else if (_name == "local-name") {
        checkArgs(1);
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return arg.getLocalName();
    } else if (_name == "string") {
        checkArgs(1);           // TODO support 0 args
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return Value(arg.getString());
    } else if (_name == "not") {
        checkArgs(1);
        std::list<const Expr*>::const_iterator i = _args->begin();
        Value arg = (*i)->eval(d, pos);
        return Value(!arg.getBool());
    } else if (_name == "true") {
        checkArgs(0);
        return Value(true);
    } else if (_name == "false") {
        checkArgs(0);
        return Value(false);
    } else {
        std::stringstream ss;
        ss << "Fun::eval unkown function: " << _name;
        throw std::runtime_error(ss.str());
    }
    return Value();
}

void
Fun::checkArgs(size_t expectedSize) const {
    size_t nArgs = _args == nullptr ? 0 : _args->size();
    if (nArgs != expectedSize) {
        std::stringstream ss;
        ss << "Fun::eval wrong number arguments to function, " << _name
           << " expected: "<< expectedSize << " actual: " << nArgs;
        throw std::runtime_error(ss.str());
    }
}

// Union
Union::Union(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Union::eval(const Value& d, size_t pos, bool firstStep) const {
  return Value();
}

// Or
Or::Or(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Or::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l.getBool() || r.getBool());
}

// And
And::And(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
And::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l.getBool() && r.getBool());
}

// Eq
Eq::Eq(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Eq::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return l == r;
}


// Ne
Ne::Ne(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Ne::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return l != r;
}

// Lt
Lt::Lt(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Lt::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l < r);
}

// Gt
Gt::Gt(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Gt::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l > r);
}

// Le
Le::Le(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Le::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l <= r);
}

// Ge
Ge::Ge(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Ge::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l >= r);

}

// Plus
Plus::Plus(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Plus::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l.getNumber() + r.getNumber());
}

// Minus
Minus::Minus(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Minus::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    if (!_r) {
        return Value(-l.getNumber());
    }
    Value r = _r->eval(d, pos);
    return Value(l.getNumber() - r.getNumber());
}

// Mul
Mul::Mul(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Mul::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l.getNumber() * r.getNumber());
}

// Div
Div::Div(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Div::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    return Value(l.getNumber() / r.getNumber());
}

// Mod
Mod::Mod(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Mod::eval(const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(d, pos);
    Value r = _r->eval(d, pos);
    double result = static_cast<int64_t>(l.getNumber()) % static_cast<int64_t>(r.getNumber());
    return Value(result);
}

