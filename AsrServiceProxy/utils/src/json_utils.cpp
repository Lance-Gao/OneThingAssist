#include "include/aip_common.hpp"
#include "include/json_util.hpp"
#include "include/aip_log.hpp"

bool JsonUtils::load_json(const std::string& raw, Json::Value& json, std::string& msg) {
    bool ok = false;

    Json::Reader reader;
    ok = reader.parse(raw, json);
    if (!ok) { // failed parse the json string
        msg = reader.getFormattedErrorMessages();
    }
    return ok;
}

std::string JsonUtils::parse_to_string(Json::Value& root, bool styledWriter) {
    if (styledWriter) {
        Json::StyledWriter writer;
        return writer.write(root);
    } else {
        Json::FastWriter writer;
        return writer.write(root);
    }
}

bool JsonUtils::extract_services(const std::string& raw, std::set<std::string>* services) {
    bool ok = false;
    Json::Reader reader;
    Json::Value json;
    ok = reader.parse(raw, json);
    if (!ok) {
        printf("failed to parse the json, error: %s\n",
                reader.getFormattedErrorMessages().c_str());
        return false;
    }
    Json::Value service_list = json["services"];
    for (unsigned int i = 0; i < service_list.size(); ++i) {
        services->insert(service_list[i]["name"].asString());
    }
    return ok;
}

