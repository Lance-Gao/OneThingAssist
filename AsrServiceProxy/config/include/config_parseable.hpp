
#ifndef CONFIG_CONFIG_PARSEABLE_HPP
#define CONFIG_CONFIG_PARSEABLE_HPP

#include "config_types.hpp"

namespace config {

///
/// _an opaque handle to something that can be parsed, obtained from
/// {@link ConfigIncludeContext}.
///
/// <p>
/// <em>_do not implement this interface</em>; it should only be implemented by
/// the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class ConfigParseable {
public:
    /// _parse whatever it is. _the options should come from
    /// {@link ConfigParseable#options options()} but you could tweak them if
    /// you like.
    ///
    /// @param options
    ///            parse options, should be based on the ones from
    ///            {@link ConfigParseable#options options()}
    virtual ConfigObjectPtr parse(const ConfigParseOptionsPtr& options) = 0;

    /// _returns a {@link ConfigOrigin} describing the origin of the parseable
    /// item.
    virtual ConfigOriginPtr origin() = 0;

    /// _get the initial options, which can be modified then passed to parse().
    /// _these options will have the right description, includer, and other
    /// parameters already set up.
    virtual ConfigParseOptionsPtr options() = 0;
};

}

#endif // CONFIG_CONFIG_PARSEABLE_HPP
