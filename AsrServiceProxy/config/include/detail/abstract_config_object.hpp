
#ifndef CONFIG_ABSTRACT_CONFIG_OBJECT_HPP
#define CONFIG_ABSTRACT_CONFIG_OBJECT_HPP

#include "detail/abstract_config_value.hpp"
#include "config_object.hpp"

namespace config {

class AbstractConfigObject : public AbstractConfigValue, public virtual ConfigObject {
public:
    CONFIG_CLASS(AbstractConfigObject);

    AbstractConfigObject(const ConfigOriginPtr& origin);

    virtual void initialize() override;

    virtual ConfigPtr to_config() override;

    virtual ConfigValuePtr to_fallback_value() override;

    virtual ConfigObjectPtr with_only_key(const std::string& key) = 0;
    virtual ConfigObjectPtr without_key(const std::string& key) = 0;

    virtual ConfigObjectPtr with_value(const std::string& key, const ConfigValuePtr& value) = 0;

    virtual AbstractConfigObjectPtr with_only_path_or_null(const PathPtr& path) = 0;
    virtual AbstractConfigObjectPtr with_only_path(const PathPtr& path) = 0;
    virtual AbstractConfigObjectPtr without_path(const PathPtr& path) = 0;

    virtual ConfigObjectPtr with_value(const PathPtr& path, const ConfigValuePtr& value) = 0;

    /// _this looks up the key with no transformation or type conversion of any
    /// kind, and returns null if the key is not present. _the object must be
    /// resolved; use attempt_peek_with_partial_resolve() if it is not.
    ///
    /// @param key
    /// @return the unmodified raw value or null
    virtual AbstractConfigValuePtr peek_assuming_resolved(const std::string& key,
            const PathPtr& original_path);

    /// _look up the key on an only-partially-resolved object, with no
    /// transformation or type conversion of any kind; if 'this' is not resolved
    /// then try to look up the key anyway if possible.
    ///
    /// @param key
    ///            key to look up
    /// @return the value of the key, or null if known not to exist
    /// @throws ConfigExceptionNotResolved
    ///             if can't figure out key's value or can't know whether it
    ///             exists
    virtual AbstractConfigValuePtr attempt_peek_with_partial_resolve(const std::string& key) = 0;

    /// _looks up the path with no transformation, type conversion, or exceptions
    /// (just returns null if path not found). _does however resolve the path, if
    /// resolver != null.
    ///
    /// @throws NotPossibleToResolve
    ///             if context is not null and resolution fails
    virtual AbstractConfigValuePtr peek_path(const PathPtr& path,
            const ResolveContextPtr& context);

public:
    /// _looks up the path. _doesn't do any resolution, will throw if any is
    /// needed.
    virtual AbstractConfigValuePtr peek_path(const PathPtr& path);

private:
    static AbstractConfigValuePtr peek_path(const AbstractConfigObjectPtr& self,
            const PathPtr& path,
            const ResolveContextPtr& context);

public:
    virtual ConfigValueType value_type() override;

protected:
    virtual AbstractConfigObjectPtr new_copy(ResolveStatus status,
            const ConfigOriginPtr& origin) = 0;

    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

    virtual AbstractConfigValuePtr construct_delayed_merge(const ConfigOriginPtr& origin,
            const VectorAbstractConfigValue& stack) override;

    virtual AbstractConfigValuePtr merged_with_object(const AbstractConfigObjectPtr& fallback) =
        0;

public:
    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) override;

public:
    static ConfigOriginPtr merge_origins(const VectorAbstractConfigValue& stack);
    static ConfigOriginPtr merge_origins(const VectorAbstractConfigObject& stack =
            VectorAbstractConfigObject());

    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override = 0;
    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix) override = 0;

protected:
    virtual void render(std::string& s, uint32_t indent, const ConfigRenderOptionsPtr& options) = 0;

private:
    static ConfigExceptionUnsupportedOperation we_are_immutable(const std::string& method);

public:
    MapConfigValue::const_iterator begin() const;
    MapConfigValue::const_iterator end() const;
    MapConfigValue::mapped_type operator[](const MapConfigValue::key_type& key) const;
    bool empty() const;
    MapConfigValue::size_type size() const;
    MapConfigValue::size_type count(const MapConfigValue::key_type& key) const;
    MapConfigValue::const_iterator find(const MapConfigValue::key_type& key) const;

    void clear();
    MapConfigValue::size_type erase(const MapConfigValue::key_type&);
    void erase(const MapConfigValue::const_iterator&);
    void insert(const MapConfigValue::value_type&);

private:
    SimpleConfigPtr config;
};

}

#endif // CONFIG_ABSTRACT_CONFIG_OBJECT_HPP
