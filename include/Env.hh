#ifndef _ENV_HH_
#define _ENV_HH_

#include <map>
#include "Value.hh"

class Env {
public:
    /**
     * Creates an environment with a context node supplied as a json object.
     * The json object is a "top level" entity in the data tree.
     * @param json a top level json entity.
     */
    Env(const nlohmann::json& json);
    /**
     * Creates an environment with a context node. The context node must be either
     * a primitivite value (number, boolean or string) or a node set with one node.
     * @param context the current context of the environment.
     */
    Env(const Value& context);
    const Value& getCurrent() const;
    void addVariable(const std::string& name, const Value& v);
    const Value& getVariable(const std::string& name) const;
private:
    std::map<std::string, Value> _vals;
    Value _context;
};

#endif
