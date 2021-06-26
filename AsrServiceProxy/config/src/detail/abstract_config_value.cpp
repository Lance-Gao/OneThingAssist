
#include "detail/abstract_config_value.hpp"
#include "detail/abstract_config_object.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/resolve_context.hpp"
#include "detail/resolve_status.hpp"
#include "detail/config_delayed_merge.hpp"
#include "detail/mergeable_value.hpp"
#include "detail/unmergeable.hpp"
#include "detail/config_impl_util.hpp"
#include "detail/simple_config_object.hpp"
#include "detail/simple_config.hpp"
#include "detail/path.hpp"
#include "config_render_options.hpp"
#include "config_origin.hpp"

namespace config {

NotPossibleToResolve::NotPossibleToResolve(const ResolveContextPtr& context) :
    ConfigException("was not possible to resolve"),
    trace_string_(context->trace_string()) {
}

std::string NotPossibleToResolve::trace_string() {
    return trace_string_;
}

AbstractConfigValuePtr NoExceptionsModifier::modify_child_may_throw(
    const std::string& key_or_null,
    const AbstractConfigValuePtr& v) {
    try {
        return modify_child(key_or_null, v);
    } catch (ConfigException& e) {
        throw;
    } catch (std::exception& e) {
        throw ConfigExceptionBugOrBroken("_something _bad happened here:" + std::string(e.what()));
    }
}

AbstractConfigValue::AbstractConfigValue(const ConfigOriginPtr& origin) :
    origin_(std::dynamic_pointer_cast<SimpleConfigOrigin>(origin)) {
}

ConfigOriginPtr AbstractConfigValue::origin() {
    return this->origin_;
}

AbstractConfigValuePtr AbstractConfigValue::resolve_substitutions(
    const ResolveContextPtr& context) {
    return shared_from_this();
}

ResolveStatus AbstractConfigValue::resolve_status() {
    return ResolveStatus::RESOLVED;
}

AbstractConfigValuePtr AbstractConfigValue::relativized(const PathPtr& prefix) {
    return shared_from_this();
}

ConfigValuePtr AbstractConfigValue::to_fallback_value() {
    return shared_from_this();
}

bool AbstractConfigValue::ignores_fallbacks() {
    // if we are not resolved, then somewhere in this value there's
    // a substitution that may need to look at the fallbacks.
    return resolve_status() == ResolveStatus::RESOLVED;
}

AbstractConfigValuePtr AbstractConfigValue::with_fallbacks_ignored() {
    if (ignores_fallbacks()) {
        return shared_from_this();
    } else {
        throw ConfigExceptionBugOrBroken("_value class doesn't "
        "implement forced fallback-ignoring " + to_string());
    }
}

void AbstractConfigValue::require_not_ignoring_fallbacks() {
    if (ignores_fallbacks()) {
        throw ConfigExceptionBugOrBroken("_method should not have been "
        "called with ignores_fallbacks=true " + get_class_name());
    }
}

AbstractConfigValuePtr AbstractConfigValue::construct_delayed_merge(
    const ConfigOriginPtr& origin,
    const VectorAbstractConfigValue& stack) {
    return ConfigDelayedMerge::make_instance(origin, stack);
}

AbstractConfigValuePtr AbstractConfigValue::merged_with_the_unmergeable(const
        VectorAbstractConfigValue& stack, const UnmergeablePtr& fallback) {
    require_not_ignoring_fallbacks();

    // if we turn out to be an object, and the fallback also does,
    // then a merge may be required; delay until we resolve.
    VectorAbstractConfigValue new_stack(stack);
    VectorAbstractConfigValue unmerged(fallback->unmerged_values());
    new_stack.insert(new_stack.end(), unmerged.begin(), unmerged.end());
    return construct_delayed_merge(AbstractConfigObject::merge_origins(new_stack), new_stack);
}

AbstractConfigValuePtr AbstractConfigValue::delay_merge(const VectorAbstractConfigValue&
        stack,
        const AbstractConfigValuePtr& fallback) {
    // if we turn out to be an object, and the fallback also does,
    // then a merge may be required.
    // if we contain a substitution, resolving it may need to look
    // back to the fallback.
    VectorAbstractConfigValue new_stack(stack);
    new_stack.push_back(fallback);
    return construct_delayed_merge(AbstractConfigObject::merge_origins(new_stack), new_stack);
}

AbstractConfigValuePtr AbstractConfigValue::merged_with_object(
    const VectorAbstractConfigValue& stack,
    const AbstractConfigObjectPtr& fallback) {
    require_not_ignoring_fallbacks();

    if (instanceof<AbstractConfigObject>(shared_from_this())) {
        throw ConfigExceptionBugOrBroken("_objects must reimplement merged_with_object");
    }

    return merged_with_non_object(stack, fallback);
}

AbstractConfigValuePtr AbstractConfigValue::merged_with_non_object(
    const VectorAbstractConfigValue&
    stack, const AbstractConfigValuePtr& fallback) {
    require_not_ignoring_fallbacks();

    if (resolve_status() == ResolveStatus::RESOLVED) {
        // falling back to a non-object doesn't merge anything, and also
        // prohibits merging any objects that we fall back to later.
        // so we have to switch to ignores_fallbacks mode.
        return with_fallbacks_ignored();
    } else {
        // if unresolved, we may have to look back to fallbacks as part of
        // the resolution process, so always delay
        return delay_merge(stack, fallback);
    }
}

AbstractConfigValuePtr AbstractConfigValue::merged_with_the_unmergeable(
    const UnmergeablePtr&
    fallback) {
    require_not_ignoring_fallbacks();

    return merged_with_the_unmergeable({shared_from_this()}, fallback);
}

AbstractConfigValuePtr AbstractConfigValue::merged_with_object(
    const AbstractConfigObjectPtr&
    fallback) {
    require_not_ignoring_fallbacks();

    return merged_with_object({shared_from_this()}, fallback);
}

AbstractConfigValuePtr AbstractConfigValue::merged_with_non_object(
    const AbstractConfigValuePtr&
    fallback) {
    require_not_ignoring_fallbacks();

    return merged_with_non_object({shared_from_this()}, fallback);
}

AbstractConfigValuePtr AbstractConfigValue::with_origin(const ConfigOriginPtr& origin) {
    if (this->origin_ == origin) {
        return shared_from_this();
    } else {
        return new_copy(origin);
    }
}

ConfigMergeablePtr AbstractConfigValue::with_fallback(const ConfigMergeablePtr&
        mergeable) {
    if (ignores_fallbacks()) {
        return std::static_pointer_cast<ConfigMergeable>(shared_from_this());
    } else {
        auto other = std::dynamic_pointer_cast<MergeableValue>(mergeable)->to_fallback_value();

        if (instanceof<Unmergeable>(other)) {
            return std::static_pointer_cast<ConfigMergeable>(merged_with_the_unmergeable(
                        std::dynamic_pointer_cast<Unmergeable>(other)));
        } else if (instanceof<AbstractConfigObject>(other)) {
            return std::static_pointer_cast<ConfigMergeable>(merged_with_object(
                        std::dynamic_pointer_cast<AbstractConfigObject>(other)));
        } else {
            return std::static_pointer_cast<ConfigMergeable>(merged_with_non_object(
                        std::dynamic_pointer_cast<AbstractConfigValue>(other)));
        }
    }
}

bool AbstractConfigValue::can_equal(const ConfigVariant& other) {
    return instanceof<ConfigValue>(other);
}

bool AbstractConfigValue::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<ConfigValue>(other)) {
        return can_equal(other) &&
               (this->value_type() == dynamic_get<ConfigValue>(other)->value_type()) &&
               ConfigImplUtil::equals_handling_null(this->unwrapped(),
                       dynamic_get<ConfigValue>(other)->unwrapped());
    } else {
        return false;
    }
}

uint32_t AbstractConfigValue::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    return std::hash<ConfigVariant>()(this->unwrapped());
}

std::string AbstractConfigValue::to_string() {
    std::string s;
    render(s, 0, nullptr, ConfigRenderOptions::concise());
    return get_class_name() + "(" + s + ")";
}

void AbstractConfigValue::indent(std::string& s, uint32_t indent,
                                    const ConfigRenderOptionsPtr& options) {
    if (options->get_formatted()) {
        uint32_t remaining = indent;

        while (remaining > 0) {
            s += "    ";
            --remaining;
        }
    }
}

void AbstractConfigValue::render(std::string& s, uint32_t indent,
                                    const boost::optional<std::string>& at_key, const ConfigRenderOptionsPtr& options) {
    if (at_key) {
        std::string rendered_key;

        if (options->get_json()) {
            rendered_key = ConfigImplUtil::render_json_string(*at_key);
        } else {
            rendered_key = ConfigImplUtil::render_string_unquoted_if_possible(*at_key);
        }

        s += rendered_key;

        if (options->get_json()) {
            if (options->get_formatted()) {
                s += " : ";
            } else {
                s += ":";
            }
        } else {
            // in non-JSON we can omit the colon or equals before an object
            if (instanceof<ConfigObject>(shared_from_this())) {
                if (options->get_formatted()) {
                    s += " ";
                }
            } else {
                s += "=";
            }
        }
    }

    render(s, indent, options);
}

void AbstractConfigValue::render(std::string& s, uint32_t indent,
                                    const ConfigRenderOptionsPtr& options) {
    ConfigVariant u = unwrapped();
    s += boost::apply_visitor(_variant_string(), u);
}

std::string AbstractConfigValue::render() {
    return render(ConfigRenderOptions::defaults());
}

std::string AbstractConfigValue::render(const ConfigRenderOptionsPtr& options) {
    std::string s;
    render(s, 0, nullptr, options);
    return s;
}

std::string AbstractConfigValue::transform_to_string() {
    return "";
}

SimpleConfigPtr AbstractConfigValue::at_key(const ConfigOriginPtr& origin,
        const std::string& key) {
    MapAbstractConfigValue m({{key, shared_from_this()}});
    return std::dynamic_pointer_cast<SimpleConfig>(SimpleConfigObject::make_instance(origin,
            m)->to_config());
}

ConfigPtr AbstractConfigValue::at_key(const std::string& key) {
    return at_key(SimpleConfigOrigin::new_simple("at_key(" + key + ")"), key);
}

SimpleConfigPtr AbstractConfigValue::at_path(const ConfigOriginPtr& origin,
        const PathPtr& path) {
    PathPtr parent = path->parent();
    SimpleConfigPtr result = at_key(origin, path->last());

    while (parent) {
        std::string key = parent->last();
        result = result->at_key(origin, key);
        parent = parent->parent();
    }

    return result;
}

ConfigPtr AbstractConfigValue::at_path(const std::string& path) {
    SimpleConfigOriginPtr origin = SimpleConfigOrigin::new_simple("at_path(" + path + ")");
    return at_path(origin, Path::new_path(path));
}

}
