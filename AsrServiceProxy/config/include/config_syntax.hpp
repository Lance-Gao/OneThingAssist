
#ifndef CONFIG_CONFIG_SYNTAX_HPP
#define CONFIG_CONFIG_SYNTAX_HPP

namespace config {

///
/// _the syntax of a character stream, <a href="http://json.org">JSON</a>, <a
/// href="https://github.com/typesafehub/config/blob/master/HOCON.md">HOCON</a>
/// aka ".conf".
///
enum class ConfigSyntax : public uint32_t {
    /// _null value.
    NONE,

    /// _pedantically strict <a href="http://json.org">JSON</a> format; no
    /// comments, no unexpected commas, no duplicate keys in the same object.
    JSON,

    /// _the JSON-superset <a
    /// href="https://github.com/typesafehub/config/blob/master/HOCON.md"
    /// >HOCON</a> format.
    CONF
};

}

#endif // CONFIG_CONFIG_SYNTAX_HPP
