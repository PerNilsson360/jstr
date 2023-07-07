#ifndef _NLJP_HH_
#define _NLJP_HH_

#include <nlohmann/json.hpp>
#include "XpathData.hh"

XpathData
eval(const std::string& xpath, const nlohmann::json& json);

#endif
