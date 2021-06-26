
#ifndef CONFIG_CONFIG_INCLUDER_FILE_HPP
#define CONFIG_CONFIG_INCLUDER_FILE_HPP

#include "config_types.hpp"

namespace config {

///
/// _implement this <em>in addition to</em> {@link ConfigIncluder} if you want to
/// support inclusion of files with the {@code include file("filename")} syntax.
/// _if you do not implement this but do implement {@link ConfigIncluder},
/// attempts to load files will use the default includer.
///
class ConfigIncluderFile {
public:
    /// _parses another item to be included. _the returned object typically would
    /// not have substitutions resolved. _you can throw a ConfigException here to
    /// abort parsing, or return an empty object, but may not return null.
    ///
    /// @param context
    ///            some info about the include context
    /// @param what
    ///            the include statement's argument
    /// @return a non-null ConfigObject
    virtual ConfigObjectPtr include_file(const ConfigIncludeContextPtr& context,
                                            const std::string& file) = 0;
};

}

#endif // CONFIG_CONFIG_INCLUDER_FILE_HPP
