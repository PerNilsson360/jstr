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
#include <stdexcept>
#include <Jstr.hh>

#include "xpath10_driver.hh"

namespace Jstr {    
namespace Xpath {
    
Value
eval(const std::string& xpath, const nlohmann::json& json) {
    xpath10_driver driver;
    if (driver.parse(xpath) != 0) {
        std::stringstream ss;
        ss << "nljp::eval failed to parse exp: " << xpath;
        throw std::runtime_error(ss.str());
    }
    Value context(Node("", json));
    Env env(context);
    return driver.result->eval(env, context, 0);
}
    
}

namespace {

std::string
getPropertyString(const nlohmann::json& json,
                  const std::string& name,
                  const std::string& messagePrefix) {
    if (!json.contains(name)) {
        std::stringstream ss;
        ss << messagePrefix << " can not find " << name; 
        throw std::runtime_error(ss.str());
    }
    const nlohmann::json& t = json[name];
    if (!t.is_string()) {
        std::stringstream ss;
        ss << messagePrefix << " " << name << " is not a string";
        throw std::runtime_error(ss.str());
    }
    return t.get<std::string>();
}

bool
evalExpression(const Expression& expr,
               const Env& env,
               const std::string& name,
               const std::string& message,
               std::ostream& out) {
    Value r = expr.eval(env);
    bool result = r.getBoolean();
    if (!result) {
        out << name << ", error: " << message << std::endl;
    }
    return result;
}
    
bool
evalAssert(const std::string& name,
           const Value& context,
           const nlohmann::json& assert,
           const nlohmann::json& data,
           std::ostream& out) {
    std::string test = getPropertyString(assert, "test", "schematron::evalAssert");
    std::string message = getPropertyString(assert, "message", "schematron::evalAssert");
    Expression expr(test);
    bool result(true);
    if (context.getType() == Value::NodeSet) {
        for (const Node& n : context.getNodeSet()) {
            Value v(n);
            Env env(v);
            result &= evalExpression(expr, env, name, message, out);
        }
    } else {
        Env env(context);
        result = evalExpression(expr, env, name, message, out);
    }
    return result;
}
   
bool
evalRule(const std::string& name,
         const nlohmann::json& rule,
         const nlohmann::json& data,
         std::ostream& out) {
    const std::string& ctx = getPropertyString(rule, "context", "schematron::evalRule");
    Env env(data);
    Expression expr(ctx);
    Value context(expr.eval(env));
    if (!rule.contains("assert")) {
        throw std::runtime_error("schematron::evalRule can not find assert");
    }
    const nlohmann::json& assert = rule["assert"];
    if (assert.is_object()) {
        return evalAssert(name, context, assert, data, out);
    } else if (assert.is_array()) {
        size_t size = assert.size();
        if (size == 0) {
            throw std::runtime_error("schematron::evalRule assert array is empty");
        }
        bool result(true);
        for (size_t i = 0; i < size; i++) {
            const nlohmann::json& a = assert[i];
            if (!a.is_object()) {
                throw std::runtime_error("schematron::evalRule assert in array is not object");
            }
            result &= evalAssert(name, context, a, data, out);
        }
        return result;
    } else {
        throw std::runtime_error("schematron::evalPattern rule is not object or array");
    }
}
    
bool
evalPattern(const nlohmann::json& pattern, const nlohmann::json& data, std::ostream& out) {
    if (!pattern.contains("name")) {
        throw std::runtime_error("schematron::evalPattern can not find name");
    }
    const nlohmann::json& n = pattern["name"];
    if (!n.is_string()) {
        throw std::runtime_error("schematron::evalPattern name is not a string");
    }
    const std::string& name = n.get<std::string>();
    if (!pattern.contains("rule")) {
        throw std::runtime_error("schematron::evalPattern can not find any rules");
    }
    const nlohmann::json& rule = pattern["rule"];
    if (rule.is_object()) {
        return evalRule(name, rule, data, out);
    } else if (rule.is_array()) {
        size_t size = rule.size();
        if (size == 0) {
            throw std::runtime_error("schematron::evalPattern rule array is empty");
        }
        bool result(true);
        for (size_t i = 0; i < size; i++) {
            const nlohmann::json& r = rule[i];
            if (!r.is_object()) {
                throw std::runtime_error("schematron::evalPattern rule in array is not object");
            }
            result &= evalRule(name, r, data, out);
        }
        return result;
    } else {
        throw std::runtime_error("schematron::evalPattern rule is not object or array");
    }
}
    
}
    
namespace Schematron {
    
bool
eval(const nlohmann::json& schematron, const nlohmann::json& data, std::ostream& out) {
    if (!schematron.contains("pattern")) {
        throw std::runtime_error("schematron::eval can not find any patterns");
    }
    const nlohmann::json& pattern  = schematron["pattern"];
    if (pattern.is_object()) {
        return evalPattern(pattern, data, out);
    } else if (pattern.is_array()) {
        size_t size = pattern.size();
        if (size == 0) {
            throw std::runtime_error("schematron::eval pattern array is empty");
        }
        bool result(true);
        for (size_t i = 0; i < size; i++) {
            const nlohmann::json& p = pattern[i];
            if (!p.is_object()) {
                throw std::runtime_error("schematron::eval pattern in array is not object");
            }
            result &= evalPattern(p, data, out);
        }
        return result;
    } else {
        throw std::runtime_error("schematron::eval pattern is not object or array");
    }
    return true;
}

}
    
}
