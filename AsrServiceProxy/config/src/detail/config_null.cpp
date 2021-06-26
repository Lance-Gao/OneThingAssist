
#include "detail/config_null.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigNull::ConfigNull(const ConfigOriginPtr& origin) :
    AbstractConfigValue(origin) {
}

ConfigValueType ConfigNull::value_type() {
    return ConfigValueType::NONE;
}

ConfigVariant ConfigNull::unwrapped() {
    return null();
}

std::string ConfigNull::transform_to_string() {
    return "null";
}

AbstractConfigValuePtr ConfigNull::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin);
}

}
