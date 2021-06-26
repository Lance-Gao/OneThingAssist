
#ifndef CONFIG_CONFIG_UTIL_HPP
#define CONFIG_CONFIG_UTIL_HPP

#include "config_types.hpp"

namespace config {

///
/// _contains static utility methods.
///
class ConfigUtil {
private:
    ConfigUtil();

public:
    /// _quotes and escapes a string, as in the JSON specification.
    ///
    /// @param s
    ///            a string
    /// @return the string quoted and escaped
    static std::string quote_string(const std::string& s);

    /// _converts a list of strings to a path expression, by quoting the path
    /// elements as needed and then joining them separated by a period. _a path
    /// expression is usable with a {@link Config}, while individual path
    /// elements are usable with a {@link ConfigObject}.
    ///
    /// @param elements
    ///            the keys in the path
    /// @return a path expression
    /// @throws ConfigException
    ///             if the list is empty
    static std::string join_path(const VectorString& elements = VectorString());

    /// _converts a path expression into a list of keys, by splitting on period
    /// and unquoting the individual path elements. _a path expression is usable
    /// with a {@link Config}, while individual path elements are usable with a
    /// {@link ConfigObject}.
    ///
    /// @param path
    ///            a path expression
    /// @return the individual keys in the path
    /// @throws ConfigException
    ///             if the path expression is invalid
    static VectorString split_path(const std::string& path);
};

}

#endif // CONFIG_CONFIG_UTIL_HPP
