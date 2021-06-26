
#ifndef CONFIG_CONFIG_MERGEABLE_HPP
#define CONFIG_CONFIG_MERGEABLE_HPP

#include "config_types.hpp"

namespace config {

///
/// _marker for types whose instances can be merged, that is {@link Config} and
/// {@link ConfigValue}. _instances of {@code Config} and {@code ConfigValue} can
/// be combined into a single new instance using the
/// {@link ConfigMergeable#with_fallback with_fallback()} method.
///
/// <p>
/// <em>_do not implement this interface</em>; it should only be implemented by
/// the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class ConfigMergeable {
public:
    /// _returns a new value computed by merging this value with another, with
    /// keys in this value "winning" over the other one. _only
    /// {@link ConfigObject} and {@link Config} instances do anything in this
    /// method (they need to merge the fallback keys into themselves). _all other
    /// values just return the original value, since they automatically override
    /// any fallback.
    ///
    /// <p>
    /// _the semantics of merging are described in the <a
    /// href="https://github.com/typesafehub/config/blob/master/HOCON.md">spec
    /// for HOCON</a>.
    ///
    /// <p>
    /// _note that objects do not merge "across" non-objects; if you write
    /// <code>object.with_fallback(non_object).with_fallback(other_object)</code>,
    /// then <code>other_object</code> will simply be ignored. _this is an
    /// intentional part of how merging works. _both non-objects, and any object
    /// which has fallen back to a non-object, block subsequent fallbacks.
    ///
    /// @param other
    ///            an object whose keys should be used if the keys are not
    ///            present in this one
    /// @return a new object (or the original one, if the fallback doesn't get
    ///         used)
    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) = 0;
};

}

#endif // CONFIG_CONFIG_MERGEABLE_HPP
