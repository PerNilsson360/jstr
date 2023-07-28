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
#include <Jstr.hh>

#include "Utils.hh"
#include "Expr.hh"

namespace {
using namespace Jstr::Xpath;    
    
std::vector<const Node*>
filter(const std::vector<const Node*>& ns, const std::vector<size_t>& keepIndexes) {
    std::vector<const Node*> result;
    for (size_t i : keepIndexes) {
        result.emplace_back(ns[i]);
    }
    return result;
}

void
addIfUnique(std::vector<const Node*>& result, const std::vector<const Node*>& ns) {
    for (const Node* n : ns) {
        addIfUnique(result, n);
    }
}
  
bool
checkLocalName(const Node* n, const std::string& name) {
    return name.empty() || name == "*" || n->getLocalName() == name;
}

std::vector<const Node*>&
concatenate(std::vector<const Node*>& u, const std::vector<const Node*>& v) {
    u.insert(u.end(), v.begin(), v.end());
    return u;
}
}

namespace Jstr {
namespace Xpath {


// Expr
Expr::Expr() : _preds(nullptr) {
}

Expr::~Expr() {
    deleteExprs(_preds);
    _preds = nullptr;
}

Value
Expr::eval(const Env& e, const Value& v, size_t pos, bool firstStep) const {
    if (_preds == nullptr) {
        return evalExpr(e, v, pos, firstStep);
    } else {
        const Value& val = evalExpr(e, v, pos, firstStep);
        return evalFilter(e, val);
    }
}

void
Expr::addPredicates(const std::list<const Expr*>* preds) {
    _preds = preds;
}

const std::list<const Expr*>*
Expr::takePredicates() {
    const std::list<const Expr*>* result = _preds;
    _preds = nullptr;
    return result;
}

Value
Expr::evalFilter(const Env& env, const Value& val) const {
    if (val.getType() != Value::NodeSet) {
        bool keep(true);
        for (const Expr* pred : *_preds) {
            Value r = pred->eval(env, val, 0);
            keep &= r.getBoolean();
        }
        return keep ? val : Value();
    }
    std::vector<const Node*> result = val.getNodeSet();
    for (const Expr* pred : *_preds) {
        std::vector<size_t> keepIndexes;
        for (int i = 0; i < result.size(); i++) {
            Value r = pred->eval(env, result, i);
            if (r.getType() == Value::Number) {
                if (i + 1 == r.getNumber()) {
                    keepIndexes.emplace_back(i);
                }
            } else {
                if (r.getBoolean()) {
                    keepIndexes.emplace_back(i);
                }
            }
        }
        result = filter(result, keepIndexes);
    }
    return Value(result);
}

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
Root::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    return e.getRoot();
}

// Path
Path::Path(Expr* e) : MultiExpr(e) {}

Value
Path::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    Value result = val;
    bool first(true);
    for (const Expr* exp : _exprs) {
        result = exp->eval(env, result, pos, first);
        first = false;
    }
    return result;
}

PathItem*
Path::createDescendant() {
    // The grammar ensures there is at least one step
    Expr* step = _exprs.front();
    PathItem* descendant;
    if (Step::isAllStep(step)) {
        Step* s = static_cast<Step*>(step);
        descendant = new DescendantAll();
        descendant->addPredicates(s->takePredicates());
        _exprs.pop_front();
       delete step;
    } else if (Step::isSelfOrParentStep(step)) {
        descendant = new DescendantAll();
    } else {
        Step* s = static_cast<Step*>(step);
        const std::string& stepName = s->getString();
        descendant = new DescendantSearch(stepName);
        descendant->addPredicates(s->takePredicates());
        _exprs.pop_front();
       delete step;
    }
    return descendant;
}
    
void
Path::addAbsoluteDescendant() {
    PathItem* item = createDescendant();
    _exprs.push_front(item);
    _exprs.push_front(new Root);
}

void
Path::addRelativeDescendant(Expr* step) {
    PathItem* descendant;
    if (Step::isAllStep(step)) {
        Step* s = static_cast<Step*>(step);
        descendant = new DescendantAll();
        descendant->addPredicates(s->takePredicates());
       delete step;
    } else if (Step::isSelfOrParentStep(step)) {
        descendant = new DescendantAll();
        _exprs.push_back(step);
    } else {
        Step* s = static_cast<Step*>(step);
        const std::string& stepName = s->getString();
        descendant = new DescendantSearch(stepName);
        descendant->addPredicates(s->takePredicates());
       delete step;
    }
    _exprs.push_back(descendant);
}

void
Path::addRelativeDescendant() {
    PathItem* item = createDescendant();
    _exprs.push_back(item);
}

// Step
Step::Step(const std::string& s) : StrExpr(s) {
}

Expr*
Step::create(const std::string& axisName, const std::string& nodeTest) {
    if (axisName.empty()) {
        if (nodeTest == "..") {
            return new ParentStep("");
        } else if (nodeTest == ".") {
            return new SelfStep("");
        } else if (nodeTest == "*") {
            return new AllStep();
        } else {
            return new ChildStep(nodeTest);
        }
    } else if (axisName == "ancestor") {
        return new AncestorStep(nodeTest);
    }
    else if (axisName == "ancestor-or-self") {
        return new AncestorSelfStep(nodeTest);
    } else if (axisName == "child") {
        if (nodeTest == "*") {
            return new AllStep();
        } else {
            return new ChildStep(nodeTest);
        }
    } else if (axisName == "descendant") {
        if (nodeTest == "*") {
            return new DescendantAll();
        } else {
            return new DescendantSearch(nodeTest);
        }
    } else if (axisName == "descendant-or-self") {
        if (nodeTest == "*") {
            return new DescendantOrSelfAll();
        } else {
            return new DescendantOrSelfSearch(nodeTest);
        }
    } else if (axisName == "following-sibling") {
        if (nodeTest == "*") {
            return new FollowingSiblingAll();
        } else {
            return new FollowingSiblingSearch(nodeTest);
        }
    } else if (axisName == "parent") {
        return new ParentStep(nodeTest);
    } else if (axisName == "self") {
        return new SelfStep(nodeTest);
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

AncestorStep::AncestorStep(const std::string& s) : Step(s) {
}

Value
AncestorStep::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> tmp1;
    if (firstStep) {
        const Node* n = nodeSet[pos];
        n->getAncestors(tmp1);
    } else {
        for (const Node* n : nodeSet) {
            std::vector<const Node*> tmp2;
            n->getAncestors(tmp2);
            ::addIfUnique(tmp1, tmp2);
        }
    }
    std::vector<const Node*> result;
    // TODO Could be more efficient with search instead of filter
    if (_s != "*") {
        for (const Node* n : tmp1) {
            if (n->getLocalName() == _s) {
                result.emplace_back(n);
            }
        }
    } else {
        result = tmp1;
    }
    return Value(result);
}

AncestorSelfStep::AncestorSelfStep(const std::string& s) :
    AncestorStep(s) {
}

Value
AncestorSelfStep::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    Value tmp = AncestorStep::evalExpr(env, val, pos, firstStep);
    std::vector<const Node*> result = tmp.getNodeSet();
    if (firstStep) {
        const Node* n = nodeSet[pos];
        if (checkLocalName(n, _s)) {
            result.emplace_back(n); // TODO should this not be added front
        }
    } else {
        for (const Node* n : nodeSet) {
            if (checkLocalName(n, _s)) { // TODO: dont need to check every iteration or?
                result.emplace_back(n);
            }
        }
    }
    return Value(result);
}

Value
AllStep::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    if (firstStep) {
        const Node* n = nodeSet[pos];
        n->getChildren(result);
    } else {
        for (const Node* n : nodeSet) {
            n->getChildren(result);
        }
    }
    return Value(result);
}

ChildStep::ChildStep(const std::string& s) :
    Step(s) {
}

Value
ChildStep::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    if (firstStep) {
        const Node* n = nodeSet[pos];
        n->getChild(_s, result);
    } else {
        for (const Node* n : nodeSet) {
            n->getChild(_s, result);
        }
    }
    return result;
}

ParentStep::ParentStep(const std::string& s) :
    Step(s) {
}

Value
ParentStep::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    if (firstStep) {
        const Node* n = nodeSet[pos];
        const Node* parent = n->getParent();
        if (parent != nullptr && checkLocalName(parent, _s)) {
            addIfUnique(result, parent);
        }
    } else {
        for (const Node* n : nodeSet) {
            const Node* parent = n->getParent();
            if (parent != nullptr && checkLocalName(parent, _s)) {
                addIfUnique(result, parent);
            }
        }
    }
    return Value(result);
}

SelfStep::SelfStep(const std::string& s) :
    Step(s) {
}

Value
SelfStep::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    if (val.getType() != Value::NodeSet) {
        return val;
    } else {
        const std::vector<const Node*>& nodeSet = val.getNodeSet();
        std::vector<const Node*> result;
        if (firstStep) {
            const Node* n = nodeSet[pos];
            if (checkLocalName(n, _s)) {
                result.emplace_back(nodeSet[pos]);
            }
        } else {
            if (_s.empty() || _s == "*") {
                result = nodeSet;
            } else {
                for (const Node* n : nodeSet) {
                    if (n->getLocalName() == _s) {
                        result.emplace_back(n);
                    }
                }
            }
        }
        return Value(result);
    }
}

// Predicate
Predicate::Predicate(const Expr* e) : _e(e) {
}

Value
Predicate::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    return _e->eval(env, val, pos);
}

// Descendant
DescendantAll::DescendantAll() {
}

Value
DescendantAll::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    for (const Node* n : nodeSet) {
        n->getSubTreeNodes(result);
    }
    return Value(result);
}

DescendantOrSelfAll::DescendantOrSelfAll() :
    DescendantAll() {
}

Value
DescendantOrSelfAll::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    for (const Node* n : nodeSet) {
        result.emplace_back(n);
    }
    const Value& d = DescendantAll::evalExpr(env, val, pos, firstStep); // TODO avoid extra copy
    return concatenate(result, d.getNodeSet());
}


DescendantSearch::DescendantSearch(const std::string& s) : Step(s) {
}

Value
DescendantSearch::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    for (const Node* n : nodeSet) {
        n->search(_s, result);
    }
    return Value(result);
}

DescendantOrSelfSearch::DescendantOrSelfSearch(const std::string& s) :
    DescendantSearch(s) {
}

Value
DescendantOrSelfSearch::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    for (const Node* n : nodeSet) {
        result.emplace_back(n);
    }
    const Value& d = DescendantSearch::evalExpr(env, val, pos, firstStep);
    return concatenate(result, d.getNodeSet());
}

// FollowingSibling
namespace {
size_t
findPosition(const Node* node, std::vector<const Node*>& children) {
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

FollowingSiblingAll::FollowingSiblingAll() : Step("") {
}

Value
FollowingSiblingAll::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    if (!firstStep) {
        // All nodes in this node set must have same parent right!
        pos = 0;
    }
    const Node* node = nodeSet[pos];
    const Node* parent = node->getParent();
    if (parent == nullptr) {
        return Value(result);
    }
    std::vector<const Node*> children;
    parent->getChildren(children);
    size_t position = findPosition(node, children) + 1; // skip the current node
    for (size_t i = position, size = children.size(); i < size; i++) {
        result.emplace_back(children[i]);
    }
    return Value(result);
}

FollowingSiblingSearch::FollowingSiblingSearch(const std::string& s) : Step(s) {
}

Value
FollowingSiblingSearch::evalExpr(const Env& env, const Value& val, size_t pos, bool firstStep) const {
    const std::vector<const Node*>& nodeSet = val.getNodeSet();
    std::vector<const Node*> result;
    if (!firstStep) {
        // All nodes in this node set must have same parent right!
        pos = 0;
    }
    const Node* node = nodeSet[pos];
    const Node* parent = node->getParent();
    if (parent == nullptr) {
        return Value(result);
    }
    std::vector<const Node*> children;
    parent->getChildren(children);
    size_t position = findPosition(node, children) + 1; // skip first node
    for (size_t i = position, size = children.size(); i < size; i++) {
        const Node* child = children[i];
        if (child->getLocalName() == _s) {
            result.emplace_back(child);
        }
    }
    return Value(result);
}

// Literal
Literal::Literal(const std::string& l) : StrExpr(l) {}

Value
Literal::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
  return Value(_s);
}

// Number
Number::Number(double d) : _d(d) {}

Value
Number::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    return Value(_d);
}

// Union
Union::Union(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Union::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return l.nodeSetUnion(r);
}

// Or
Or::Or(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Or::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l.getBoolean() || r.getBoolean());
}

// And
And::And(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
And::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l.getBoolean() && r.getBoolean());
}

// Eq
Eq::Eq(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Eq::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return l == r;
}

// Ne
Ne::Ne(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Ne::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return l != r;
}

// Lt
Lt::Lt(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Lt::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l < r);
}

// Gt
Gt::Gt(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Gt::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l > r);
}

// Le
Le::Le(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Le::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l <= r);
}

// Ge
Ge::Ge(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Ge::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l >= r);

}

// Plus
Plus::Plus(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Plus::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l.getNumber() + r.getNumber());
}

// Minus
Minus::Minus(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Minus::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    if (!_r) {
        Value v(-l.getNumber());
        return Value(-l.getNumber());
    }
    Value r = _r->eval(e, d, pos);
    return Value(l.getNumber() - r.getNumber());
}

// Mul
Mul::Mul(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Mul::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l.getNumber() * r.getNumber());
}

// Div
Div::Div(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Div::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    return Value(l.getNumber() / r.getNumber());
}

// Mod
Mod::Mod(const Expr* l, const Expr* r) : BinaryExpr(l, r) {}

Value
Mod::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    Value l = _l->eval(e, d, pos);
    Value r = _r->eval(e, d, pos);
    double result = static_cast<int64_t>(l.getNumber()) % static_cast<int64_t>(r.getNumber());
    return Value(result);
}

// VarRef
VarRef::VarRef(const std::string& s) : StrExpr(s) {}

Value
VarRef::evalExpr(const Env& e, const Value& d, size_t pos, bool firstStep) const {
    return e.getVariable(_s);
}

}
}
