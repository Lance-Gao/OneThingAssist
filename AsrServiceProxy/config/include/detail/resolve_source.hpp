
#ifndef CONFIG_RESOLVE_SOURCE_HPP
#define CONFIG_RESOLVE_SOURCE_HPP

#include "detail/config_base.hpp"

namespace config {

///
/// _this class is the source for values for a substitution like ${foo}.
///
class ResolveSource : public ConfigBase {
public:
    CONFIG_CLASS(ResolveSource);

    ResolveSource(const AbstractConfigObjectPtr& root);

private:
    static AbstractConfigValuePtr find_in_object(const AbstractConfigObjectPtr& obj,
            const ResolveContextPtr& context,
            const SubstitutionExpressionPtr& subst);

public:
    AbstractConfigValuePtr lookup_subst(const ResolveContextPtr& context,
                                            const SubstitutionExpressionPtr& subst,
                                            uint32_t prefix_length);

    void replace(const AbstractConfigValuePtr& value, const ResolveReplacerPtr& replacer);
    void unreplace(const AbstractConfigValuePtr& value);

private:
    AbstractConfigValuePtr replacement(const ResolveContextPtr& context,
                                           const AbstractConfigValuePtr& value);

public:
    /// _conceptually, this is key.value().resolve_substitutions() but using the
    /// replacement for key.value() if any.
    AbstractConfigValuePtr resolve_checking_replacement(const ResolveContextPtr& context,
            const AbstractConfigValuePtr& original);

private:
    AbstractConfigObjectPtr _root;

    // _conceptually, we transform the ResolveSource whenever we traverse
    // a substitution or delayed merge stack, in order to remove the
    // traversed node and therefore avoid circular dependencies.
    // _we implement it with this somewhat hacky "patch a replacement"
    // mechanism instead of actually transforming the tree.
    MapResolveReplacer _replacements;
};

}

#endif // CONFIG_RESOLVE_SOURCE_HPP

