#include <iostream>
#include <nlohmann/json.hpp>
#include "Node.hh"

Node::Node() : _parent(nullptr), _json(nullptr), _i(-1) {}

Node::Node(const std::string& name, const nlohmann::json& json, int64_t i) :
	_parent(nullptr), _name(name),  _json(&json), _i(i) {
}

Node::Node(const Node* parent, const std::string& name, const nlohmann::json& json, int64_t i) :
	_json(&json), _name(name), _i(i) {
	_parent.reset(parent);
}

Node::Node(const Node& node)  {
	assign(node);
}

Node&
Node::operator=(const Node& node) {
	assign(node);
	return *this;
}

Node
Node::getRoot() const {
	if (_parent == nullptr) {
		return *this;
	} else {
		return _parent->getRoot();
	}
}

const Node*
Node::getParent() const {
	return _parent.get();
}

const nlohmann::json&
Node::getJson() const {
	return _i == -1 ? *_json : (*_json)[_i];
}

bool
Node::isValue() const {
	const nlohmann::json& j = getJson();
	return j.is_primitive();
}

double
Node::getNumber() const {
	const nlohmann::json& j = getJson();
	if (j.is_number()) {
		return j.get<double>();
	} else if (j.is_string()) {
		const std::string& s = j.get<std::string>();
		return s.empty() ? NAN : std::stod(s);
	} else if (j.is_boolean()) {
		return j.get<bool>() ? 1 : 0;
	} else {
		return NAN;
	}
}

bool
Node::getBool() const {
	const nlohmann::json& j = getJson();
	if (j.is_number()) {
		double d = j.get<double>();
		return (d == 0 || d == NAN) ? false : true;
	} else if (j.is_string()) {
		const std::string& s = j.get<std::string>();
		return !s.empty();
	} else if (j.is_boolean()) {
		return j.get<bool>();
	} else {
		return true;			// object or object in an array
	}	
}

namespace {
	
void
getString(const nlohmann::json& json, std::string& r) {
	if (json.is_string()) {
		r += json.get<std::string>(); // Dont want quotation marks you get with dump
	}else if (json.is_primitive()) {
		r += json.dump();		// TODO: check is number NAN "NaN" which is xml syntax
	} else {
		for (const nlohmann::json& j: json) {
			getString(j, r);
		}
	}
}

}

std::string
Node::getString() const {
	std::string r;
	::getString(getJson(), r);
	return r;
}

const std::string&
Node::getLocalName() const {
	return _name;
}

bool
Node::isArrayChild() const {
	return _i != -1;
}

namespace {
void
search(Node parent, const nlohmann::json& j, const std::string& name, std::vector<Node>& result) {
	if (j.is_object()) {
		for (const auto& item : j.items()) {
			const nlohmann::json& value = item.value();
			if (item.key() == name) {
				if (value.is_array()) {
					// the stringValue becomes nicer if you first add array nodes then search
					for (size_t i = 0, size = value.size(); i < size; i++) {
						result.emplace_back(Node(new Node(parent), name, value, i));
					}
					for (size_t i = 0, size = value.size(); i < size; i++) {
						::search(Node(new Node(parent), name, value[i]), value[i], name, result);
					}
				} else {
					result.emplace_back(Node(new Node(parent), name, value));
					::search(Node(new Node(parent), name, value), value, name, result);
				}
			} else {
				::search(Node(new Node(parent), name, value), value, name, result);
			}
		}
	}
}

}

void
Node::getChild(const std::string& name , std::vector<Node>& result) const {
	const nlohmann::json& j = getJson();
	if (j.is_object() && j.contains(name)) {
		const nlohmann::json& child = j[name];
		if (child.is_array()) {
			for (size_t i = 0, size = child.size(); i < size; i++) {
				result.emplace_back(Node(new Node(*this), name, child, i));
			}
		} else {
			result.emplace_back(Node(new Node(*this), name, child));
		}
	}
}

void
Node::getChildren(std::vector<Node>& result) const {
	const nlohmann::json& j = getJson();
	if (j.is_object()) {
		for (const auto& item : j.items()) {
			const nlohmann::json& value = item.value();
			if (value.is_array()) {
				for (size_t i = 0, size = value.size(); i < size; i++) {
					result.emplace_back(Node(new Node(*this), item.key(), value, i));
				}
			} else {
				result.emplace_back(Node(new Node(*this), item.key(), value));
			}
		}
	}
}

void
Node::getSubTreeNodes(std::vector<Node>& result) const {
	const nlohmann::json& j = getJson();
	if (j.is_object()) {
		for (const auto& item : j.items()) {
			const nlohmann::json& value = item.value();
			const std::string& name = item.key();
			if (value.is_array()) {
				std::vector<Node> tmp;
				for (size_t i = 0, size = value.size(); i < size; i++) {
					const Node& n = Node(new Node(*this), name, value, i);
					result.emplace_back(n);
					tmp.emplace_back(n);
				}
				for (const Node& n : tmp) {
					n.getSubTreeNodes(result);
				}
			} else {
				const Node& n = Node(new Node(*this), name, value);
				result.emplace_back(n);
				n.getSubTreeNodes(result);
			}
		}
	}
}

void
Node::search(const std::string& name, std::vector<Node>& result) const {
	::search(*this, getJson(), name, result);
}

bool
Node::operator==(const Node& r) const {
	return _json == r._json;
}

void
Node::assign(const Node& node) {
	_name = node._name;
	_json = node._json;
	_i = node._i;
	_parent.reset(node._parent == nullptr ? nullptr : new Node(*node._parent));
}
