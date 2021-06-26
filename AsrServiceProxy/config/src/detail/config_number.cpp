
#include "detail/config_number.hpp"
#include "detail/config_impl_util.hpp"
#include "detail/config_int.hpp"
#include "detail/config_int64.hpp"
#include "detail/config_double.hpp"
#include "config_exception.hpp"

namespace config {

ConfigNumber::ConfigNumber(const ConfigOriginPtr& origin, const std::string& original_text) :
    AbstractConfigValue(origin),
    original_text(original_text) {
}

std::string ConfigNumber::transform_to_string() {
    return original_text;
}

int32_t ConfigNumber::int_value_range_checked(const std::string& path) {
    int64_t l = int64_value();

    if (l < std::numeric_limits<int32_t>::min() || l > std::numeric_limits<int32_t>::max()) {
        throw ConfigExceptionWrongType(origin(), path, "32-bit integer",
                            "out-of-range value " + boost::lexical_cast<std::string>(l));
    }

    return static_cast<int32_t>(l);
}

bool ConfigNumber::is_whole() {
    int64_t as_int64 = int64_value();
    return as_int64 == double_value();
}

bool ConfigNumber::can_equal(const ConfigVariant& other) {
    return instanceof<ConfigNumber>(other);
}

bool ConfigNumber::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<ConfigNumber>(other)) {
        return can_equal(other) &&
               (this->value_type() == dynamic_get<ConfigValue>(other)->value_type()) &&
               ConfigImplUtil::equals_handling_null(this->unwrapped(),
                       dynamic_get<ConfigValue>(other)->unwrapped());
    } else {
        return false;
    }
}

uint32_t ConfigNumber::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    int64_t as_int64;

    if (is_whole()) {
        as_int64 = int64_value();
    } else {
        const uint64_t double_exponent_mask = 0x7ff0000000000000LL;
        const uint64_t double_mantissa_mask = 0x000fffffffffffffLL;
        const uint64_t double_nan_bits = double_exponent_mask |
                0x0008000000000000LL;

        double value = double_value();
        std::memcpy(&as_int64, &value, sizeof(double));

        // replacement for _double.double_to_int64_bits(double_value())
        if ((as_int64 & double_exponent_mask) == double_exponent_mask) {
            if (as_int64 & double_mantissa_mask) {
                as_int64 = double_nan_bits;
            }
        }
    }

    return static_cast<uint32_t>(as_int64 ^ (as_int64 >> 32));
}

ConfigNumberPtr ConfigNumber::new_number(const ConfigOriginPtr& origin, int64_t number,
        const std::string& original_text) {
    if (number <= std::numeric_limits<int32_t>::max()
            && number >= std::numeric_limits<int32_t>::min()) {
        return ConfigInt::make_instance(origin, static_cast<int32_t>(number), original_text);
    } else {
        return ConfigInt64::make_instance(origin, number, original_text);
    }
}

ConfigNumberPtr ConfigNumber::new_number(const ConfigOriginPtr& origin, double number,
        const std::string& original_text) {
    int64_t as_int64 = static_cast<int64_t>(number);

    if (as_int64 == number) {
        return new_number(origin, as_int64, original_text);
    } else {
        return ConfigDouble::make_instance(origin, number, original_text);
    }
}

}
