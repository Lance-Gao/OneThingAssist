
#include "detail/resolve_replacer.hpp"
#include "detail/abstract_config_value.hpp"

namespace config {

AbstractConfigValuePtr ResolveReplacer::replace(const ResolveContextPtr& context) {
    if (!replacement) {
        replacement = make_replacement(context);
    }

    return replacement;
}

ResolveReplacerPtr ResolveReplacer::cycle_resolve_replacer() {
    return ResolveReplacerCycle::make_instance();
}

AbstractConfigValuePtr ResolveReplacerCycle::make_replacement(const ResolveContextPtr&
        context) {
    throw NotPossibleToResolve(context);
}

}
