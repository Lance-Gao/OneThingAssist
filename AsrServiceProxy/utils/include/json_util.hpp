#ifndef UTILS_JSON_UTIL_HPP
#define UTILS_JSON_UTIL_HPP

#include "aip_common.hpp"

class JsonUtils {
public:
    static bool load_json(const std::string& raw, Json::Value& json, std::string& msg);
    static bool extract_services(const std::string& raw, std::set<std::string>* services);

    // parse struct to JSON::Value
    static std::string parse_to_string(Json::Value& root, bool styledWirter);
};

#endif // UTILS_JSON_UTIL_HPP
