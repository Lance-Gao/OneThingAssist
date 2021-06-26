
#ifndef CONFIG_CONFIG_DELAYED_MERGE_OBJECT_HPP
#define CONFIG_CONFIG_DELAYED_MERGE_OBJECT_HPP

#include "detail/abstract_config_object.hpp"
#include "detail/unmergeable.hpp"
#include "detail/replaceable_merge_stack.hpp"
#include "detail/resolve_replacer.hpp"

namespace config {

///
/// _this is just like ConfigDelayedMergeObject except we know statically
/// that it will turn out to be an object.
///
class ConfigDelayedMergeObject : public AbstractConfigObject, public virtual Unmergeable,
    public virtual ReplaceableMergeStack {
public:
    CONFIG_CLASS(ConfigDelayedMergeObject);

    ConfigDelayedMergeObject(const ConfigOriginPtr& origin,
                                 const VectorAbstractConfigValue& stack);

    virtual AbstractConfigObjectPtr new_copy(ResolveStatus status,
            const ConfigOriginPtr& origin) override;
    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override;
    virtual ResolveReplacerPtr make_replacer(uint32_t skipping) override;
    virtual ResolveStatus resolve_status() override;
    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix) override;

    virtual bool ignores_fallbacks() override;

protected:
    using AbstractConfigObject::merged_with_the_unmergeable;
    virtual AbstractConfigValuePtr merged_with_the_unmergeable(const UnmergeablePtr& fallback)
    override;

    using AbstractConfigObject::merged_with_object;
    virtual AbstractConfigValuePtr merged_with_object(const AbstractConfigObjectPtr& fallback)
    override;

    using AbstractConfigObject::merged_with_non_object;
    virtual AbstractConfigValuePtr merged_with_non_object(const AbstractConfigValuePtr&
            fallback) override;

public:
    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) override;

    virtual ConfigObjectPtr with_only_key(const std::string& key) override;
    virtual ConfigObjectPtr without_key(const std::string& key) override;
    virtual AbstractConfigObjectPtr with_only_path_or_null(const PathPtr& path) override;
    virtual AbstractConfigObjectPtr with_only_path(const PathPtr& path) override;
    virtual AbstractConfigObjectPtr without_path(const PathPtr& path) override;

    virtual ConfigObjectPtr with_value(const std::string& key,
                                          const ConfigValuePtr& value) override;
    virtual ConfigObjectPtr with_value(const PathPtr& path,
                                          const ConfigValuePtr& value) override;

    virtual VectorAbstractConfigValue unmerged_values() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

    virtual void render(std::string& s,
                        uint32_t indent,
                        const boost::optional<std::string>& at_key,
                        const ConfigRenderOptionsPtr& options) override;
    virtual void render(std::string& s,
                        uint32_t indent,
                        const ConfigRenderOptionsPtr& options) override;

private:
    static ConfigExceptionNotResolved not_resolved();

public:
    virtual ConfigVariant unwrapped() override;
    virtual ConfigValuePtr get(const std::string& key) override;

    virtual MapConfigValue::const_iterator begin() const override;
    virtual MapConfigValue::const_iterator end() const override;
    virtual MapConfigValue::mapped_type operator[](const MapConfigValue::key_type& key) const
    override;
    virtual bool empty() const override;
    virtual MapConfigValue::size_type size() const override;
    virtual MapConfigValue::size_type count(const MapConfigValue::key_type& key) const override;
    virtual MapConfigValue::const_iterator find(const MapConfigValue::key_type& key) const
    override;

    virtual AbstractConfigValuePtr attempt_peek_with_partial_resolve(const std::string& key)
    override;

private:
    VectorAbstractConfigValue stack;
};

class ConfigDelayedMergeObjectResolveReplacer : public virtual ResolveReplacer,
    public ConfigBase {
public:
    CONFIG_CLASS(ConfigDelayedMergeObjectResolveReplacer);

    ConfigDelayedMergeObjectResolveReplacer(const VectorAbstractConfigValue& stack,
            uint32_t skipping);

protected:
    virtual AbstractConfigValuePtr make_replacement(const ResolveContextPtr& context) override;

private:
    const VectorAbstractConfigValue& stack;
    uint32_t skipping;
};

}

#endif // CONFIG_CONFIG_DELAYED_MERGE_OBJECT_HPP
