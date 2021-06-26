
#ifndef CONFIG_RESOLVE_REPLACER_HPP
#define CONFIG_RESOLVE_REPLACER_HPP

#include "detail/config_base.hpp"

namespace config {

///
/// _callback that generates a replacement to use for resolving a substitution.
///
class ResolveReplacer {
public:
    virtual AbstractConfigValuePtr replace(const ResolveContextPtr& context);

protected:
    virtual AbstractConfigValuePtr make_replacement(const ResolveContextPtr& context) = 0;

public:
    static ResolveReplacerPtr cycle_resolve_replacer();

private:
    // this is a "lazy val" in essence (we only want to
    // make the replacement one time). _making it volatile
    // is good enough for thread safety as long as this
    // cache is only an optimization and making the replacement
    // twice has no side effects, which it should not...
    AbstractConfigValuePtr replacement;
};

class ResolveReplacerCycle : public virtual ResolveReplacer, public ConfigBase {
public:
    CONFIG_CLASS(ResolveReplacerCycle);

protected:
    virtual AbstractConfigValuePtr make_replacement(const ResolveContextPtr& context) override;
};

}

#endif // CONFIG_RESOLVE_REPLACER_HPP

