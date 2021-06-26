
#include "detail/config_double.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigDouble::ConfigDouble(const ConfigOriginPtr& origin, double value,
                               const std::string& original_text) :
    ConfigNumber(origin, original_text),
    value(value) {
}

ConfigValueType ConfigDouble::value_type() {
    return ConfigValueType::NUMBER;
}

ConfigVariant ConfigDouble::unwrapped() {
    return value;
}

std::string ConfigDouble::transform_to_string() {
    std::string s = ConfigNumber::transform_to_string();

    if (s.empty()) {
        return boost::lexical_cast<std::string>(value);
    } else {
        return s;
    }
}

int64_t ConfigDouble::int64_value() {
    return static_cast<int64_t>(value);
}

double ConfigDouble::double_value() {
    return value;
}

AbstractConfigValuePtr ConfigDouble::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin, value, original_text);
}

}
