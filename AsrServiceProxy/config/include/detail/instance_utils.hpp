
#ifndef CONFIG_INSTANCE_UTILS_HPP
#define CONFIG_INSTANCE_UTILS_HPP

#include "config_types.hpp"

namespace config {

/// _return whether given object is of a specified type
template <typename _t, typename _o>
bool instanceof(const _o& object) {
    return !!std::dynamic_pointer_cast<_t>(object);
}

/// _return whether given variant is of a specified type
template <typename _t>
typename std::enable_if<std::is_scalar<_t>::value, bool>::type instanceof(
    const ConfigVariant& var) {
    return boost::get<_t>(&var);
}

/// _return whether given variant is of a specified type
template <typename _t>
typename std::enable_if<std::is_class<_t>::value, bool>::type instanceof(
    const ConfigVariant& var) {
    if (var.type() == typeid(ConfigBasePtr)) {
        auto obj = boost::get<ConfigBasePtr>(var);
        return !obj || instanceof<_t>(obj);
    }

    return boost::get<_t>(&var);
}

}

#endif // CONFIG_INSTANCE_UTILS_HPP
