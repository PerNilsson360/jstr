#include <sstream>
#include <iostream>
#include <stdexcept>

#include "Env.hh"

Env::Env(const Value& context) : _context(context) {
    if (context.getType() != Value::NodeSet) {
        throw std::runtime_error("Env::Env context is not a NodeSet");
    }
}

const Value&
Env::getCurrent() const {
    return _context;
}

void
Env::addVariable(const std::string& name, const Value& v) {
    if (!_vals.emplace(name, v).second) {
        std::stringstream ss;
        ss << "Env::addVariable " << name << " is allready added.";
        throw std::runtime_error(ss.str());
    }
}

const Value&
Env::getVariable(const std::string& name) const {
    std::map<std::string, Value>::const_iterator i = _vals.find(name);
    if (i == _vals.end()) {
        std::stringstream ss;
        ss << "Env::getVariable can not find: " << name;
        throw std::runtime_error(ss.str());
    }
    return i->second;
}
