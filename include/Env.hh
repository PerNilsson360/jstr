#ifndef _ENV_HH_
#define _ENV_HH_

#include <map>
#include "Value.hh"

class Env {
public:
    Env(const Value& context);
    const Value& getCurrent() const;
    void addVariable(const std::string& name, const Value& v);
    const Value& getVariable(const std::string& name) const;
private:
    std::map<std::string, Value> _vals;
    Value _context;
};

#endif
