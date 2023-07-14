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
#include "XpathExpr.hh"
#include "XpathData.hh"

namespace {
    
void
deleteExprs(const std::list<const XpathExpr*>* l) {
    if (l != nullptr) {
        for (const XpathExpr* e : *l) {
            delete e;
        }
        delete l;
    }
}
    
}

// UnaryExpr
UnaryExpr::UnaryExpr(const XpathExpr* e) : _e(e) {}

// BinaryExpr
BinaryExpr::BinaryExpr(const XpathExpr* l, const XpathExpr* r) : _l(l), _r(r) {}

// StrExpr
StrExpr::StrExpr(const std::string& s) : _s(s) {
}

const std::string&
StrExpr::getString() const {
    return _s;
}

// MultiExpr
MultiExpr::MultiExpr(XpathExpr* e) {
    if (e != nullptr) {
        _exprs.push_back(e);
    }
}

MultiExpr::~MultiExpr() {
    for (const XpathExpr* e : _exprs) {
        delete e;
    }
    _exprs.clear();
}

void
MultiExpr::addFront(XpathExpr* e) {
  _exprs.push_front(e);
}

void
MultiExpr::addBack(XpathExpr* e) {
  _exprs.push_back(e);
}

std::list<XpathExpr*>&
MultiExpr::getExprs() {
    return _exprs;
}

// Root
XpathData
Root::eval(const XpathData& d, size_t pos, bool firstStep) const {
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
Path::Path(XpathExpr* e) : MultiExpr(e) {}

XpathData
Path::eval(const XpathData& d, size_t pos, bool firstStep) const {
    std::vector<Node> result = d.getNodeSet();
    bool first(true);
    for (const XpathExpr* e : _exprs) {
        const XpathData& tmp = e->eval(result, pos, first);
        result = tmp.getNodeSet();
        first = false;
    }
    return XpathData(result);
}


void
Path::addAbsoluteDescendant() {
    // The grammar ensures there is at least one step
    XpathExpr* step = _exprs.front();
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
Path::addRelativeDescendant(XpathExpr* step) {
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
Step::Step(const std::string& s, const std::list<const XpathExpr*>* preds) :
    StrExpr(s), _preds(preds) {
}

Step::~Step() {
    deleteExprs(_preds);
}

XpathData
Step::eval(const XpathData& d, size_t pos, bool firstStep) const {
    std::vector<Node> result;
    const std::vector<Node>& ns = d.getNodeSet();
    if (!ns.empty()) {
        evalStep(pos, firstStep, ns, result);
        evalFilter(result);
    }
    return XpathData(result);
}

void
Step::evalFilter(std::vector<Node>& result) const {
    if (_preds != nullptr) {
        for (const XpathExpr* e : *_preds) {
            std::vector<size_t> keepIndexes;
            for (int i = 0; i < result.size(); i++) {
                XpathData r = e->eval(result, i);
                if (r.getType() == XpathData::Number) {
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

const std::list<const XpathExpr*>*
Step::takePredicates() {
    const std::list<const XpathExpr*>* result = _preds;
    _preds = nullptr;
    return result;
}

XpathExpr*
Step::create(const std::string& axisName,
             const std::string& nodeTest,
             std::list<const XpathExpr*>* predicates) {
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
    } else if (axisName == "parent") {
        return new ParentStep(nodeTest, predicates);
    } else if (axisName == "self") {
        return new SelfStep(nodeTest, predicates);
    }else {
        throw std::runtime_error("Step::create not a supported step");
    }
}

bool Step::isAllStep(const XpathExpr* step) {
    return dynamic_cast<const AllStep*>(step) != nullptr;
}
bool Step::isSelfOrParentStep(const XpathExpr* step) {
    return
        dynamic_cast<const SelfStep*>(step) != nullptr ||
        dynamic_cast<const ParentStep*>(step) != nullptr;
}

AncestorStep::AncestorStep(const std::string& s, const std::list<const XpathExpr*>* preds) :
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
                                   const std::list<const XpathExpr*>* preds) :
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

AllStep::AllStep(const std::string& s, const std::list<const XpathExpr*>* preds) :
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

ChildStep::ChildStep(const std::string& s, const std::list<const XpathExpr*>* preds) :
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

ParentStep::ParentStep(const std::string& s, const std::list<const XpathExpr*>* preds) :
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

SelfStep::SelfStep(const std::string& s, const std::list<const XpathExpr*>* preds) :
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
Predicate::Predicate(const XpathExpr* e) : UnaryExpr(e) {}

XpathData
Predicate::eval(const XpathData& d, size_t pos, bool firstStep) const {
    return _e->eval(d, pos);
}

// Descendant
DescendantAll::DescendantAll(const std::list<const XpathExpr*>* preds) : Step("", preds) {
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

DescendantOrSelfAll::DescendantOrSelfAll(const std::list<const XpathExpr*>* preds) :
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
                                   const std::list<const XpathExpr*>* preds) : Step(s, preds) {
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
                                               const std::list<const XpathExpr*>* preds) :
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

// Literal
Literal::Literal(const std::string& l) : StrExpr(l) {}

XpathData
Literal::eval(const XpathData& d, size_t pos, bool firstStep) const {
  return XpathData(_s);
}

// Number
Number::Number(double d) : _d(d) {}

XpathData
Number::eval(const XpathData& d, size_t pos, bool firstStep) const {
    return XpathData(_d);
}

// Fun
Fun::Fun(const std::string& name, const std::list<const XpathExpr*>* args) :
    _name(name), _args(args) {
}

Fun::~Fun() {
    deleteExprs(_args);
}

XpathData
Fun::eval(const XpathData& d, size_t pos, bool firstStep) const {
    // TODO dont do string comparison
    if (_name == "last") {
        checkArgs(0);
        double size = d.getNodeSet().size();
        return XpathData(size);
    } else if (_name == "position") {
        checkArgs(0);
        double position = pos + 1;
        return XpathData(position);
    }else if (_name == "count") {
        checkArgs(1);
        std::list<const XpathExpr*>::const_iterator i = _args->begin();
        XpathData arg = (*i)->eval(d, pos);
        return arg.getNodeSetSize();
    } else if (_name == "local-name") {
        checkArgs(1);
        std::list<const XpathExpr*>::const_iterator i = _args->begin();
        XpathData arg = (*i)->eval(d, pos);
        return arg.getLocalName();
    } else if (_name == "string") {
        checkArgs(1);           // TODO support 0 args
        std::list<const XpathExpr*>::const_iterator i = _args->begin();
        XpathData arg = (*i)->eval(d, pos);
        return XpathData(arg.getString());
    } else if (_name == "not") {
        checkArgs(1);
        std::list<const XpathExpr*>::const_iterator i = _args->begin();
        XpathData arg = (*i)->eval(d, pos);
        return XpathData(!arg.getBool());
    } else if (_name == "true") {
        checkArgs(0);
        return XpathData(true);
    } else if (_name == "false") {
        checkArgs(0);
        return XpathData(false);
    } else {
        std::stringstream ss;
        ss << "Fun::eval unkown function: " << _name;
        throw std::runtime_error(ss.str());
    }
    return XpathData();
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
Union::Union(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Union::eval(const XpathData& d, size_t pos, bool firstStep) const {
  return XpathData();
}

// Or
Or::Or(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Or::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l.getBool() || r.getBool());
}

// And
And::And(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
And::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l.getBool() && r.getBool());
}

// Eq
Eq::Eq(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Eq::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return l == r;
}


// Ne
Ne::Ne(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Ne::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return l != r;
}

// Lt
Lt::Lt(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Lt::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l < r);
}

// Gt
Gt::Gt(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Gt::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l > r);
}

// Le
Le::Le(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Le::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l <= r);
}

// Ge
Ge::Ge(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Ge::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l >= r);

}

// Plus
Plus::Plus(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Plus::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l.getNumber() + r.getNumber());
}

// Minus
Minus::Minus(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Minus::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    if (!_r) {
        return XpathData(-l.getNumber());
    }
    XpathData r = _r->eval(d, pos);
    return XpathData(l.getNumber() - r.getNumber());
}

// Mul
Mul::Mul(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Mul::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l.getNumber() * r.getNumber());
}

// Div
Div::Div(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Div::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    return XpathData(l.getNumber() / r.getNumber());
}

// Mod
Mod::Mod(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Mod::eval(const XpathData& d, size_t pos, bool firstStep) const {
    XpathData l = _l->eval(d, pos);
    XpathData r = _r->eval(d, pos);
    double result = static_cast<int64_t>(l.getNumber()) % static_cast<int64_t>(r.getNumber());
    return XpathData(result);
}

