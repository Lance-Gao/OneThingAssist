
#include "detail/abstract_config_object.hpp"
#include "detail/simple_config.hpp"
#include "detail/config_impl.hpp"
#include "detail/resolve_context.hpp"
#include "detail/path.hpp"
#include "detail/config_delayed_merge_object.hpp"
#include "detail/simple_config_origin.hpp"
#include "config_exception.hpp"
#include "config_value_type.hpp"

namespace config {

AbstractConfigObject::AbstractConfigObject(const ConfigOriginPtr& origin) :
    AbstractConfigValue(origin) {
}

void AbstractConfigObject::initialize() {
    config = SimpleConfig::make_instance(shared_from_this());
}

ConfigPtr AbstractConfigObject::to_config() {
    return config;
}

ConfigValuePtr AbstractConfigObject::to_fallback_value() {
    return shared_from_this();
}

AbstractConfigValuePtr AbstractConfigObject::peek_assuming_resolved(const std::string& key,
        const PathPtr& original_path) {
    try {
        return attempt_peek_with_partial_resolve(key);
    } catch (ConfigExceptionNotResolved& e) {
        throw ConfigImpl::improve_not_resolved(original_path, e);
    }
}

AbstractConfigValuePtr AbstractConfigObject::peek_path(const PathPtr& path,
        const ResolveContextPtr& context) {
    return peek_path(shared_from_this(), path, context);
}

AbstractConfigValuePtr AbstractConfigObject::peek_path(const PathPtr& path) {
    try {
        return peek_path(shared_from_this(), path, nullptr);
    } catch (NotPossibleToResolve&) {
        throw ConfigExceptionBugOrBroken("NotPossibleToResolve happened though we had no ResolveContext in peek_path");
    }
}

AbstractConfigValuePtr AbstractConfigObject::peek_path(const AbstractConfigObjectPtr&
        self,
        const PathPtr& path, const ResolveContextPtr& context) {
    try {
        if (context) {
            // walk down through the path resolving only things along that
            // path, and then recursively call ourselves with no resolve
            // context.
            auto partially_resolved = context->restrict(path)->resolve(self);

            if (instanceof<AbstractConfigObject>(partially_resolved)) {
                return peek_path(std::static_pointer_cast<AbstractConfigObject>(partially_resolved), path,
                                 nullptr);
            } else {
                throw ConfigExceptionBugOrBroken("resolved object to non-object " + self->to_string() + " to "
                                                      +
                                                      partially_resolved->to_string());
            }
        } else {
            // with no resolver, we'll fail if anything along the path can't
            // be looked at without resolving.
            auto next = path->remainder();
            auto v = self->attempt_peek_with_partial_resolve(path->first());

            if (!next) {
                return v;
            } else {
                if (instanceof<AbstractConfigObject>(v)) {
                    return peek_path(std::static_pointer_cast<AbstractConfigObject>(v), next, nullptr);
                } else {
                    return nullptr;
                }
            }
        }
    } catch (ConfigExceptionNotResolved& e) {
        throw ConfigImpl::improve_not_resolved(path, e);
    }
}

ConfigValueType AbstractConfigObject::value_type() {
    return ConfigValueType::OBJECT;
}

AbstractConfigValuePtr AbstractConfigObject::new_copy(const ConfigOriginPtr& origin) {
    return new_copy(resolve_status(), origin);
}

AbstractConfigValuePtr AbstractConfigObject::construct_delayed_merge(
    const ConfigOriginPtr& origin,
    const VectorAbstractConfigValue& stack) {
    return ConfigDelayedMergeObject::make_instance(origin, stack);
}

ConfigMergeablePtr AbstractConfigObject::with_fallback(const ConfigMergeablePtr&
        mergeable) {
    return AbstractConfigValue::with_fallback(mergeable);
}

ConfigOriginPtr AbstractConfigObject::merge_origins(const VectorAbstractConfigValue&
        stack) {
    if (stack.empty()) {
        throw ConfigExceptionBugOrBroken("can't merge origins on empty list");
    }

    VectorConfigOrigin origins;
    ConfigOriginPtr first_origin;
    uint32_t num_merged = 0;

    for (auto& v : stack) {
        if (!first_origin) {
            first_origin = v->origin();
        }

        if (instanceof<AbstractConfigObject>(v) &&
                std::static_pointer_cast<AbstractConfigObject>(v)->resolve_status() == ResolveStatus::RESOLVED
                &&
                std::dynamic_pointer_cast<ConfigObject>(v)->empty()) {
            // don't include empty files or the .empty()
            // config in the description, since they are
            // likely to be "implementation details"
        } else {
            origins.push_back(v->origin());
            num_merged += 1;
        }
    }

    if (num_merged == 0) {
        // the configs were all empty, so just use the first one
        origins.push_back(first_origin);
    }

    return SimpleConfigOrigin::merge_origins(origins);
}

ConfigOriginPtr AbstractConfigObject::merge_origins(const VectorAbstractConfigObject&
        stack) {
    return merge_origins(VectorAbstractConfigValue(stack.begin(), stack.end()));
}

MapConfigValue::const_iterator AbstractConfigObject::begin() const {
    return ConfigObject::begin();
}

MapConfigValue::const_iterator AbstractConfigObject::end() const {
    return ConfigObject::end();
}

MapConfigValue::mapped_type AbstractConfigObject::operator[](const
        MapConfigValue::key_type& key)
const {
    auto val = ConfigObject::find(key);
    return val == ConfigObject::end() ? nullptr : val->second;
}

bool AbstractConfigObject::empty() const {
    return ConfigObject::empty();
}

MapConfigValue::size_type AbstractConfigObject::size() const {
    return ConfigObject::size();
}

MapConfigValue::size_type AbstractConfigObject::count(const MapConfigValue::key_type& key)
const {
    return ConfigObject::count(key);
}

MapConfigValue::const_iterator AbstractConfigObject::find(const MapConfigValue::key_type&
        key)
const {
    return ConfigObject::find(key);
}

ConfigExceptionUnsupportedOperation AbstractConfigObject::we_are_immutable(
    const std::string& method) {
    return ConfigExceptionUnsupportedOperation("ConfigObject is immutable, you can't call _map." +
            method);
}

void AbstractConfigObject::clear() {
    throw we_are_immutable("clear");
}

MapConfigValue::size_type AbstractConfigObject::erase(const MapConfigValue::key_type&) {
    throw we_are_immutable("erase");
}

void AbstractConfigObject::erase(const MapConfigValue::const_iterator&) {
    throw we_are_immutable("erase");
}

void AbstractConfigObject::insert(const MapConfigValue::value_type&) {
    throw we_are_immutable("insert");
}

}
