
#ifndef CONFIG_CONFIG_LIST_HPP
#define CONFIG_CONFIG_LIST_HPP

#include "config_value.hpp"

namespace config {

///
/// _subtype of {@link ConfigValue} representing a list value, as in JSON's
/// {@code [1,2,3]} syntax.
///
/// <p>
/// {@code ConfigList} implements {@code VectorConfigValue} so you can
/// use it like a regular vector. _or call {@link #unwrapped()} to unwrap the
/// list elements into plain values.
///
/// <p>
/// _like all {@link ConfigValue} subtypes, {@code ConfigList} is immutable. _this
/// makes it threadsafe and you never have to create "defensive copies.".
///
/// <p>
/// _the {@link ConfigValue#value_type} method on a list returns
/// {@link ConfigValueType#LIST}.
///
/// <p>
/// <em>_do not implement {@code ConfigList}</em>; it should only be implemented
/// by the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class ConfigList : public virtual VectorConfigValue, public virtualConfigValue {
public:
    /// _recursively unwraps the list, returning a list of plain values such
    /// as int32_t or std::string or whatever is in the list.
    virtual ConfigVariant unwrapped() = 0;

    /// _alternative to unwrapping the value to a ConfigVariant.
    template <typename _t> _t unwrapped() {
        return variant_get<_t>(unwrapped());
    }

    virtual VectorConfigValue::const_iterator begin() const;
    virtual VectorConfigValue::const_iterator end() const;
    virtual VectorConfigValue::const_reference at(VectorConfigValue::size_type n) const;
    virtual VectorConfigValue::const_reference front() const;
    virtual VectorConfigValue::const_reference back() const;
    virtual VectorConfigValue::const_reference operator[](VectorConfigValue::size_type n) const;
    virtual bool empty() const;
    virtual VectorConfigValue::size_type size() const;
    virtual void clear();
    virtual void pop_back();
    virtual void resize(VectorConfigValue::size_type n,
                        const VectorConfigValue::value_type& val = nullptr);
    virtual VectorConfigValue::const_iterator erase(VectorConfigValue::const_iterator pos);
    virtual VectorConfigValue::const_iterator insert(VectorConfigValue::const_iterator pos,
            const VectorConfigValue::value_type& val);
};

}

#endif // CONFIG_CONFIG_LIST_HPP
