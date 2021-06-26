
#ifndef CONFIG_SIMPLE_CONFIG_LIST_HPP
#define CONFIG_SIMPLE_CONFIG_LIST_HPP

#include "detail/abstract_config_value.hpp"
#include "config_list.hpp"

namespace config {

class SimpleConfigList : public AbstractConfigValue, public virtual ConfigList {
public:
    CONFIG_CLASS(SimpleConfigList);

    SimpleConfigList(const ConfigOriginPtr& origin,
                        const VectorAbstractConfigValue& value);
    SimpleConfigList(const ConfigOriginPtr& origin,
                        const VectorAbstractConfigValue& value,
                        ResolveStatus status);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;

    virtual ResolveStatus resolve_status() override;

private:
    SimpleConfigList modify(const _no_exceptionsModifierPtr& modifier,
                                   ResolveStatus new_resolve_status);
    SimpleConfigList modify_may_throw(const ModifierPtr& modifier,
            ResolveStatus new_resolve_status);

public:
    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override;

    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix) override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

protected:
    virtual void render(std::string& s,
                        uint32_t indent,
                        const ConfigRenderOptionsPtr& options) override;

public:
    virtual VectorConfigValue::const_iterator begin() const override;
    virtual VectorConfigValue::const_iterator end() const override;
    virtual VectorConfigValue::const_reference at(VectorConfigValue::size_type n) const override;
    virtual VectorConfigValue::const_reference front() const override;
    virtual VectorConfigValue::const_reference back() const override;
    virtual VectorConfigValue::const_reference operator[](VectorConfigValue::size_type n) const
    override;
    virtual bool empty() const override;
    virtual VectorConfigValue::size_type size() const override;
    virtual void clear() override;
    virtual void pop_back() override;
    virtual void resize(VectorConfigValue::size_type n,
                        const VectorConfigValue::value_type& val = nullptr) override;
    virtual VectorConfigValue::const_iterator erase(VectorConfigValue::const_iterator pos)
    override;
    virtual VectorConfigValue::const_iterator insert(VectorConfigValue::const_iterator pos,
            const VectorConfigValue::value_type& val) override;

private:
    static ConfigExceptionUnsupportedOperation we_are_immutable(const std::string& method);

public:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

    SimpleConfigList concatenate(const SimpleConfigList& other);

private:
    VectorConfigValue value;
    bool resolved;
};

class SimpleConfigListModifier : public virtual Modifier, public ConfigBase {
public:
    CONFIG_CLASS(SimpleConfigListModifier);

    SimpleConfigListModifier(const ResolveContextPtr& context);

    virtual AbstractConfigValuePtr modify_child_may_throw(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) override;

private:
    ResolveContextPtr context;
};

class SimpleConfigListNoExceptionsModifier : public NoExceptionsModifier {
public:
    CONFIG_CLASS(SimpleConfigListNoExceptionsModifier);

    SimpleConfigListNoExceptionsModifier(const PathPtr& prefix);

    virtual AbstractConfigValuePtr modify_child(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) override;

private:
    PathPtr prefix;
};

}

#endif // CONFIG_SIMPLE_CONFIG_LIST_HPP
