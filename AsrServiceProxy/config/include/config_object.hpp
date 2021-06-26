
#ifndef CONFIG_CONFIG_OBJECT_HPP
#define CONFIG_CONFIG_OBJECT_HPP

#include "config_value.hpp"

namespace config {

///
/// _subtype of {@link ConfigValue} representing an object (dictionary, map)
/// value, as in JSON's <code>{ "a" : 42 }</code> syntax.
///
/// <p>
/// {@code ConfigObject} implements {@code MapConfigValue} so you can use it
/// like a regular std map. _or call {@link #unwrapped()} to unwrap the map to a
/// map with plain values rather than {@code ConfigValue}.
///
/// <p>
/// _like all {@link ConfigValue} subtypes, {@code ConfigObject} is immutable.
/// _this makes it threadsafe and you never have to create "defensive copies".
///
/// <p>
/// _the {@link ConfigValue#value_type} method on an object returns
/// {@link ConfigValueType#OBJECT}.
///
/// <p>
/// _in most cases you want to use the {@link Config} interface rather than this
/// one. _call {@link #to_config()} to convert a {@code ConfigObject} to a
/// {@code Config}.
///
/// <p>
/// _the API for a {@code ConfigObject} is in terms of keys, while the API for a
/// {@link Config} is in terms of path expressions. _conceptually,
/// {@code ConfigObject} is a tree of maps from keys to values, while a
/// {@code Config} is a one-level map from paths to values.
///
/// <p>
/// _use {@link ConfigUtil#join_path} and {@link ConfigUtil#split_path} to convert
/// between path expressions and individual path elements (keys).
///
/// <p>
/// _a {@code ConfigObject} may contain null values, which will have
/// {@link ConfigValue#value_type()} equal to {@link ConfigValueType#NULL}. _if
/// {@code get()} returns empty then the key was not present in the parsed
/// file (or wherever this value tree came from). _if {@code get()} returns a
/// {@link ConfigValue} with type {@code ConfigValueType#NULL} then the key was
/// set to null explicitly in the config file.
///
/// <p>
/// <em>_do not implement {@code ConfigObject}</em>; it should only be implemented
/// by the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class ConfigObject : public virtualConfigValue, public virtual MapConfigValue {
public:
    /// _converts this object to a {@link Config} instance, enabling you to use
    /// path expressions to find values in the object. _this is a constant-time
    /// operation (it is not proportional to the size of the object).
    ///
    /// @return a {@link Config} with this object as its root
    virtual ConfigPtr to_config() = 0;

    /// _recursively unwraps the object, returning a map from std::string to
    /// whatever plain values are unwrapped from boost:variant values.
    ///
    /// @return a {@link MapVariant} containing plain objects
    virtual ConfigVariant unwrapped() = 0;

    /// _alternative to unwrapping the value to a ConfigVariant.
    template <typename _t> _t unwrapped() {
        return variant_get<MapVariant>(unwrapped());
    }

    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) = 0;

    /// _gets a {@link ConfigValue} at the given key, or returns null if there is
    /// no value. _the returned {@link ConfigValue} may have
    /// {@link ConfigValueType#NONE} or any other type, and the passed-in key
    /// must be a key in this object, rather than a path expression.
    ///
    /// @param key
    ///            key to look up
    ///
    /// @return the value at the key or null if none
    virtual ConfigValuePtr get(const std::string& key) = 0;

    /// _clone the object with only the given key (and its children) retained; all
    /// sibling keys are removed.
    ///
    /// @param key
    ///            key to keep
    /// @return a copy of the object minus all keys except the one specified
    virtual ConfigObjectPtr with_only_key(const std::string& key) = 0;

    /// _clone the object with the given key removed.
    ///
    /// @param key
    ///            key to remove
    /// @return a copy of the object minus the specified key
    virtual ConfigObjectPtr without_key(const std::string& key) = 0;

    /// _returns a {@code ConfigObject} based on this one, but with the given key
    /// set to the given value. _does not modify this instance (since it's
    /// immutable). _if the key already has a value, that value is replaced. _to
    /// remove a value, use without_key().
    ///
    /// @param key
    ///            key to add
    /// @param value
    ///            value at the new key
    /// @return the new instance with the new map entry
    virtual ConfigObjectPtr with_value(const std::string& key, const ConfigValuePtr& value) = 0;

    /// _return set of key-value pairs.
    virtual SetConfigValue entry_set();

    virtual MapConfigValue::const_iterator begin() const;
    virtual MapConfigValue::const_iterator end() const;
    virtual MapConfigValue::mapped_type operator[](const MapConfigValue::key_type& key) const;
    virtual bool empty() const;
    virtual MapConfigValue::size_type size() const;
    virtual MapConfigValue::size_type count(const MapConfigValue::key_type& key) const;
    virtual MapConfigValue::const_iterator find(const MapConfigValue::key_type& key) const;

    virtual void clear();
    virtual MapConfigValue::size_type erase(const MapConfigValue::key_type& key);
    virtual void erase(const MapConfigValue::const_iterator& pos);
    virtual void insert(const MapConfigValue::value_type& val);
};

}

#endif // CONFIG_CONFIG_OBJECT_HPP
