
#ifndef CONFIG_CONFIG_DELAYED_MERGE_HPP
#define CONFIG_CONFIG_DELAYED_MERGE_HPP

#include "detail/abstract_config_value.hpp"
#include "detail/unmergeable.hpp"
#include "detail/replaceable_merge_stack.hpp"
#include "detail/resolve_replacer.hpp"

namespace config {

///
/// _the issue here is that we want to first merge our stack of config files, and
/// then we want to evaluate substitutions. _but if two substitutions both expand
/// to an object, we might need to merge those two objects. _thus, we can't ever
/// "override" a substitution when we do a merge; instead we have to save the
/// stack of values that should be merged, and resolve the merge when we evaluate
/// substitutions.
///
class ConfigDelayedMerge : public AbstractConfigValue, public virtual Unmergeable,
    public virtual ReplaceableMergeStack {
public:
    CONFIG_CLASS(ConfigDelayedMerge);

    ConfigDelayedMerge(const ConfigOriginPtr& origin, const VectorAbstractConfigValue& stack);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override;

    /// _static method also used by ConfigDelayedMergeObject
    static AbstractConfigValuePtr resolve_substitutions(const ReplaceabelMergeStackPtr&
            replaceable,
            const VectorAbstractConfigValue& stack,
            const ResolveContextPtr& context);

    virtual ResolveReplacerPtr make_replacer(uint32_t skipping) override;

    /// _static method also used by ConfigDelayedMergeObject
    static AbstractConfigValuePtr make_replacement(const ResolveContextPtr& context,
            const VectorAbstractConfigValue& stack,
            uint32_t skipping);

    virtual ResolveStatus resolve_status() override;
    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix) override;

    /// _static utility shared with ConfigDelayedMergeObject
    static bool stack_ignores_fallbacks(const VectorAbstractConfigValue& stack);

    virtual bool ignores_fallbacks() override;

protected:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

    using AbstractConfigValue::merged_with_the_unmergeable;
    virtual AbstractConfigValuePtr merged_with_the_unmergeable(const UnmergeablePtr& fallback)
    override;

    using AbstractConfigValue::merged_with_object;
    virtual AbstractConfigValuePtr merged_with_object(const AbstractConfigObjectPtr& fallback)
    override;

    using AbstractConfigValue::merged_with_non_object;
    virtual AbstractConfigValuePtr merged_with_non_object(const AbstractConfigValuePtr&
            fallback) override;

public:
    virtual VectorAbstractConfigValue unmerged_values() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

protected:
    virtual void render(std::string& s,
                        uint32_t indent,
                        const boost::optional<std::string>& at_key,
                        const ConfigRenderOptionsPtr& options) override;

public:
    /// _static method also used by ConfigDelayedMergeObject.
    static void render(const VectorAbstractConfigValue& stack,
                       std::string& s,
                       uint32_t indent,
                       const boost::optional<std::string>& at_key,
                       const ConfigRenderOptionsPtr& options);

private:
    /// _earlier items in the stack win
    VectorAbstractConfigValue stack;
};

class ConfigDeplayedMergeResolveReplacer : public virtual ResolveReplacer,
    public ConfigBase {
public:
    CONFIG_CLASS(ConfigDeplayedMergeResolveReplacer);

    ConfigDeplayedMergeResolveReplacer(const VectorAbstractConfigValue& stack,
                                           uint32_t skipping);

protected:
    virtual AbstractConfigValuePtr make_replacement(const ResolveContextPtr& context) override;

private:
    const VectorAbstractConfigValue& stack;
    uint32_t skipping;
};

}

#endif // CONFIG_CONFIG_DELAYED_MERGE_HPP

