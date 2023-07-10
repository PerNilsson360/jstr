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
MultiExpr::MultiExpr(const XpathExpr* e) {
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
MultiExpr::addFront(const XpathExpr* e) {
  _exprs.push_front(e);
}

void
MultiExpr::addBack(const XpathExpr* e) {
  _exprs.push_back(e);
}

const std::list<const XpathExpr*>&
MultiExpr::getExprs() const {
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

void addIfUnique(std::vector<Node>& ns, Node&& node) {
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
	
}

// Path
Path::Path(const XpathExpr* e) : MultiExpr(e) {}

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
	if (ns.empty()) {
		return XpathData(result);
	}
	if (_s == "..") {			// TODO avoid string comparison
		if (firstStep) {
			const Node& n = ns[pos];
			const Node* parent = n.getParent();
			if (parent != nullptr) {
				addIfUnique(result, Node(*parent));
			}
		} else {
			for (const Node& n : ns) {
				const Node* parent = n.getParent();
				if (parent != nullptr) {
					addIfUnique(result, Node(*parent));
				}
			}
		}
	} else if (_s == ".") {
		if (firstStep) {
			result = std::vector<Node>(1, ns[pos]);
		} else {
			result = ns;
		}
	} else if (_s == "*") {
		if (firstStep) {
			const Node& n = ns[pos];
			n.getChildren(result);
		} else {
			for (const Node& n : ns) {
				n.getChildren(result);
			}
		}
	} else {
		if (firstStep) {
			const Node& n = ns[pos];
			n.getChild(_s, result);
		} else {
			for (const Node& n : ns) {
				n.getChild(_s, result);
			}
		}
	}
	// Apply filter
	if (_preds != nullptr) {
		for (const XpathExpr* e : *_preds) {
			std::vector<size_t> keepIndexes;
			for (int i = 0; i < result.size(); i++) {
				XpathData r = e->eval(result, i);
				if (r.getType() == XpathData::Number) {
					if (i + 1 == r.getNumber()) {
						keepIndexes.push_back(i);
					}
				} else {
					if (r.getBool()) {
						keepIndexes.push_back(i);
					}
				}
			}
			result = filter(result, keepIndexes);
		}
	}
	return XpathData(result);
}

// Predicate
Predicate::Predicate(const XpathExpr* e) : UnaryExpr(e) {}

XpathData
Predicate::eval(const XpathData& d, size_t pos, bool firstStep) const {
	return _e->eval(d, pos);
}

// Descendant
Descendant::Descendant(const Path* path) : _path(path) {
}

XpathData
Descendant::eval(const XpathData& d, size_t pos, bool firstStep) const {
	const std::list<const XpathExpr*>& es = _path->getExprs();
	std::list<const XpathExpr*>::const_iterator i = es.begin();
	const Step* s = static_cast<const Step*>(*i);
	const std::string& stepName = s->getString();
	std::vector<Node> searchResult;
	if (stepName == "*") {
		for (const Node& n : d.getNodeSet()) {
			n.getSubTreeNodes(searchResult);
		}
		i++;
	} else if (stepName == "." || stepName == "..") {
		for (const Node& n : d.getNodeSet()) {
			n.getSubTreeNodes(searchResult);
		}
	} else {
		for (const Node& n : d.getNodeSet()) {
			n.search(stepName, searchResult);
		}
		i++;
	}
	XpathData result(searchResult);
	for (;i != es.end(); ++i) {
		result = (*i)->eval(result, pos);
	}
	return result;
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
		checkArgs(1);			// TODO support 0 args
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

