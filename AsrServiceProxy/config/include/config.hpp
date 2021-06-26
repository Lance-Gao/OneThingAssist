
#ifndef CONFIG_CONFIG_HPP
#define CONFIG_CONFIG_HPP

#include "config_mergeable.hpp"

namespace config {

///
/// _an immutable map from config paths to config values.
///
/// <p>
/// _contrast with {@link ConfigObject} which is a map from config <em>keys</em>,
/// rather than paths, to config values. _a {@code Config} contains a tree of
/// {@code ConfigObject}, and {@link Config#root()} returns the tree's root
/// object.
///
/// <p>
/// _throughout the API, there is a distinction between "keys" and "paths". _a key
/// is a key in a JSON object; it's just a string that's the key in a map. _a
/// "path" is a parseable expression with a syntax and it refers to a series of
/// keys. Path expressions are described in the <a
/// href="https://github.com/typesafehub/config/blob/master/HOCON.md">spec for
/// _human-_optimized Config _object _notation</a>. _in brief, a path is
/// period-separated so "a.b.c" looks for key c in object b in object a in the
/// root object. _sometimes double quotes are needed around special characters in
/// path expressions.
///
/// <p>
/// _the API for a {@code Config} is in terms of path expressions, while the API
/// for a {@code ConfigObject} is in terms of keys. _conceptually, {@code Config}
/// is a one-level map from <em>paths</em> to values, while a
/// {@code ConfigObject} is a tree of nested maps from <em>keys</em> to values.
///
/// <p>
/// _use {@link ConfigUtil#join_path} and {@link ConfigUtil#split_path} to convert
/// between path expressions and individual path elements (keys).
///
/// <p>
/// _another difference between {@code Config} and {@code ConfigObject} is that
/// conceptually, {@code ConfigValue}s with a {@link ConfigValue#value_type()
/// value_type()} of {@link ConfigValueType#NONE NONE} exist in a
/// {@code ConfigObject}, while a {@code Config} treats null values as if they
/// were missing.
///
/// <p>
/// {@code Config} is an immutable object and thus safe to use from multiple
/// threads. _there's never a need for "defensive copies."
///
/// <p>
/// _the "getters" on a {@code Config} all work in the same way. _they never return
/// null, nor do they return a {@code ConfigValue} with
/// {@link ConfigValue#value_type() value_type()} of {@link ConfigValueType#NONE
/// NONE}. _instead, they throw {@link ConfigExceptionMissing} if the value is
/// completely absent or set to null. _if the value is set to null, a subtype of
/// {@code ConfigExceptionMissing} called {@link ConfigExceptionNull} will be
/// thrown. {@link ConfigExceptionWrongType} will be thrown anytime you ask for
/// a type and the value has an incompatible type. _reasonable type conversions
/// are performed for you though.
///
/// <p>
/// _if you want to iterate over the contents of a {@code Config}, you can get its
/// {@code ConfigObject} with {@link #root()}, and then iterate over the
/// {@code ConfigObject} (which implements <code>std::unordered_map</code>). _or,
/// you can use {@link #entry_set()} which recurses the object tree for you and
/// builds up a <code>std::unordered_set</code> of all path-value pairs where the
/// value is not null.
///
/// <p>
/// <em>_do not implement {@code Config}</em>; it should only be implemented by
/// the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class Config : public virtual ConfigMergeable {
public:
    /// _loads an application's configuration from the given file basename,
    /// sandwiches it between default reference
    /// config and default overrides, and then resolves it.
    ///
    /// <p>
    /// _the loaded object will already be resolved (substitutions have already
    /// been processed). _as a result, if you add more fallbacks then they won't
    /// be seen by substitutions. _substitutions are the "${foo.bar}" syntax. _if
    /// you want to parse additional files or something then you need to use
    /// {@link #load(Config)}.
    ///
    /// @param file_basename
    ///            name (optionally without extension) of a config file
    /// @return resolved configuration with overrides and fallbacks added
    static ConfigPtr load(const std::string& file_basename);

    /// _like {@link #load(const std::string&)} but allows you to specify parse
    /// and resolve options.
    ///
    /// @param file_basename
    ///            name (optionally without extension) of a config file
    /// @param parse_options
    ///            options to use when parsing the file
    /// @param resolve_options
    ///            options to use when resolving the stack
    /// @return resolved configuration with overrides and fallbacks added
    static ConfigPtr load(const std::string& file_basename,
                            const ConfigParseOptionsPtr& parse_options,
                            const ConfigResolveOptionsPtr& resolve_options);

    /// _assembles a standard configuration using a custom <code>Config</code>
    /// object rather than loading "application.conf". _the <code>Config</code>
    /// object will be sandwiched between the default reference config and
    /// default overrides and then resolved.
    ///
    /// @param config
    ///            the application's portion of the configuration
    /// @return resolved configuration with overrides and fallbacks added
    static ConfigPtr load(const ConfigPtr& config,
                            const ConfigResolveOptionsPtr& resolve_options);

    /// _obtains the default override configuration, which currently consists of
    /// environment variables. _the returned override configuration will already
    /// have substitutions resolved.
    ///
    /// <p>
    /// _the {@link #load()} methods merge this configuration for you
    /// automatically.
    ///
    /// <p>
    /// _future versions may get overrides in more places. _it is not guaranteed
    /// that this method <em>only</em> uses system properties.
    ///
    /// @return the default override configuration
    static ConfigPtr default_overrides();

    /// _gets an empty configuration with a description to be used to create a
    /// {@link ConfigOrigin} for this <code>Config</code>. _the description should
    /// be very short and say what the configuration is, like "default settings"
    /// or "foo settings" or something. (_presumably you will merge some actual
    /// settings into this empty config using {@link Config#with_fallback}, making
    /// the description more useful.)
    ///
    /// @param origin_description
    ///            description of the config
    /// @return an empty configuration
    static ConfigPtr empty_config(const std::string& origin_description = "");

    static ConfigPtr parse_reader(const ReaderPtr& reader,
                                    const ConfigParseOptionsPtr& options = nullptr);

    static ConfigPtr parse_file(const std::string& file,
                                  const ConfigParseOptionsPtr& options = nullptr);

    /// _parses a file with a flexible extension. _if the <code>file_basename</code>
    /// already ends in a known extension, this method parses it according to
    /// that extension (the file's syntax must match its extension). _if the
    /// <code>file_basename</code> does not end in an extension, it parses files
    /// with all known extensions and merges whatever is found.
    ///
    /// <p>
    /// _in the current implementation, the extension ".conf" forces
    /// {@link ConfigSyntax#CONF}, ".json" forces {@link ConfigSyntax#JSON}, and
    /// ".properties" forces {@link ConfigSyntax#PROPERTIES}. _when merging files,
    /// ".conf" falls back to ".json" falls back to ".properties".
    ///
    /// <p>
    /// _future versions of the implementation may add additional syntaxes or
    /// additional extensions. _however, the ordering (fallback priority) of the
    /// three current extensions will remain the same.
    ///
    /// <p>
    /// _if <code>options</code> forces a specific syntax, this method only parses
    /// files with an extension matching that syntax.
    ///
    /// <p>
    /// _if {@link ConfigParseOptions#get_allow_missing options.get_allow_missing()}
    /// is true, then no files have to exist; if false, then at least one file
    /// has to exist.
    ///
    /// @param file_basename
    ///            a filename with or without extension
    /// @param options
    ///            parse options
    /// @return the parsed configuration
    static ConfigPtr parse_file_any_syntax(const std::string& file_basename,
            const ConfigParseOptionsPtr& options = nullptr);

    static ConfigPtr parse_string(const std::string& s,
                                    const ConfigParseOptionsPtr& options = nullptr);

    static ConfigPtr parse_map(const MapVariant& values,
                                 const std::string& origin_description = "");

    /// _gets the {@code Config} as a tree of {@link ConfigObject}. _this is a
    /// constant-time operation (it is not proportional to the number of values
    /// in the {@code Config}).
    ///
    /// @return the root object in the configuration
    virtual ConfigObjectPtr root() = 0;

    /// _gets the origin of the {@code Config}, which may be a file, or a file
    /// with a line number, or just a descriptive phrase.
    ///
    /// @return the origin of the {@code Config} for use in error messages
    virtual ConfigOriginPtr origin() = 0;

    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) = 0;

    /// _returns a replacement config with all substitutions (the
    /// <code>${foo.bar}</code> syntax, see <a
    /// href="https://github.com/typesafehub/config/blob/master/HOCON.md">the
    /// spec</a>) resolved. _substitutions are looked up using this
    /// <code>Config</code> as the root object, that is, a substitution
    /// <code>${foo.bar}</code> will be replaced with the result of
    /// <code>get_value("foo.bar")</code>.
    ///
    /// <p>
    /// _this method uses {@link ConfigResolveOptions#defaults()}, there is
    /// another variant {@link Config#resolve(ConfigResolveOptions)} which lets
    /// you specify non-default options.
    ///
    /// <p>
    /// _a given {@link Config} must be resolved before using it to retrieve
    /// config values, but ideally should be resolved one time for your entire
    /// stack of fallbacks (see {@link Config#with_fallback}). _otherwise, some
    /// substitutions that could have resolved with all fallbacks available may
    /// not resolve, which will be a user-visible oddity.
    ///
    /// <p>
    /// <code>resolve()</code> should be invoked on root config objects, rather
    /// than on a subtree (a subtree is the result of something like
    /// <code>config->get_config("foo")</code>). _the problem with
    /// <code>resolve()</code> on a subtree is that substitutions are relative to
    /// the root of the config and the subtree will have no way to get values
    /// from the root. _for example, if you did
    /// <code>config->get_config("foo")->resolve()</code> on the below config file,
    /// it would not work:
    ///
    /// <pre>
    ///   common-value = 10
    ///   foo {
    ///      whatever = ${common-value}
    ///   }
    /// </pre>
    ///
    /// @return an immutable object with substitutions resolved
    /// @throws ConfigExceptionUnresolvedSubstitution
    ///             if any substitutions refer to nonexistent paths
    /// @throws ConfigException
    ///             some other config exception if there are other problems
    virtual ConfigPtr resolve() = 0;

    /// _like {@link Config#resolve()} but allows you to specify non-default
    /// options.
    ///
    /// @param options
    ///            resolve options
    /// @return the resolved <code>Config</code>
    virtual ConfigPtr resolve(const ConfigResolveOptionsPtr& options) = 0;

    /// _validates this config against a reference config, throwing an exception
    /// if it is invalid. _the purpose of this method is to "fail early" with a
    /// comprehensive list of problems; in general, anything this method can find
    /// would be detected later when trying to use the config, but it's often
    /// more user-friendly to fail right away when loading the config.
    ///
    /// <p>
    /// _using this method is always optional, since you can "fail late" instead.
    ///
    /// <p>
    /// _you must restrict validation to paths you "own" (those whose meaning are
    /// defined by your code module). _if you validate globally, you may trigger
    /// errors about paths that happen to be in the config but have nothing to do
    /// with your module. _it's best to allow the modules owning those paths to
    /// validate them. _also, if every module validates only its own stuff, there
    /// isn't as much redundant work being done.
    ///
    /// <p>
    /// _if no paths are specified in <code>check_valid()</code>'s parameter list,
    /// validation is for the entire config.
    ///
    /// <p>
    /// _if you specify paths that are not in the reference config, those paths
    /// are ignored. (_there's nothing to validate.)
    ///
    /// <p>
    /// _here's what validation involves:
    ///
    /// <ul>
    /// <li>_all paths found in the reference config must be present in this
    /// config or an exception will be thrown.
    /// <li>
    /// _some changes in type from the reference config to this config will cause
    /// an exception to be thrown. _not all potential type problems are detected,
    /// in particular it's assumed that strings are compatible with everything
    /// except objects and lists. _this is because string types are often "really"
    /// some other type (system properties always start out as strings, or a
    /// string like "5ms" could be used with {@link #get_milliseconds}). _also,
    /// it's allowed to set any type to null or override null with any type.
    /// <li>
    /// _any unresolved substitutions in this config will cause a validation
    /// failure; both the reference config and this config should be resolved
    /// before validation. _if the reference config is unresolved, it's a bug in
    /// the caller of this method.
    /// </ul>
    ///
    /// <p>
    /// _if you want to allow a certain setting to have a flexible type (or
    /// otherwise want validation to be looser for some settings), you could
    /// either remove the problematic setting from the reference config provided
    /// to this method, or you could intercept the validation exception and
    /// screen out certain problems. _of course, this will only work if all other
    /// callers of this method are careful to restrict validation to their own
    /// paths, as they should be.
    ///
    /// <p>
    /// _if validation fails, the thrown exception contains a list of all problems
    /// found. _see {@link ConfigExceptionValidationFailed#problems}. _the
    /// exception's <code>what()</code> will have all the problems
    /// concatenated into one huge string, as well.
    ///
    /// <p>
    /// _again, <code>check_valid()</code> can't guess every domain-specific way a
    /// setting can be invalid, so some problems may arise later when attempting
    /// to use the config. <code>check_valid()</code> is limited to reporting
    /// generic, but common, problems such as missing settings and blatant type
    /// incompatibilities.
    ///
    /// @param reference
    ///            a reference configuration
    /// @param restrict_to_paths
    ///            only validate values underneath these paths that your code
    ///            module owns and understands
    /// @throws ConfigExceptionValidationFailed
    ///             if there are any validation issues
    /// @throws ConfigExceptionNotResolved
    ///             if this config is not resolved
    /// @throws ConfigExceptionBugOrBroken
    ///             if the reference config is unresolved or caller otherwise
    ///             misuses the API
    virtual void check_valid(const ConfigPtr& reference,
                             const VectorString& restrict_to_paths = VectorString()) = 0;

    /// _checks whether a value is present and non-null at the given path. _this
    /// differs in two ways from {@code _map.contains_key()} as implemented by
    /// {@link ConfigObject}: it looks for a path expression, not a key; and it
    /// returns false for null values, while {@code contains_key()} returns true
    /// indicating that the object contains a null value for the key.
    ///
    /// <p>
    /// _if a path exists according to {@link #has_path(std::string)}, then
    /// {@link #get_value(std::string)} will never throw an exception. _however, the
    /// typed getters, such as {@link #get_int(std::string)}, will still throw if the
    /// value is not convertible to the requested type.
    ///
    /// @param path
    ///            the path expression
    /// @return true if a non-null value is present at the path
    /// @throws ConfigExceptionBadPath
    ///             if the path expression is invalid
    virtual bool has_path(const std::string& path) = 0;

    /// _returns true if the {@code Config}'s root object contains no key-value
    /// pairs.
    ///
    /// @return true if the configuration is empty
    virtual bool empty() = 0;

    /// _returns the set of path-value pairs, excluding any null values, found by
    /// recursing {@link #root() the root object}. _note that this is very
    /// different from <code>root()->entry_set()</code> which returns the set of
    /// immediate-child keys in the root object and includes null values.
    ///
    /// @return set of paths with non-null values, built up by recursing the
    ///         entire tree of {@link ConfigObject}
    virtual SetConfigValue entry_set() = 0;

    /// @param path
    ///            path expression
    /// @return the boolean value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to boolean
    virtual bool get_boolean(const std::string& path) = 0;

    /// @param path
    ///            path expression
    /// @return the 32-bit integer value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to an int (for example it is out
    ///             of range, or it's a boolean value)
    virtual int32_t get_int(const std::string& path) = 0;

    /// @param path
    ///            path expression
    /// @return the 64-bit integer value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to an int64
    virtual int64_t get_int64(const std::string& path) = 0;

    /// @param path
    ///            path expression
    /// @return the floating-point value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to an double
    virtual double get_double(const std::string& path) = 0;

    /// @param path
    ///            path expression
    /// @return the string value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to an string
    virtual std::string get_string(const std::string& path) = 0;

    /// @param path
    ///            path expression
    /// @return the {@link ConfigObject} value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to an object
    virtual ConfigObjectPtr get_object(const std::string& path) = 0;

    /// @param path
    ///            path expression
    /// @return the nested {@code Config} value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to a Config
    virtual ConfigPtr get_config(const std::string& path) = 0;

    /// _gets the value at the path as an unwrapped boost::variant value (see
    /// {@link ConfigValue#unwrapped()}).
    ///
    /// @param path
    ///            path expression
    /// @return the unwrapped value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    virtual ConfigVariant get_variant(const std::string& path) = 0;

    /// _gets the value at the given path, unless the value is a null value
    /// or missing, in which case it throws just like the other getters. _use
    /// {@code get()} on the {@link Config#root()} object (or other object in
    /// the tree) if you want an unprocessed value.
    ///
    /// @param path
    ///            path expression
    /// @return the value at the requested path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    virtual ConfigValuePtr get_value(const std::string& path) = 0;

    /// _gets a value as a size in bytes (parses special strings like "128_m"). _if
    /// the value is already a number, then it's left alone; if it's a string,
    /// it's parsed understanding unit suffixes such as "128_k", as documented in
    /// the <a
    /// href="https://github.com/typesafehub/config/blob/master/HOCON.md">the
    /// spec</a>.
    ///
    /// @param path
    ///            path expression
    /// @return the value at the requested path, in bytes
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to uint64_t or std::string
    /// @throws ConfigExceptionBadValue
    ///             if value cannot be parsed as a size in bytes
    virtual uint64_t get_bytes(const std::string& path) = 0;

    /// _get value as a duration in milliseconds. _if the value is already a
    /// number, then it's left alone; if it's a string, it's parsed understanding
    /// units suffixes like "10m" or "5ns" as documented in the <a
    /// href="https://github.com/typesafehub/config/blob/master/HOCON.md">the
    /// spec</a>.
    ///
    /// @param path
    ///            path expression
    /// @return the duration value at the requested path, in milliseconds
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to uint64_t or std::string
    /// @throws ConfigExceptionBadValue
    ///             if value cannot be parsed as a number of milliseconds
    virtual uint64_t get_milliseconds(const std::string& path) = 0;

    /// _get value as a duration in nanoseconds. _if the value is already a number
    /// it's taken as milliseconds and converted to nanoseconds. _if it's a
    /// string, it's parsed understanding unit suffixes, as for
    /// {@link #get_milliseconds(std::string)}.
    ///
    /// @param path
    ///            path expression
    /// @return the duration value at the requested path, in nanoseconds
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to uint64_t or std::string
    /// @throws ConfigExceptionBadValue
    ///             if value cannot be parsed as a number of nanoseconds
    virtual uint64_t get_nanoseconds(const std::string& path) = 0;

    /// _gets a list value (with any element type) as a {@link ConfigList}, which
    /// implements {@code std::vector<ConfigValuePtr>}. _throws if the path is
    /// unset or null.
    ///
    /// @param path
    ///            the path to the list value.
    /// @return the {@link ConfigList} at the path
    /// @throws ConfigExceptionMissing
    ///             if value is absent or null
    /// @throws ConfigExceptionWrongType
    ///             if value is not convertible to a ConfigList
    virtual ConfigListPtr get_list(const std::string& path) = 0;

    virtual VectorBool get_boolean_list(const std::string& path) = 0;
    virtual VectorInt get_int_list(const std::string& path) = 0;
    virtual VectorInt64 get_int64_list(const std::string& path) = 0;
    virtual VectorDouble get_double_list(const std::string& path) = 0;
    virtual VectorString get_string_list(const std::string& path) = 0;
    virtual VectorConfigObject get_object_list(const std::string& path) = 0;
    virtual VectorConfig get_config_list(const std::string& path) = 0;
    virtual VectorVariant get_variant_list(const std::string& path) = 0;
    virtual VectorInt64 get_bytes_list(const std::string& path) = 0;
    virtual VectorInt64 get_milliseconds_list(const std::string& path) = 0;
    virtual VectorInt64 get_nanoseconds_list(const std::string& path) = 0;

    /// _clone the config with only the given path (and its children) retained;
    /// all sibling paths are removed.
    ///
    /// @param path
    ///            path to keep
    /// @return a copy of the config minus all paths except the one specified
    virtual ConfigPtr with_only_path(const std::string& path) = 0;

    /// _clone the config with the given path removed.
    ///
    /// @param path
    ///            path to remove
    /// @return a copy of the config minus the specified path
    virtual ConfigPtr without_path(const std::string& path) = 0;

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

    /// _returns a {@code Config} based on this one, but with the given path set
    /// to the given value. _does not modify this instance (since it's immutable).
    /// _if the path already has a value, that value is replaced. _to remove a
    /// value, use without_path().
    ///
    /// @param path
    ///            path to add
    /// @param value
    ///            value at the new path
    /// @return the new instance with the new map entry
    virtual ConfigPtr with_value(const std::string& path, const ConfigValuePtr& value) = 0;
};

}

#endif // CONFIG_CONFIG_HPP
