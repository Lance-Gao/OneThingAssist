
#include "detail/config_int.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigInt::ConfigInt(const ConfigOriginPtr& origin, int32_t value,
                         const std::string& original_text) :
    ConfigNumber(origin, original_text),
    value(value) {
}

ConfigValueType ConfigInt::value_type() {
    return ConfigValueType::NUMBER;
}

ConfigVariant ConfigInt::unwrapped() {
    return value;
}

std::string ConfigInt::transform_to_string() {
    std::string s = ConfigNumber::transform_to_string();

    if (s.empty()) {
        return boost::lexical_cast<std::string>(value);
    } else {
        return s;
    }
}

int64_t ConfigInt::int64_value() {
    return static_cast<int64_t>(value);
}

double ConfigInt::double_value() {
    return static_cast<double>(value);
}

AbstractConfigValuePtr ConfigInt::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin, value, original_text);
}

}
