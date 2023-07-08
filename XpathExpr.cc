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
Root::eval(const XpathData& d, size_t pos) const {
  	return d.getRoot();
}

// RelPath
RelPath::RelPath(const XpathExpr* e) : MultiExpr(e) {}

XpathData
RelPath::eval(const XpathData& d, size_t pos) const {
	std::vector<Node> result = d.getNodeSet();
	for (const XpathExpr* e : _exprs) {
		const XpathData& tmp = e->eval(result, pos);
		result = tmp.getNodeSet();
	}
	return XpathData(result);
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

// Step
Step::Step(const std::string& s, const std::list<const XpathExpr*>* preds) :
	StrExpr(s), _preds(preds) {
}

Step::~Step() {
	deleteExprs(_preds);
}

XpathData
Step::eval(const XpathData& d, size_t pos) const {
	std::vector<Node> result;
	const std::vector<Node>& ns = d.getNodeSet();
	if (ns.empty()) {
		return XpathData(result);
	}
	if (_s == "..") {			// TODO avoid string comparison
		for (const Node& n : ns) {
			// TODO implement getParent
			const Node* parent = n.getParent();
			if (parent != nullptr) {
				addIfUnique(result, Node(*parent));
			}
		}
	} else if (_s == ".") {
		result.emplace_back(ns[pos]);
	} else if (_s == "*") {
		for (const Node& n : ns) {
			n.getChildren(result);
		}
	} else {
		for (const Node& n : ns) {
			// TODO implement getchild(name, result) in node
			const nlohmann::json& child = n.getJson();
			if (child.is_object() && child.contains(_s)) {
				const nlohmann::json& j = child[_s];
				if (j.is_array()) {
					for (size_t i = 0, size = j.size(); i < size; i++) {
						result.emplace_back(Node(new Node(n), _s, j, i)); // One item for each array item
					}
				} else {
					result.emplace_back(Node(new Node(n),_s, j));
				}
			}
		}
	}
	// Apply filter
	if (_preds != nullptr) {
		for (const XpathExpr* e : *_preds) {
			std::vector<size_t> keepIndexes;
			for (int i = 0; i < result.size(); i++) {
				XpathData r = e->eval(result, i);
				if (r.getBool()) {
					keepIndexes.push_back(i);
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
Predicate::eval(const XpathData& d, size_t pos) const {
	return _e->eval(d, pos);
}

// Descendant
Descendant::Descendant(const RelPath* relPath) : _relPath(relPath) {
}

XpathData
Descendant::eval(const XpathData& d, size_t pos) const {
	const std::list<const XpathExpr*>& es = _relPath->getExprs();
	std::list<const XpathExpr*>::const_iterator i = es.begin();
	const Step* s = static_cast<const Step*>(*i);
	const std::string stepName = s->getString();
	std::vector<Node> searchResult;
	for (const Node& n : d.getNodeSet()) {
		n.search(stepName, searchResult);
	}
	i++;
	XpathData result(searchResult);
	for (;i != es.end(); ++i) {
		result = (*i)->eval(result, pos);
	}
	return result;
}

// Literal
Literal::Literal(const std::string& l) : StrExpr(l) {}

XpathData
Literal::eval(const XpathData& d, size_t pos) const {
  return XpathData(_s);
}

// Number
Number::Number(double d) : _d(d) {}

XpathData
Number::eval(const XpathData& d, size_t pos) const {
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
Fun::eval(const XpathData& d, size_t pos) const {
	if (_name == "count") {
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
Union::eval(const XpathData& d, size_t pos) const {
  return XpathData();
}

// Or
Or::Or(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Or::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l.getBool() || r.getBool());
}

// And
And::And(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
And::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l.getBool() && r.getBool());
}

// Eq
Eq::Eq(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Eq::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return l == r;
}


// Ne
Ne::Ne(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Ne::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return l != r;
}

// Lt
Lt::Lt(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Lt::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l < r);
}

// Gt
Gt::Gt(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Gt::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l > r);
}

// Le
Le::Le(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Le::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l <= r);
}

// Ge
Ge::Ge(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Ge::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l >= r);

}

// Plus
Plus::Plus(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Plus::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l.getNumber() + r.getNumber());
}

// Minus
Minus::Minus(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Minus::eval(const XpathData& d, size_t pos) const {
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
Mul::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l.getNumber() * r.getNumber());
}

// Div
Div::Div(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Div::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	return XpathData(l.getNumber() / r.getNumber());
}

// Mod
Mod::Mod(const XpathExpr* l, const XpathExpr* r) : BinaryExpr(l, r) {}

XpathData
Mod::eval(const XpathData& d, size_t pos) const {
	XpathData l = _l->eval(d, pos);
	XpathData r = _r->eval(d, pos);
	double result = static_cast<int64_t>(l.getNumber()) % static_cast<int64_t>(r.getNumber());
	return XpathData(result);
}

