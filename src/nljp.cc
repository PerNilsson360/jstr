#include <sstream>
#include <stdexcept>
#include "nljp.hh"
#include "xpath10_driver.hh"

Value
eval(const std::string& xpath, const nlohmann::json& json) {
    xpath10_driver driver;
    if (driver.parse(xpath) != 0) {
        std::stringstream ss;
        ss << "nljp::eval failed to parse exp: " << xpath;
        throw std::runtime_error(ss.str());
    }
    return driver.result->eval(Value(Node("", json)), 0);
}
