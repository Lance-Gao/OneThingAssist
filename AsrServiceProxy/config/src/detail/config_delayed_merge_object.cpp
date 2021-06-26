
#include "detail/config_delayed_merge_object.hpp"
#include "detail/config_delayed_merge.hpp"
#include "config_exception.hpp"
#include "config_origin.hpp"
#include "config_list.hpp"

namespace config {

ConfigDelayedMergeObject::ConfigDelayedMergeObject(const ConfigOriginPtr& origin,
        const VectorAbstractConfigValue& stack) :
    AbstractConfigObject(origin),
    stack(stack) {

    if (stack.empty()) {
        throw ConfigExceptionBugOrBroken("creating empty delayed merge object");
    }

    if (!instanceof<AbstractConfigObject>(stack.front())) {
        throw ConfigExceptionBugOrBroken(
            "created a delayed merge object not guaranteed to be an object");
    }

    for (auto& v : stack) {
        if (instanceof<ConfigDelayedMerge>(v) || instanceof<ConfigDelayedMergeObject>(v)) {
            throw ConfigExceptionBugOrBroken(
                "placed nested _delayed_merge in a ConfigDelayedMergeObject, should have consolidated stack");
        }
    }
}

AbstractConfigObjectPtr ConfigDelayedMergeObject::new_copy(ResolveStatus status,
        const ConfigOriginPtr& origin) {
    if (status != resolve_status()) {
        throw ConfigExceptionBugOrBroken(
            "attempt to create resolved ConfigDelayedMergeObject");
    }

    return make_instance(origin, stack);
}

AbstractConfigValuePtr ConfigDelayedMergeObject::resolve_substitutions(
    const ResolveContextPtr& context) {
    auto merged = ConfigDelayedMerge::resolve_substitutions(shared_from_this(), stack, context);

    if (instanceof<AbstractConfigObject>(merged)) {
        return std::static_pointer_cast<AbstractConfigObject>(merged);
    } else {
        throw ConfigExceptionBugOrBroken("somehow brokenly merged an object and didn't get an object, got "
                                              + merged->to_string());
    }
}

ResolveReplacerPtr ConfigDelayedMergeObject::make_replacer(uint32_t skipping) {
    return ConfigDelayedMergeObjectResolveReplacer::make_instance(stack, skipping);
}

ConfigDelayedMergeObjectResolveReplacer::ConfigDelayedMergeObjectResolveReplacer(
    const VectorAbstractConfigValue& stack, uint32_t skipping) :
    stack(stack),
    skipping(skipping) {
}

AbstractConfigValuePtr ConfigDelayedMergeObjectResolveReplacer::make_replacement(
    const ResolveContextPtr& context) {
    return ConfigDelayedMerge::make_replacement(context, stack, skipping);
}

ResolveStatus ConfigDelayedMergeObject::resolve_status() {
    return ResolveStatus::UNRESOLVED;
}

AbstractConfigValuePtr ConfigDelayedMergeObject::relativized(const PathPtr& prefix) {
    VectorAbstractConfigValue new_stack;

    for (auto& o : stack) {
        new_stack.push_back(o->relativized(prefix));
    }

    return make_instance(origin(), new_stack);
}

bool ConfigDelayedMergeObject::ignores_fallbacks() {
    return ConfigDelayedMerge::stack_ignores_fallbacks(stack);
}

AbstractConfigValuePtr ConfigDelayedMergeObject::merged_with_the_unmergeable(
    const UnmergeablePtr& fallback) {
    return std::static_pointer_cast<ConfigDelayedMergeObject>(merged_with_the_unmergeable(stack,
            fallback));
}

AbstractConfigValuePtr ConfigDelayedMergeObject::merged_with_object(
    const AbstractConfigObjectPtr&
    fallback) {
    return merged_with_non_object(fallback);
}

AbstractConfigValuePtr ConfigDelayedMergeObject::merged_with_non_object(
    const AbstractConfigValuePtr&
    fallback) {
    return std::static_pointer_cast<ConfigDelayedMergeObject>(merged_with_non_object(stack,
            fallback));
}

ConfigMergeablePtr ConfigDelayedMergeObject::with_fallback(const ConfigMergeablePtr&
        mergeable) {
    return AbstractConfigObject::with_fallback(mergeable);
}

ConfigObjectPtr ConfigDelayedMergeObject::with_only_key(const std::string& key) {
    throw not_resolved();
}

ConfigObjectPtr ConfigDelayedMergeObject::without_key(const std::string& key) {
    throw not_resolved();
}

AbstractConfigObjectPtr ConfigDelayedMergeObject::with_only_path_or_null(
    const PathPtr& path) {
    throw not_resolved();
}

AbstractConfigObjectPtr ConfigDelayedMergeObject::with_only_path(const PathPtr& path) {
    throw not_resolved();
}

AbstractConfigObjectPtr ConfigDelayedMergeObject::without_path(const PathPtr& path) {
    throw not_resolved();
}

ConfigObjectPtr ConfigDelayedMergeObject::with_value(const std::string& key,
        const ConfigValuePtr& value) {
    throw not_resolved();
}

ConfigObjectPtr ConfigDelayedMergeObject::with_value(const PathPtr& path,
        const ConfigValuePtr& value) {
    throw not_resolved();
}

VectorAbstractConfigValue ConfigDelayedMergeObject::unmerged_values() {
    return stack;
}

bool ConfigDelayedMergeObject::can_equal(const ConfigVariant& other) {
    return instanceof<ConfigDelayedMergeObject>(other);
}

bool ConfigDelayedMergeObject::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<ConfigDelayedMergeObject>(other)) {
        return can_equal(other) &&
               this->stack.size() == dynamic_get<ConfigDelayedMergeObject>(other)->stack.size() &&
               std::equal(this->stack.begin(), this->stack.end(),
                          dynamic_get<ConfigDelayedMergeObject>(other)->stack.begin(),
                          config_equals<AbstractConfigValuePtr>());
    } else {
        return false;
    }
}

uint32_t ConfigDelayedMergeObject::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    size_t hash = 0;

    for (auto& v : stack) {
        boost::hash_combine(hash, v->hash_code());
    }

    return static_cast<uint32_t>(hash);
}

void ConfigDelayedMergeObject::render(std::string& s, uint32_t indent,
        const boost::optional<std::string>& at_key, const ConfigRenderOptionsPtr& options) {
    ConfigDelayedMerge::render(stack, s, indent, at_key, options);
}

void ConfigDelayedMergeObject::render(std::string& s, uint32_t indent,
        const ConfigRenderOptionsPtr& options) {
    render(s, indent, nullptr, options);
}

ConfigExceptionNotResolved ConfigDelayedMergeObject::not_resolved() {
    return ConfigExceptionNotResolved("need to Config#resolve() before using this object, see the API docs for Config::resolve()");
}

ConfigVariant ConfigDelayedMergeObject::unwrapped() {
    throw not_resolved();
}

ConfigValuePtr ConfigDelayedMergeObject::get(const std::string& key) {
    throw not_resolved();
}

MapConfigValue::const_iterator ConfigDelayedMergeObject::begin() const {
    throw not_resolved();
}

MapConfigValue::const_iterator ConfigDelayedMergeObject::end() const {
    throw not_resolved();
}

MapConfigValue::mapped_type ConfigDelayedMergeObject::operator[](
    const MapConfigValue::key_type&
    key) const {
    throw not_resolved();
}

bool ConfigDelayedMergeObject::empty() const {
    throw not_resolved();
}

MapConfigValue::size_type ConfigDelayedMergeObject::size() const {
    throw not_resolved();
}

MapConfigValue::size_type ConfigDelayedMergeObject::count(const MapConfigValue::key_type&
        key)
const {
    throw not_resolved();
}

MapConfigValue::const_iterator ConfigDelayedMergeObject::find(const
        MapConfigValue::key_type& key)
const {
    throw not_resolved();
}

AbstractConfigValuePtr ConfigDelayedMergeObject::attempt_peek_with_partial_resolve(
    const std::string& key) {
    // a partial resolve of a ConfigDelayedMergeObject always results in a
    // SimpleConfigObject because all the substitutions in the stack get
    // resolved in order to look up the partial.
    // _so we know here that we have not been resolved at all even
    // partially.
    // _given that, all this code is probably gratuitous, since the app code
    // is likely broken. _but in general we only throw _not_resolved if you try
    // to touch the exact key that isn't resolved, so this is in that
    // spirit.

    // we'll be able to return a key if we have a value that ignores
    // fallbacks, prior to any unmergeable values.
    for (auto& layer : stack) {
        if (instanceof<AbstractConfigObject>(layer)) {
            auto object_layer = std::static_pointer_cast<AbstractConfigObject>(layer);
            auto v = object_layer->attempt_peek_with_partial_resolve(key);

            if (v) {
                if (v->ignores_fallbacks()) {
                    // we know we won't need to merge anything in to this value
                    return v;
                } else {
                    // we can't return this value because we know there are
                    // unmergeable values later in the stack that may
                    // contain values that need to be merged with this
                    // value. we'll throw the exception when we get to those
                    // unmergeable values, so continue here.
                    continue;
                }
            } else if (instanceof<Unmergeable>(layer)) {
                // an unmergeable object (which would be another
                // ConfigDelayedMergeObject) can't know that a key is
                // missing, so it can't return null; it can only return a
                // value or throw NotPossibleToResolve
                throw ConfigExceptionBugOrBroken("should not be reached: unmergeable object returned null value");
            } else {
                // a non-unmergeable AbstractConfigObject that returned null
                // for the key in question is not relevant, we can keep
                // looking for a value.
                continue;
            }
        } else if (instanceof<Unmergeable>(layer)) {
            throw ConfigExceptionNotResolved("_key '" + key + "' is not available at '" +
                                                 origin()->description() + "' because value at '" +
                                                 layer->origin()->description() +
                                                 "' has not been resolved and may turn out to contain or hide '" +
                                                 key + "'." +
                                                 " _be sure to Config#resolve() before using a config object.");
        } else if (layer->resolve_status() == ResolveStatus::UNRESOLVED) {
            // if the layer is not an object, and not a substitution or merge,
            // then it's something that's unresolved because it _contains_
            // an unresolved object... i.e. it's an array
            if (!instanceof< ConfigList>(layer)) {
                throw ConfigExceptionBugOrBroken("_expecting a list here, not " + layer->to_string());
            }

            // all later objects will be hidden so we can say we won't find the key
            return nullptr;
        } else {
            // non-object, but resolved, like an integer or something.
            // has no children so the one we're after won't be in it.
            // we would only have this in the stack in case something
            // else "looks back" to it due to a cycle.
            // anyway at this point we know we can't find the key anymore.
            if (!layer->ignores_fallbacks()) {
                throw ConfigExceptionBugOrBroken("resolved non-object should ignore fallbacks");
            }

            return nullptr;
        }
    }

    // _if we get here, then we never found anything unresolved which means
    // the ConfigDelayedMergeObject should not have existed. some
    // invariant was violated.
    throw ConfigExceptionBugOrBroken("_delayed merge stack does not contain any unmergeable values");
}

}
