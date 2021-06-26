
#ifndef CONFIG_CONFIG_RESOLVE_OPTIONS_HPP
#define CONFIG_CONFIG_RESOLVE_OPTIONS_HPP

#include "detail/config_base.hpp"

namespace config {

///
/// _a set of options related to resolving substitutions. _substitutions use the
/// <code>${foo.bar}</code> syntax and are documented in the <a
/// href="https://github.com/typesafehub/config/blob/master/HOCON.md">HOCON</a>
/// spec.
/// <p>
/// _this object is immutable, so the "setters" return a new object.
/// <p>
/// _here is an example of creating a custom {@code ConfigResolveOptions}:
///
/// <pre>
///     auto options = ConfigResolveOptions::defaults()
///         ->set_use_system_environment(false)
/// </pre>
/// <p>
/// _in addition to {@link ConfigResolveOptions#defaults}, there's a prebuilt
/// {@link ConfigResolveOptions#no_system} which avoids looking at any system
/// environment variables or other external system information. (_right now,
/// environment variables are the only example.)
///
class ConfigResolveOptions : public ConfigBase {
public:
    CONFIG_CLASS(ConfigResolveOptions);

    ConfigResolveOptions(bool use_system_environment);

    /// _returns the default resolve options.
    ///
    /// @return the default resolve options
    static ConfigResolveOptionsPtr defaults();

    /// _returns resolve options that disable any reference to "system" data
    /// (currently, this means environment variables).
    ///
    /// @return the resolve options with env variables disabled
    static ConfigResolveOptionsPtr no_system();

    /// _returns options with use of environment variables set to the given value.
    ///
    /// @param value
    ///            true to resolve substitutions falling back to environment
    ///            variables.
    /// @return options with requested setting for use of environment variables
    ConfigResolveOptionsPtr set_use_system_environment(bool value);

    /// _returns whether the options enable use of system environment variables.
    /// _this method is mostly used by the config lib internally, not by
    /// applications.
    ///
    /// @return true if environment variables should be used
    bool get_use_system_environment();

private:
    bool use_system_environment;
};

}

#endif // CONFIG_CONFIG_RESOLVE_OPTIONS_HPP
