
#include "detail/config_int64.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigInt64::ConfigInt64(const ConfigOriginPtr& origin, int64_t value,
                             const std::string& original_text) :
    ConfigNumber(origin, original_text),
    value(value) {
}

ConfigValueType ConfigInt64::value_type() {
    return ConfigValueType::NUMBER;
}

ConfigVariant ConfigInt64::unwrapped() {
    return value;
}

std::string ConfigInt64::transform_to_string() {
    std::string s = ConfigNumber::transform_to_string();

    if (s.empty()) {
        return boost::lexical_cast<std::string>(value);
    } else {
        return s;
    }
}

int64_t ConfigInt64::int64_value() {
    return value;
}

double ConfigInt64::double_value() {
    return static_cast<double>(value);
}

AbstractConfigValuePtr ConfigInt64::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin, value, original_text);
}

}
