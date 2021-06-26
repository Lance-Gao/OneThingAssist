
#include "detail/config_delayed_merge.hpp"
#include "detail/config_delayed_merge_object.hpp"
#include "detail/resolve_context.hpp"
#include "detail/resolve_source.hpp"
#include "detail/config_impl_util.hpp"
#include "config_render_options.hpp"
#include "config_origin.hpp"
#include "config_exception.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigDelayedMerge::ConfigDelayedMerge(const ConfigOriginPtr& origin,
        const VectorAbstractConfigValue& stack) :
    AbstractConfigValue(origin),
    stack(stack) {
    if (stack.empty()) {
        throw ConfigExceptionBugOrBroken("creating empty delayed merge value");
    }

    for (auto& v : stack) {
        if (instanceof<ConfigDelayedMerge>(v) || instanceof<ConfigDelayedMergeObject>(v)) {
            throw ConfigExceptionBugOrBroken("placed nested _delayed_merge in a ConfigDelayedMerge, should have consolidated stack");
        }
    }
}

ConfigValueType ConfigDelayedMerge::value_type() {
    throw ConfigExceptionNotResolved("called value_type() on value with unresolved substitutions, need to _config::resolve() first, see API docs");
}

ConfigVariant ConfigDelayedMerge::unwrapped() {
    throw ConfigExceptionNotResolved("called unwrapped() on value with unresolved substitutions, need to _config::resolve() first, see API docs");
}

AbstractConfigValuePtr ConfigDelayedMerge::resolve_substitutions(
    const ResolveContextPtr& context) {
    return resolve_substitutions(shared_from_this(), stack, context);
}

AbstractConfigValuePtr ConfigDelayedMerge::resolve_substitutions(
    const ReplaceabelMergeStackPtr&
    replaceable, const VectorAbstractConfigValue& stack, const ResolveContextPtr& context) {
    // to resolve substitutions, we need to recursively resolve
    // the stack of stuff to merge, and merge the stack so
    // we won't be a delayed merge anymore. _if restrict_to_child_or_null
    // is non-null, we may remain a delayed merge though.

    uint32_t count = 0;
    AbstractConfigValuePtr merged;

    for (auto& v : stack) {
        if (instanceof<ReplaceableMergeStack>(v)) {
            throw ConfigExceptionBugOrBroken("_a delayed merge should not contain another one: " +
                                                  boost::lexical_cast<std::string>(replaceable.get()));
        }

        bool replaced = false;

        // we only replace if we have a substitution, or
        // value-concatenation containing one. The Unmergeable
        // here isn't a delayed merge stack since we can't contain
        // another stack (see assertion above).
        if (instanceof<Unmergeable>(v)) {
            // _if, while resolving 'v' we come back to the same
            // merge stack, we only want to look _below_ 'v'
            // in the stack. _so we arrange to replace the
            // ConfigDelayedMerge with a value that is only
            // the remainder of the stack below this one.
            context->source()->replace(std::dynamic_pointer_cast<AbstractConfigValue>(replaceable),
                                       replaceable->make_replacer(count + 1));
            replaced = true;
        }

        AbstractConfigValuePtr resolved;
        ConfigExceptionPtr finally;

        try {
            resolved = context->resolve(v);
        } catch (ConfigException& e) {
            finally = e.clone();
        }

        if (replaced) {
            context->source()->unreplace(std::dynamic_pointer_cast<AbstractConfigValue>(replaceable));
        }

        if (finally) {
            finally->raise();
        }

        if (resolved) {
            if (!merged) {
                merged = resolved;
            } else {
                merged = std::dynamic_pointer_cast<AbstractConfigValue>(merged->with_fallback(resolved));
            }
        }

        count += 1;
    }

    return merged;
}

ResolveReplacerPtr ConfigDelayedMerge::make_replacer(uint32_t skipping) {
    return ConfigDeplayedMergeResolveReplacer::make_instance(stack, skipping);
}

ConfigDeplayedMergeResolveReplacer::ConfigDeplayedMergeResolveReplacer(const
        VectorAbstractConfigValue& stack, uint32_t skipping) :
    stack(stack),
    skipping(skipping) {
}

AbstractConfigValuePtr ConfigDeplayedMergeResolveReplacer::make_replacement(
    const ResolveContextPtr& context) {
    return ConfigDelayedMerge::make_replacement(context, stack, skipping);
}

AbstractConfigValuePtr ConfigDelayedMerge::make_replacement(const ResolveContextPtr&
        context,
        const VectorAbstractConfigValue& stack, uint32_t skipping) {
    VectorAbstractConfigValue sub_stack(stack.begin() + skipping, stack.end());

    if (sub_stack.empty()) {
        throw NotPossibleToResolve(context);
    } else {
        // generate a new merge stack from only the remaining items
        AbstractConfigValuePtr merged;

        for (auto& v : sub_stack) {
            if (!merged) {
                merged = v;
            } else {
                merged = std::dynamic_pointer_cast<AbstractConfigValue>(merged->with_fallback(v));
            }
        }

        return merged;
    }
}

ResolveStatus ConfigDelayedMerge::resolve_status() {
    return ResolveStatus::UNRESOLVED;
}

AbstractConfigValuePtr ConfigDelayedMerge::relativized(const PathPtr& prefix) {
    VectorAbstractConfigValue new_stack;

    for (auto& o : stack) {
        new_stack.push_back(o->relativized(prefix));
    }

    return make_instance(origin(), new_stack);
}

bool ConfigDelayedMerge::stack_ignores_fallbacks(const VectorAbstractConfigValue& stack) {
    auto last = stack.back();
    return last->ignores_fallbacks();
}

bool ConfigDelayedMerge::ignores_fallbacks() {
    return stack_ignores_fallbacks(stack);
}

AbstractConfigValuePtr ConfigDelayedMerge::new_copy(const ConfigOriginPtr& new_origin) {
    return make_instance(new_origin, stack);
}

AbstractConfigValuePtr ConfigDelayedMerge::merged_with_the_unmergeable(const UnmergeablePtr&
        fallback) {
    return merged_with_the_unmergeable(stack, fallback);
}

AbstractConfigValuePtr ConfigDelayedMerge::merged_with_object(const
        AbstractConfigObjectPtr&
        fallback) {
    return merged_with_object(stack, fallback);
}

AbstractConfigValuePtr ConfigDelayedMerge::merged_with_non_object(
    const AbstractConfigValuePtr&
    fallback) {
    return merged_with_non_object(stack, fallback);
}

VectorAbstractConfigValue ConfigDelayedMerge::unmerged_values() {
    return stack;
}

bool ConfigDelayedMerge::can_equal(const ConfigVariant& other) {
    return instanceof<ConfigDelayedMerge>(other);
}

bool ConfigDelayedMerge::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<ConfigDelayedMerge>(other)) {
        return can_equal(other) &&
               this->stack.size() == dynamic_get<ConfigDelayedMerge>(other)->stack.size() &&
               std::equal(this->stack.begin(), this->stack.end(),
                          dynamic_get<ConfigDelayedMerge>(other)->stack.begin(),
                          config_equals<AbstractConfigValuePtr>());
    } else {
        return false;
    }
}

uint32_t ConfigDelayedMerge::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    size_t hash = 0;

    for (auto& v : stack) {
        boost::hash_combine(hash, v->hash_code());
    }

    return static_cast<uint32_t>(hash);
}

void ConfigDelayedMerge::render(std::string& s, uint32_t indent,
                                   const boost::optional<std::string>& at_key, const ConfigRenderOptionsPtr& options) {
    render(stack, s, indent, at_key, options);
}

void ConfigDelayedMerge::render(const VectorAbstractConfigValue& stack, std::string& s,
                                   uint32_t indent_, const boost::optional<std::string>& at_key,
                                   const ConfigRenderOptionsPtr& options) {
    bool comment_merge = options->get_comments();

    if (comment_merge) {
        s += "# unresolved merge of " + boost::lexical_cast<std::string>(stack.size()) +
             " values follows (\n";

        if (!at_key) {
            indent(s, indent_, options);
            s += "# this unresolved merge will not be parseable because it's at the root of the object\n";
            indent(s, indent_, options);
            s += "# the HOCON format has no way to list multiple root objects in a single file\n";
        }
    }

    VectorAbstractConfigValue reversed(stack.rbegin(), stack.rend());

    uint32_t i = 0;

    for (auto& v : reversed) {
        if (comment_merge) {
            indent(s, indent_, options);

            if (at_key) {
                s += "#     unmerged value " + boost::lexical_cast<std::string>(i) + " for key " +
                     ConfigImplUtil::render_json_string(*at_key) + " from ";
            } else {
                s += "#     unmerged value " + boost::lexical_cast<std::string>(i) + " from ";
            }

            i += 1;
            s += v->origin()->description() + "\n";

            for (auto& comment : v->origin()->comments()) {
                indent(s, indent_, options);
                s += "# " + comment + "\n";
            }
        }

        indent(s, indent_, options);

        if (at_key) {
            s += ConfigImplUtil::render_json_string(*at_key);

            if (options->get_formatted()) {
                s += " : ";
            } else {
                s += ":";
            }
        }

        v->render(s, indent_, options);
        s += ",";

        if (options->get_formatted()) {
            s += "\n";
        }
    }

    // chop comma or newline
    s.resize(s.length() - 1);

    if (options->get_formatted()) {
        s.resize(s.length() - 1); // also chop comma
        s += "\n"; // put a newline back
    }

    if (comment_merge) {
        indent(s, indent_, options);
        s += "# ) end of unresolved merge\n";
    }
}

}
