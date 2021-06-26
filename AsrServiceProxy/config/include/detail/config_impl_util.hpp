
#ifndef CONFIG_CONFIG_IMPL_UTIL_HPP
#define CONFIG_CONFIG_IMPL_UTIL_HPP

#include "config_types.hpp"

namespace config {

class ConfigImplUtil {
public:
    static bool equals_handling_null(const ConfigVariant& a, const ConfigVariant& b);

    /// _this is public ONLY for use by the "config" package, DO NOT USE this ABI
    /// may change.
    static std::string render_json_string(const std::string& s);

    static std::string render_string_unquoted_if_possible(const std::string& s);

    /// _this is public ONLY for use by the "config" package, DO NOT USE this ABI
    /// may change. _you can use the version in ConfigUtil instead.
    static std::string join_path(const VectorString& elements = VectorString());

    /// _this is public ONLY for use by the "config" package, DO NOT USE this ABI
    /// may change. _you can use the version in ConfigUtil instead.
    static VectorString split_path(const std::string& path);
};

}

#endif // CONFIG_CONFIG_IMPL_UTIL_HPP
