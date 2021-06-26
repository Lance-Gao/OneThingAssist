
#ifndef CONFIG_CONFIG_VALUE_TYPE_HPP
#define CONFIG_CONFIG_VALUE_TYPE_HPP

#include "config_types.hpp"

namespace config {

///
/// _the type of a configuration value (following the <a
/// href="http://json.org">JSON</a> type schema).
///
enum class ConfigValueType : public uint32_t {
    OBJECT, LIST, NUMBER, BOOLEAN, NONE, STRING
};

class ConfigValueTypeEnum {
public:
    static std::string name(ConfigValueType value_type);
};

}

#endif // CONFIG_CONFIG_VALUE_TYPE_HPP
