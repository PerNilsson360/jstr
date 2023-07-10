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

#include <sstream>
#include <iostream>
#include <stdexcept>
#include "XpathData.hh"

XpathData::XpathData() : _type(NodeSet) {
	_d.ns = new std::vector<Node>();
}

XpathData::XpathData(const XpathData& xd) {
	assign(xd);
}

XpathData::XpathData(double n) : _type(Number){
	_d.n = n;
}

XpathData::XpathData(bool b) : _type(Bool) {
	_d.b = b;
}

XpathData::XpathData(const std::string& s) : _type(String) {
	_d.s = new std::string(s);
}

XpathData::XpathData(const std::string& name, const nlohmann::json& json) :
	_type(NodeSet) {
	_d.ns = new std::vector<Node>();
	if (json.is_array()) {
		for (size_t i = 0, size = json.size(); i < size; i++) {
			_d.ns->emplace_back(Node(name, json, i));
		}
	} else {
		_d.ns->emplace_back(Node(name, json));
	}
}

XpathData::XpathData(const Node& node) : _type(NodeSet) {
	_d.ns = new std::vector<Node>();
	_d.ns->emplace_back(Node(node));
}

XpathData::XpathData(const std::vector<Node>& ns) : _type(NodeSet) {
	_d.ns = new std::vector<Node>(ns);
}

XpathData&
XpathData::operator=(const XpathData& xd) {
	assign(xd);
	return *this;
}

XpathData::~XpathData() {
	clear();
}

XpathData::Type
XpathData::getType() const {
	return _type;
}

bool XpathData::isValue() const {
	return _type != NodeSet || (_d.ns->size() == 1 && (*_d.ns)[0].isValue());
}

double
XpathData::getNumber() const {
	switch (_type) {
	case Number: return _d.n;
	case Bool: return _d.b;
	case String: {
		if (_d.s->empty()) {
			return NAN;
		} else {
			try {
				std::stod(*_d.s);
			} catch (const std::exception& e) {
				return NAN;
			}
		}
	}
	case NodeSet: {
		const std::string& s = getString();
		return s.empty() ? NAN : std::stod(s);
	}
	default:
		throw std::runtime_error("XpathData::getNumber(): unkown type");
	}
}

bool
XpathData::getBool() const {
	switch(_type) {
	case Number: return _d.n;
	case Bool: return _d.b;
	case String: return !_d.s->empty();
	case NodeSet: return !_d.ns->empty();
	default:
		throw std::runtime_error("XpathData::getBool(): unkown type");
	}
}

std::string
XpathData::getStringValue() const {
	switch (_type) {
	case Number:
	case Bool:
	case String: return getString();
	case NodeSet: {
		std::string r;
		for (const Node& n :  *_d.ns) {
			r += n.getString();	// TODO: r should be input parameter
		}
		return r;
	}
	default:
		throw std::runtime_error("XpathData::getString(): unkown type");
	}
}

std::string
XpathData::getString() const {
	switch (_type) {
	case Number: {
		if (std::isnan(_d.n)) {
			return "NaN";
		} else {
			std::to_string(_d.n);
		}
	}
	case Bool: return _d.b ? "true" : "false";
	case String: return *(_d.s);
	case NodeSet:  return _d.ns->empty() ? "" : _d.ns->begin()->getString();
	default:
		throw std::runtime_error("XpathData::getString(): unkown type");
	}
}

const std::vector<Node>&
XpathData::getNodeSet() const {
	return *_d.ns;
}

XpathData
XpathData::getNodeSetSize() const {
	switch(_type) {
	case Number:
	case Bool:
	case String: return XpathData(static_cast<double>(1));
	case NodeSet: return XpathData(static_cast<double>(_d.ns->size()));
	default:
		throw std::runtime_error("XpathData::getNodeSetSize(): unkown type");
	}
}

XpathData
XpathData::getLocalName() const {
	if (_type == NodeSet && !_d.ns->empty()) {
		return XpathData(_d.ns->begin()->getLocalName());
	} else {
		return XpathData("");
	}
}

XpathData
XpathData::getRoot() const {
	if (_type != NodeSet) {
		std::stringstream ss;
		ss << "XpathData::getRoot() node set is not NodeSet: " << _type << std::endl;
		throw std::runtime_error(ss.str());
	}
	if (_d.ns->empty()) {
		throw std::runtime_error("XpathData::getRoot() node set is empty");
	}
	return XpathData(_d.ns->begin()->getRoot());
}

bool
XpathData::operator==(const XpathData& xd) const {
	if (_type == NodeSet && xd._type == NodeSet) {
		for (const Node& l : getNodeSet()) {
			if (xd == l.getString()) {
				return true;
			}
		}
		return false;
	} else if (_type == NodeSet) {
		switch (xd._type) {
		case Number: return *this == xd.getNumber();
		case Bool: return *this == xd.getBool();
		case String: return *this == xd.getString();
		default: throw std::runtime_error("XpathData::operator==, unkown type _type==NodeSet");
		}
	} else if (xd._type == NodeSet){
		switch (_type) {
		case Number: return xd == getNumber();
		case Bool: return xd == getBool();
		case String: return xd == getString();
		default: throw std::runtime_error("XpathData::operator==, unkown type xd._type==NodeSet");
		}
	} else if (_type == Bool || xd._type == Bool) {
		return getBool() == xd.getBool();
	} else if (_type == Number || xd._type == Bool) {
		return getNumber() == xd.getNumber();
	} else {
		return getString() == xd.getString();
	}
}

bool
XpathData::operator!=(const XpathData& xd) const {
	if (_type == NodeSet && xd._type == NodeSet) {
		for (const Node& l : getNodeSet()) {
			if (xd != l.getString()) {
				return true;
			}
		}
		return false;
	} else if (_type == NodeSet) {
		switch (xd._type) {
		case Number: return *this != xd.getNumber();
		case Bool: return *this != xd.getBool();
		case String: return *this != xd.getString();
		default: throw std::runtime_error("XpathData::operator!=, unkown type _type==NodeSet");
		}
	} else if (xd._type == NodeSet){
		switch (_type) {
		case Number: return xd != getNumber();
		case Bool: return xd != getBool();
		case String: return xd != getString();
		default: throw std::runtime_error("XpathData::operator!=, unkown type xd._type==NodeSet");
		}
	} else if (_type == Bool || xd._type == Bool) {
		return getBool() != xd.getBool();
	} else if (_type == Number || xd._type == Number) {
		return getNumber() != xd.getNumber();
	} else {
		return getString() != xd.getString();
	}
}

bool XpathData::operator<(const XpathData& xd) const {
	checkOrderingRelationArgs(xd);
	return getNumber() < xd.getNumber();
}

bool XpathData::operator<=(const XpathData& xd) const {
	checkOrderingRelationArgs(xd);
	return getNumber() <= xd.getNumber();
}

bool XpathData::operator>(const XpathData& xd) const {
	checkOrderingRelationArgs(xd);
	return getNumber() > xd.getNumber();
}

bool XpathData::operator>=(const XpathData& xd) const {
	checkOrderingRelationArgs(xd);
	return getNumber() >= xd.getNumber();
}

void
XpathData::checkOrderingRelationArgs(const XpathData& xd) const {
	if (!isValue() || !xd.isValue()) {
		std::string m("XpathData::checkOrderingRelationArgs, can not compare node sets");
		throw std::runtime_error(m);
	}
}

void
XpathData::assign(const XpathData& xd) {
	clear();
	_type = xd._type;
	switch(_type) {
	case Number:
		_d.n = xd._d.n;
		break;
	case Bool:
		_d.b = xd._d.b;
		break;
	case String:
		_d.s = new std::string(*xd._d.s);
		break;
	case NodeSet:
		_d.ns = new std::vector<Node>(*xd._d.ns);
		break;
	default:
		throw std::runtime_error("XpathData::assign: unkown type");
	}
}

void
XpathData::clear() {
	switch (_type) {
	case String:
		delete _d.s;
		_d.s = nullptr;
		break;
	case NodeSet:
		delete _d.ns;
		_d.ns = nullptr;
		break;
	default:
		;						// nothing to delete
	}
}
