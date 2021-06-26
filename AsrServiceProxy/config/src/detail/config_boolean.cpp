
#include "detail/config_boolean.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigBoolean::ConfigBoolean(const ConfigOriginPtr& origin, bool value) :
    AbstractConfigValue(origin),
    value(value) {
}

ConfigValueType ConfigBoolean::value_type() {
    return ConfigValueType::BOOLEAN;
}

ConfigVariant ConfigBoolean::unwrapped() {
    return value;
}

std::string ConfigBoolean::transform_to_string() {
    return value ? "true" : "false";
}

AbstractConfigValuePtr ConfigBoolean::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin, value);
}

}
