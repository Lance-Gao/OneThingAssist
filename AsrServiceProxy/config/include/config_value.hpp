
#ifndef CONFIG_CONFIG_VALUE_HPP
#define CONFIG_CONFIG_VALUE_HPP

#include "detail/config_base.hpp"
#include "detail/variant_utils.hpp"
#include "config_mergeable.hpp"

namespace config {

///
/// _an immutable value, following the <a href="http://json.org">JSON</a> type
/// schema.
///
/// <p>
/// _because this object is immutable, it is safe to use from multiple threads and
/// there's no need for "defensive copies."
///
/// <p>
/// <em>_do not implement {@code ConfigValue}</em>; it should only be implemented
/// by the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
/// _this class also holds some static factory methods for building {@link
///ConfigValue} instances. _see also {@link Config} which has methods for parsing
/// files and certain in-memory data structures.
///
class ConfigValue : public virtual ConfigMergeable {
public:
    /// _the origin of the value (file, line number, etc.), for debugging and
    /// error messages.
    ///
    /// @return where the value came from
    virtual ConfigOriginPtr origin() = 0;

    /// _the {@link ConfigValueType} of the value; matches the JSON type schema.
    ///
    /// @return value's type
    virtual ConfigValueType value_type() = 0;

    /// _returns the value, that is, a {@code std::string}, {@code int32_t},
    /// {@code int64_t}, {@code bool}, {@code MapVariant}, {@code VectorVariant},
    /// or {@code null}, matching the {@link #value_type()} of this
    /// {@code ConfigValue}. _if the value is a {@link ConfigObject} or
    /// {@link ConfigList}, it is recursively unwrapped.
    virtual ConfigVariant unwrapped() = 0;

    /// _alternative to unwrapping the value to a ConfigVariant.
    template <typename _t> _t unwrapped() {
        return variant_get<_t>(unwrapped());
    }

    /// _renders the config value as a HOCON string. _this method is primarily
    /// intended for debugging, so it tries to add helpful comments and
    /// whitespace.
    ///
    /// <p>
    /// _if the config value has not been resolved (see {@link Config#resolve}),
    /// it's possible that it can't be rendered as valid HOCON. _in that case the
    /// rendering should still be useful for debugging but you might not be able
    /// to parse it.
    ///
    /// <p>
    /// _this method is equivalent to
    /// {@code render(ConfigRenderOptions.defaults())}.
    ///
    /// @return the rendered value
    virtual std::string render() = 0;

    /// _renders the config value to a string, using the provided options.
    ///
    /// <p>
    /// _if the config value has not been resolved (see {@link Config#resolve}),
    /// it's possible that it can't be rendered as valid HOCON. _in that case the
    /// rendering should still be useful for debugging but you might not be able
    /// to parse it.
    ///
    /// <p>
    /// _if the config value has been resolved and the options disable all
    /// HOCON-specific features (such as comments), the rendering will be valid
    /// JSON. _if you enable HOCON-only features such as comments, the rendering
    /// will not be valid JSON.
    ///
    /// @param options
    ///            the rendering options
    /// @return the rendered value
    virtual std::string render(const ConfigRenderOptionsPtr& options) = 0;

    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) = 0;

    /// _places the config inside another {@code Config} at the given path.
    ///
    /// @param path
    ///            path to store this config at.
    /// @return a {@code Config} instance containing this config at the given
    ///         path.
    virtual ConfigPtr at_path(const std::string& path) = 0;

    /// _places the config inside a {@code Config} at the given key. _see also
    /// at_path().
    ///
    /// @param key
    ///            key to store this config at.
    /// @return a {@code Config} instance containing this config at the given
    ///         key.
    virtual ConfigPtr at_key(const std::string& key) = 0;

    /// _creates a ConfigValue from a ConfigVariant value, which may be a
    /// bool, int32_t, int64_t, std::string, map, vector, or null/blank. _a map
    /// must be a map from std::string to more ConfigVariant values that can be
    /// supplied to from_any_ref(). _a map will become a ConfigObject and an vector
    /// will become a ConfigList.
    ///
    /// <p>
    /// _if a map passed to from_any_ref(), the map's keys are plain keys, not path
    /// expressions. _so if your map has a key "foo.bar" then you will get one
    /// object with a key called "foo.bar", rather than an object with a key
    /// "foo" containing another object with a key "bar".
    ///
    /// <p>
    /// _the origin_description will be used to set the origin() field on the
    ///ConfigValue. _it should normally be the name of the file the values came
    /// from, or something short describing the value such as "default settings".
    /// _the origin_description is prefixed to error messages so users can tell
    /// where problematic values are coming from.
    ///
    /// <p>
    /// _supplying the result of ConfigValue.unwrapped() to this function is
    /// guaranteed to work and should give you back a ConfigValue that matches
    /// the one you unwrapped. _the re-wrapped ConfigValue will lose some
    /// information that was present in the original such as its origin, but it
    /// will have matching values.
    ///
    /// <p>
    /// _this function throws if you supply a value that cannot be converted to a
    ///ConfigValue, but supplying such a value is a bug in your program, so you
    /// should never handle the exception. _just fix your program (or report a bug
    /// against this library).
    ///
    /// @param object
    ///            object to convert to ConfigValue
    /// @param origin_description
    ///            name of origin file or brief description of what the value is
    /// @return a new value
    static ConfigValuePtr from_any_ref(const ConfigVariant& object,
                                          const std::string& origin_description = "");

    /// _see the from_any_ref() documentation for details. _this is a typesafe
    /// wrapper that only works on {@link MapVariant} and returns
    /// {@link ConfigObject} rather than {@link ConfigValue}.
    ///
    /// <p>
    /// _if your map has a key "foo.bar" then you will get one object with a key
    /// called "foo.bar", rather than an object with a key "foo" containing
    /// another object with a key "bar". _the keys in the map are keys; not path
    /// expressions. _that is, the map corresponds exactly to a single
    /// {@code ConfigObject}. _the keys will not be parsed or modified, and the
    /// values are wrapped in ConfigValue. _to get nested {@code ConfigObject},
    /// some of the values in the map would have to be more maps.
    ///
    /// <p>
    /// _see also {@link Config#parse_map(MapVariant,std::string)} which interprets
    /// the keys in the map as path expressions.
    ///
    /// @param values
    /// @param origin_description
    /// @return a new {@link ConfigObject} value
    static ConfigObjectPtr from_map(const MapVariant& values,
                                       const std::string& origin_description = "");

    /// _see the from_any_ref() documentation for details. _this is a typesafe
    /// wrapper that only works on {@link VectorVariant} and returns
    /// {@link ConfigList} rather than {@link ConfigValue}.
    ///
    /// @param values
    /// @param origin_description
    /// @return a new {@link ConfigList} value
    static ConfigListPtr from_vector(const VectorVariant& values,
                                        const std::string& origin_description = "");
};

}

#endif // CONFIG_CONFIG_VALUE_HPP
