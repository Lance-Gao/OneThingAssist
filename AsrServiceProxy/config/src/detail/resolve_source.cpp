
#include "detail/resolve_source.hpp"
#include "detail/resolve_context.hpp"
#include "detail/resolve_replacer.hpp"
#include "detail/abstract_config_object.hpp"
#include "detail/substitution_expression.hpp"
#include "detail/path.hpp"
#include "detail/config_impl.hpp"
#include "config_resolve_options.hpp"

namespace config {

ResolveSource::ResolveSource(const AbstractConfigObjectPtr& root) :
    _root(root) {
}

AbstractConfigValuePtr ResolveSource::find_in_object(const AbstractConfigObjectPtr& obj,
        const ResolveContextPtr& context, const SubstitutionExpressionPtr& subst) {
    return obj->peek_path(subst->path(), context);
}

AbstractConfigValuePtr ResolveSource::lookup_subst(const ResolveContextPtr& context,
        const SubstitutionExpressionPtr& subst, uint32_t prefix_length) {
    context->trace(subst);
    ConfigExceptionPtr finally;
    AbstractConfigValuePtr result;

    try {
        // _first we look up the full path, which means relative to the
        // included file if we were not a root file
        result = find_in_object(_root, context, subst);

        if (!result) {
            // _then we want to check relative to the root file. _we don't
            // want the prefix we were included at to be used when looking
            // up env variables either.
            auto unprefixed = subst->change_path(subst->path()->sub_path(prefix_length));

            // replace the debug trace path
            context->untrace();
            context->trace(unprefixed);

            if (prefix_length > 0) {
                result = find_in_object(_root, context, unprefixed);
            }

            if (!result && context->options()->get_use_system_environment()) {
                result = find_in_object(ConfigImpl::env_variables_as_config_object(), context, unprefixed);
            }
        }

        if (result) {
            result = context->resolve(result);
        }
    } catch (ConfigException& e) {
        finally = e.clone();
    }

    context->untrace();

    if (finally) {
        finally->raise();
    }

    return result;
}

void ResolveSource::replace(const AbstractConfigValuePtr& value,
                              const ResolveReplacerPtr& replacer) {
    auto old = _replacements.insert(std::make_pair(value, replacer));

    if (!old.second) {
        throw ConfigExceptionBugOrBroken("should not have replaced the same value twice: " +
                                              value->to_string());
    }
}

void ResolveSource::unreplace(const AbstractConfigValuePtr& value) {
    if (_replacements.erase(value) == 0) {
        throw ConfigExceptionBugOrBroken("unreplace() without replace(): " + value->to_string());
    }
}

AbstractConfigValuePtr ResolveSource::replacement(const ResolveContextPtr& context,
        const AbstractConfigValuePtr& value) {
    auto replacer = _replacements.find(value);

    if (replacer == _replacements.end()) {
        return value;
    } else {
        return replacer->second->replace(context);
    }
}

AbstractConfigValuePtr ResolveSource::resolve_checking_replacement(
    const ResolveContextPtr& context,
    const AbstractConfigValuePtr& original) {
    auto replacement_ = replacement(context, original);

    if (replacement_ != original) {
        // start over, checking if replacement was memoized
        return context->resolve(replacement_);
    } else {
        auto resolved = original->resolve_substitutions(context);
        return resolved;
    }
}

}
