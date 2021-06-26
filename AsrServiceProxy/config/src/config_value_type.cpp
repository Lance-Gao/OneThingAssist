
#include "config_value_type.hpp"

namespace config {

std::string ConfigValueTypeEnum::name(ConfigValueType value_type) {
    typedef std::map<ConfigValueType, std::string> ConfigValueType_name;
    static ConfigValueType_name names = {
        {ConfigValueType::OBJECT, "OBJECT"},
        {ConfigValueType::LIST, "LIST"},
        {ConfigValueType::NUMBER, "NUMBER"},
        {ConfigValueType::BOOLEAN, "BOOLEAN"},
        {ConfigValueType::NONE, "NONE"},
        {ConfigValueType::STRING, "STRING"}
    };
    return names[value_type];
}

}
