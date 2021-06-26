
#ifndef CONFIG_CONFIG_INCLUDE_CONTEXT_HPP
#define CONFIG_CONFIG_INCLUDE_CONTEXT_HPP

#include "config_types.hpp"

namespace config {

///
/// _context provided to a {@link ConfigIncluder}; this interface is only useful
/// inside a {@code ConfigIncluder} implementation, and is not intended for apps
/// to implement.
///
/// <p>
/// <em>_do not implement this interface</em>; it should only be implemented by
/// the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class ConfigIncludeContext {
public:
    /// _tries to find a name relative to whatever is doing the including, for
    /// example in the same directory as the file doing the including. _returns
    /// null if it can't meaningfully create a relative name. _the returned
    /// parseable may not exist; this function is not required to do any _i_o, just
    /// compute what the name would be.
    ///
    /// _the passed-in filename has to be a complete name (with extension), not
    /// just a basename. (_include statements in config files are allowed to give
    /// just a basename.)
    ///
    /// @param filename
    ///            the name to make relative to the resource doing the including
    /// @return parseable item relative to the resource doing the including, or
    ///         null
    virtual ConfigParseablePtr relative_to(const std::string& filename) = 0;

    /// _parse options to use (if you use another method to get a
    /// {@link ConfigParseable} then use {@link ConfigParseable#options()}
    /// instead though).
    ///
    /// @return the parse options
    virtual ConfigParseOptionsPtr parse_options() = 0;
};

}

#endif // CONFIG_CONFIG_INCLUDE_CONTEXT_HPP
