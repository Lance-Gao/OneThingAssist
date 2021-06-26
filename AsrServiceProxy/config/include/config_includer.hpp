
#ifndef CONFIG_CONFIG_INCLUDER_HPP
#define CONFIG_CONFIG_INCLUDER_HPP

#include "config_types.hpp"

namespace config {

///
/// _implement this interface and provide an instance to
/// {@link ConfigParseOptions#set_includer ConfigParseOptions.set_includer()} to
/// customize handling of {@code include} statements in config files. _you may
/// also want to implement {@link ConfigIncluderFile}.
///
class ConfigIncluder {
public:
    /// _returns a new includer that falls back to the given includer. _this is how
    /// you can obtain the default includer; it will be provided as a fallback.
    /// _it's up to your includer to chain to it if you want to. _you might want to
    /// merge any files found by the fallback includer with any objects you load
    /// yourself.
    ///
    /// _it's important to handle the case where you already have the fallback
    /// with a "return this", i.e. this method should not create a new object if
    /// the fallback is the same one you already have. _the same fallback may be
    /// added repeatedly.
    ///
    /// @param fallback
    /// @return a new includer
    virtual ConfigIncluderPtr with_fallback(const ConfigIncluderPtr& fallback) = 0;

    /// _parses another item to be included. _the returned object typically would
    /// not have substitutions resolved. _you can throw a ConfigException here to
    /// abort parsing, or return an empty object.
    ///
    /// _this method is used for a "heuristic" include statement that does not
    /// specify file resource. _if the include statement does specify, then the
    /// same class implementing {@link ConfigIncluder} must also implement
    /// {@link ConfigIncluderFile} as needed, or a default includer will be used.
    ///
    /// @param context
    ///            some info about the include context
    /// @param what
    ///            the include statement's argument
    /// @return a non-null ConfigObject
    virtual ConfigObjectPtr include(const ConfigIncludeContextPtr& context,
                                       const std::string& what) = 0;
};

}

#endif // CONFIG_CONFIG_INCLUDER_HPP
