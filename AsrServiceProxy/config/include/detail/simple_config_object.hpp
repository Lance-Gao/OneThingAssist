
#ifndef CONFIG_SIMPLE_CONFIG_OBJECT_HPP
#define CONFIG_SIMPLE_CONFIG_OBJECT_HPP

#include "detail/abstract_config_object.hpp"

namespace config {

class SimpleConfigObject : public AbstractConfigObject {
public:
    CONFIG_CLASS(SimpleConfigObject);

    SimpleConfigObject(const ConfigOriginPtr& origin,
                          const MapAbstractConfigValue& value,
                          ResolveStatus status,
                          bool ignores_fallbacks);
    SimpleConfigObject(const ConfigOriginPtr& origin,
                          const MapAbstractConfigValue& value);

    virtual ConfigObjectPtr with_only_key(const std::string& key) override;
    virtual ConfigObjectPtr without_key(const std::string& key) override;

    /// _gets the object with only the path if the path
    /// exists, otherwise null if it doesn't. _this ensures
    /// that if we have { a : { b : 42 } } and do
    /// with_only_path("a.b.c") that we don't keep an empty
    /// "a" object.
    virtual AbstractConfigObjectPtr with_only_path_or_null(const PathPtr& path) override;

    virtual AbstractConfigObjectPtr with_only_path(const PathPtr& path) override;
    virtual AbstractConfigObjectPtr without_path(const PathPtr& path) override;

    virtual ConfigObjectPtr with_value(const std::string& key,
                                          const ConfigValuePtr& value) override;
    virtual ConfigObjectPtr with_value(const PathPtr& path,
                                          const ConfigValuePtr& value) override;

    virtual AbstractConfigValuePtr attempt_peek_with_partial_resolve(const std::string& key)
    override;

private:
    virtual AbstractConfigObjectPtr new_copy(ResolveStatus status,
            const ConfigOriginPtr& origin,
            bool new_ignores_fallbacks);

protected:
    virtual AbstractConfigObjectPtr new_copy(ResolveStatus status,
            const ConfigOriginPtr& origin) override;

protected:
    virtual AbstractConfigValuePtr with_fallbacks_ignored() override;

public:
    virtual ResolveStatus resolve_status() override;

    virtual bool ignores_fallbacks() override;

public:
    virtual ConfigVariant unwrapped() override;

    /// _alternative to unwrapping the value to a ConfigVariant.
    template <typename _t> _t unwrapped() {
        return variant_get<_t>(unwrapped());
    }

protected:
    using AbstractConfigObject::merged_with_object;
    virtual AbstractConfigValuePtr merged_with_object(const AbstractConfigObjectPtr& fallback)
    override;

private:
    SimpleConfigObjectPtr modify(const _no_exceptionsModifierPtr& modifier);
    SimpleConfigObjectPtr modify_may_throw(const ModifierPtr& modifier);

public:
    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override;
    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix) override;

protected:
    virtual void render(std::string& s,
                        uint32_t indent,
                        const ConfigRenderOptionsPtr& options) override;

public:
    virtual ConfigValuePtr get(const std::string& key) override;

private:
    static bool map_equals(const MapConfigValue& a, const MapConfigValue& b);
    static uint32_t map_hash(const MapConfigValue& m);

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

public:
    virtual MapConfigValue::const_iterator begin() const override;
    virtual MapConfigValue::const_iterator end() const override;
    virtual MapConfigValue::mapped_type operator[](const MapConfigValue::key_type& key) const
    override;
    virtual bool empty() const override;
    virtual MapConfigValue::size_type size() const override;
    virtual MapConfigValue::size_type count(const MapConfigValue::key_type& key) const override;
    virtual MapConfigValue::const_iterator find(const MapConfigValue::key_type& key) const
    override;

public:
    static SimpleConfigObjectPtr make_empty();
    static SimpleConfigObjectPtr make_empty(const ConfigOriginPtr& origin);
    static SimpleConfigObjectPtr make_empty_missing(const ConfigOriginPtr& base_origin);

private:
    // this map should never be modified - assume immutable
    MapConfigValue value;
    bool resolved;
    bool ignores_fallbacks_;
};

class SimpleConfigObjectModifier : public virtual Modifier, public ConfigBase {
public:
    CONFIG_CLASS(SimpleConfigObjectModifier);

    SimpleConfigObjectModifier(const ResolveContextPtr& context);

    virtual AbstractConfigValuePtr modify_child_may_throw(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) override;

private:
    ResolveContextPtr context;
};

class SimpleConfigObjectNoExceptionsModifier : public NoExceptionsModifier {
public:
    CONFIG_CLASS(SimpleConfigObjectNoExceptionsModifier);

    SimpleConfigObjectNoExceptionsModifier(const PathPtr& prefix);

    virtual AbstractConfigValuePtr modify_child(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) override;

private:
    PathPtr prefix;
};

}

#endif // CONFIG_SIMPLE_CONFIG_OBJECT_HPP
