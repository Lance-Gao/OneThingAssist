
#ifndef CONFIG_CONFIG_PARSE_OPTIONS_HPP
#define CONFIG_CONFIG_PARSE_OPTIONS_HPP

#include "detail/config_base.hpp"

namespace config {

///
/// _a set of options related to parsing.
///
/// <p>
/// _this object is immutable, so the "setters" return a new object.
///
/// <p>
/// _here is an example of creating a custom {@code ConfigParseOptions}:
///
/// <pre>
///     auto options = ConfigParseOptions::defaults()
///         ->set_syntax(ConfigSyntax::JSON)
///         ->set_allow_missing(false)
/// </pre>
///
class ConfigParseOptions : public ConfigBase {
public:
    CONFIG_CLASS(ConfigParseOptions);

    ConfigParseOptions(ConfigSyntax syntax,
                          const std::string& origin_description,
                          bool allow_missing,
                          const ConfigIncluderPtr& includer);

    static ConfigParseOptionsPtr defaults();

    /// _set the file format. _if set to null, try to guess from any available
    /// filename extension; if guessing fails, assume {@link ConfigSyntax#CONF}.
    ///
    /// @param syntax
    ///            a syntax or {@code ConfigSyntax::NONE} for best guess
    /// @return options with the syntax set
    ConfigParseOptionsPtr set_syntax(ConfigSyntax syntax);

    ConfigSyntax get_syntax();

    /// _set a description for the thing being parsed. _in most cases this will be
    /// set up for you to something like the filename, but if you provide just an
    /// input stream you might want to improve on it. _set to null to allow the
    /// library to come up with something automatically. _this description is the
    /// basis for the {@link ConfigOrigin} of the parsed values.
    ///
    /// @param origin_description
    /// @return options with the origin description set
    ConfigParseOptionsPtr set_origin_description(const std::string& origin_description);

    std::string get_origin_description();

private:
    ConfigParseOptionsPtr with_fallback_origin_description(const std::string& origin_description);

public:
    /// _set to false to throw an exception if the item being parsed (for example
    /// a file) is missing. _set to true to just return an empty document in that
    /// case.
    ///
    /// @param allow_missing
    /// @return options with the "allow missing" flag set
    ConfigParseOptionsPtr set_allow_missing(bool allow_missing);

    bool get_allow_missing();

    /// _set a ConfigIncluder which customizes how includes are handled.
    ///
    /// @param includer
    /// @return new version of the parse options with different includer
    ConfigParseOptionsPtr set_includer(const ConfigIncluderPtr& includer);

    ConfigParseOptionsPtr prepend_includer(const ConfigIncluderPtr& includer);
    ConfigParseOptionsPtr append_includer(const ConfigIncluderPtr& includer);

    ConfigIncluderPtr get_includer();

private:
    ConfigSyntax _syntax;
    std::string _origin_description;
    bool _allow_missing;
    ConfigIncluderPtr _includer;
};

}

#endif // CONFIG_CONFIG_PARSE_OPTIONS_HPP
