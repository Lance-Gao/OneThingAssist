
#include "detail/simple_config_object.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/simple_config.hpp"
#include "detail/resolve_status.hpp"
#include "detail/resolve_context.hpp"
#include "detail/path.hpp"
#include "config_render_options.hpp"
#include "config_origin.hpp"

namespace config {

SimpleConfigObject::SimpleConfigObject(const ConfigOriginPtr& origin,
        const MapAbstractConfigValue& value, ResolveStatus status, bool ignores_fallbacks) :
    AbstractConfigObject(origin),
    value(value.begin(), value.end()),
    ignores_fallbacks_(ignores_fallbacks) {
    resolved = (status == ResolveStatus::RESOLVED);

    // _kind of an expensive debug check. _comment out?
    if (status != ResolveStatusEnum::from_values(value)) {
        throw ConfigExceptionBugOrBroken("SimpleConfigObject created with wrong resolve status");
    }
}

SimpleConfigObject::SimpleConfigObject(const ConfigOriginPtr& origin,
        const MapAbstractConfigValue& value) :
    SimpleConfigObject(origin, value, ResolveStatusEnum::from_values(value), false) {
}

ConfigObjectPtr SimpleConfigObject::with_only_key(const std::string& key) {
    return with_only_path(_path::new_key(key));
}

ConfigObjectPtr SimpleConfigObject::without_key(const std::string& key) {
    return without_path(_path::new_key(key));
}

AbstractConfigObjectPtr SimpleConfigObject::with_only_path_or_null(const PathPtr& path) {
    std::string key = path->first();
    auto next = path->remainder();
    auto val = value.find(key);
    auto v = val == value.end() ? nullptr : std::dynamic_pointer_cast<AbstractConfigValue>
             (val->second);

    if (next) {
        if (v && instanceof<AbstractConfigObject>(v)) {
            v = std::static_pointer_cast<AbstractConfigObject>(v)->with_only_path_or_null(next);
        } else {
            // if the path has more elements but we don't have an object,
            // then the rest of the path does not exist.
            v = nullptr;
        }
    }

    if (!v) {
        return nullptr;
    } else {
        return SimpleConfigObject::make_instance(origin(), MapAbstractConfigValue({{key, v}}),
        v->resolve_status(), ignores_fallbacks_);
    }
}

AbstractConfigObjectPtr SimpleConfigObject::with_only_path(const PathPtr& path) {
    auto o = std::static_pointer_cast<SimpleConfigObject>(with_only_path_or_null(path));

    if (!o) {
        return SimpleConfigObject::make_instance(origin(), MapAbstractConfigValue(),
                ResolveStatus::RESOLVED, ignores_fallbacks_);
    } else {
        return o;
    }
}

AbstractConfigObjectPtr SimpleConfigObject::without_path(const PathPtr& path) {
    std::string key = path->first();
    auto next = path->remainder();
    auto val = value.find(key);
    auto v = val == value.end() ? nullptr : std::dynamic_pointer_cast<AbstractConfigValue>
             (val->second);

    if (v && next && instanceof<AbstractConfigObject>(v)) {
        v = std::static_pointer_cast<AbstractConfigObject>(v)->without_path(next);
        MapAbstractConfigValue updated = MiscUtils::dynamic_map<MapAbstractConfigValue>(value);
        updated[key] = v;
        return SimpleConfigObject::make_instance(origin(), updated,
                ResolveStatusEnum::from_values(updated),
                ignores_fallbacks_);
    } else if (next || !v) {
        // can't descend, nothing to remove
        return shared_from_this();
    } else {
        MapAbstractConfigValue smaller;

        for (auto& old : value) {
            if (old.first != key) {
                smaller[old.first] = std::dynamic_pointer_cast<AbstractConfigValue>(old.second);
            }
        }

        return SimpleConfigObject::make_instance(origin(), smaller,
                ResolveStatusEnum::from_values(smaller),
                ignores_fallbacks_);
    }
}

ConfigObjectPtr SimpleConfigObject::with_value(const std::string& key,
        const ConfigValuePtr& v) {
    if (!v) {
        throw ConfigExceptionBugOrBroken("_trying to store nullConfigValue in a ConfigObject");
    }

    MapAbstractConfigValue new_map;

    if (value.empty()) {
        new_map = {{key, std::dynamic_pointer_cast<AbstractConfigValue>(v)}};
    } else {
        new_map = MiscUtils::dynamic_map<MapAbstractConfigValue>(value);
        new_map[key] = std::dynamic_pointer_cast<AbstractConfigValue>(v);
    }

    return SimpleConfigObject::make_instance(origin(), new_map,
            ResolveStatusEnum::from_values(new_map),
            ignores_fallbacks_);
}

ConfigObjectPtr SimpleConfigObject::with_value(const PathPtr& path,
        const ConfigValuePtr& v) {
    std::string key = path->first();
    PathPtr next = path->remainder();

    if (!next) {
        return with_value(key, v);
    } else {
        auto val = value.find(key);
        auto child = val == value.end() ? nullptr : val->second;

        if (child && instanceof<AbstractConfigObject>(child)) {
            // if we have an object, add to it
            return with_value(key, std::dynamic_pointer_cast<ConfigValue>
                              (std::dynamic_pointer_cast<AbstractConfigObject>(child)->with_value(next, v)));
        } else {
            // as soon as we have a non-object, replace it entirely
            SimpleConfigPtr subtree = std::dynamic_pointer_cast<AbstractConfigValue>(v)->at_path(
                                             SimpleConfigOrigin::new_simple("with_value(" + next->render() + ")"), next);
            return with_value(key, subtree->root());
        }
    }
}

AbstractConfigValuePtr SimpleConfigObject::attempt_peek_with_partial_resolve(
    const std::string& key) {
    auto val = value.find(key);
    return val == value.end() ? nullptr : std::dynamic_pointer_cast<AbstractConfigValue>
           (val->second);
}

AbstractConfigObjectPtr SimpleConfigObject::new_copy(ResolveStatus new_status,
        const ConfigOriginPtr& new_origin, bool new_ignores_fallbacks) {
    return SimpleConfigObject::make_instance(new_origin,
            MiscUtils::dynamic_map<MapAbstractConfigValue>(value), new_status, new_ignores_fallbacks);
}

AbstractConfigObjectPtr SimpleConfigObject::new_copy(ResolveStatus new_status,
        const ConfigOriginPtr& new_origin) {
    return new_copy(new_status, new_origin, ignores_fallbacks_);
}

AbstractConfigValuePtr SimpleConfigObject::with_fallbacks_ignored() {
    if (ignores_fallbacks_) {
        return shared_from_this();
    } else {
        return new_copy(resolve_status(), origin(), true);
    }
}

ResolveStatus SimpleConfigObject::resolve_status() {
    return ResolveStatusEnum::from_bool(resolved);
}

bool SimpleConfigObject::ignores_fallbacks() {
    return ignores_fallbacks_;
}

ConfigVariant SimpleConfigObject::unwrapped() {
    MapVariant m;

    for (auto& e : value) {
        m[e.first] = e.second->unwrapped();
    }

    return m;
}

AbstractConfigValuePtr SimpleConfigObject::merged_with_object(const
        AbstractConfigObjectPtr&
        abstract_fallback) {
    require_not_ignoring_fallbacks();

    if (!instanceof<SimpleConfigObject>(abstract_fallback)) {
        throw ConfigExceptionBugOrBroken("should not be reached (merging non-SimpleConfigObject)");
    }

    auto fallback = std::static_pointer_cast<SimpleConfigObject>(abstract_fallback);

    bool changed = false;
    bool all_resolved = true;
    MapAbstractConfigValue merged;
    SetString all_keys;

    for (auto& v : value) {
        all_keys.insert(v.first);
    }

    for (auto& v : fallback->value) {
        all_keys.insert(v.first);
    }

    for (auto& key : all_keys) {
        auto first_val = value.find(key);
        auto first = first_val == value.end() ? nullptr : std::dynamic_pointer_cast<AbstractConfigValue>
                     (first_val->second);

        auto second_val = fallback->value.find(key);
        auto second = second_val == fallback->value.end() ? nullptr :
                      std::dynamic_pointer_cast<AbstractConfigValue>(second_val->second);

        AbstractConfigValuePtr kept;

        if (!first) {
            kept = second;
        } else if (!second) {
            kept = first;
        } else {
            kept = std::dynamic_pointer_cast<AbstractConfigValue>(first->with_fallback(second));
        }

        merged[key] = kept;

        if (first != kept) {
            changed = true;
        }

        if (kept->resolve_status() == ResolveStatus::UNRESOLVED) {
            all_resolved = false;
        }
    }

    ResolveStatus new_resolve_status = ResolveStatusEnum::from_bool(all_resolved);
    bool new_ignores_fallbacks = fallback->ignores_fallbacks();

    if (changed) {
        return SimpleConfigObject::make_instance(merge_origins(VectorAbstractConfigObject({shared_from_this(), fallback})),
                merged, new_resolve_status, new_ignores_fallbacks);
    } else if (new_resolve_status != resolve_status() || new_ignores_fallbacks != ignores_fallbacks()) {
        return new_copy(new_resolve_status, origin(), new_ignores_fallbacks);
    } else {
        return shared_from_this();
    }
}

SimpleConfigObjectPtr SimpleConfigObject::modify(const _no_exceptionsModifierPtr&
        modifier) {
    try {
        return modify_may_throw(modifier);
    } catch (ConfigException&) {
        throw;
    } catch (std::exception& e) {
        throw ConfigExceptionBugOrBroken(std::string("unexpected checked exception:") + e.what());
    }
}

SimpleConfigObjectPtr SimpleConfigObject::modify_may_throw(const ModifierPtr& modifier) {
    boost::optional<MapAbstractConfigValue> changes;

    for (auto& kv : value) {
        // "modified" may be null, which means remove the child;
        // to do that we put null in the "changes" map.
        auto modified = modifier->modify_child_may_throw(kv.first,
                        std::dynamic_pointer_cast<AbstractConfigValue>(kv.second));

        if (modified != kv.second) {
            if (!changes) {
                changes = MapAbstractConfigValue();
            }

            (*changes)[kv.first] = modified;
        }
    }

    if (!changes) {
        return shared_from_this();
    } else {
        MapAbstractConfigValue modified;
        bool saw_unresolved = false;

        for (auto& kv : value) {
            if (changes->count(kv.first) > 0) {
                auto new_value = changes->find(kv.first);

                if (new_value != changes->end() && new_value->second) {
                    modified[kv.first] = new_value->second;

                    if (new_value->second->resolve_status() == ResolveStatus::UNRESOLVED) {
                        saw_unresolved = true;
                    }
                } else {
                    // remove this child; don't put it in the new map.
                }
            } else {
                auto new_value = value.find(kv.first);
                modified[kv.first] = std::dynamic_pointer_cast<AbstractConfigValue>(new_value->second);

                if (std::dynamic_pointer_cast<AbstractConfigValue>(new_value->second)->resolve_status() ==
                        ResolveStatus::UNRESOLVED) {
                    saw_unresolved = true;
                }
            }
        }

        return SimpleConfigObject::make_instance(origin(), modified,
                saw_unresolved ? ResolveStatus::UNRESOLVED : ResolveStatus::RESOLVED, ignores_fallbacks());
    }
}

AbstractConfigValuePtr SimpleConfigObject::resolve_substitutions(
    const ResolveContextPtr& context) {
    if (resolve_status() == ResolveStatus::RESOLVED) {
        return shared_from_this();
    }

    try {
        return modify_may_throw(SimpleConfigObjectModifier::make_instance(context));
    } catch (NotPossibleToResolve&) {
        throw;
    } catch (ConfigException&) {
        throw;
    } catch (std::exception& e) {
        throw ConfigExceptionBugOrBroken(std::string("unexpected checked exception:") + e.what());
    }
}

SimpleConfigObjectModifier::SimpleConfigObjectModifier(const ResolveContextPtr& context)
    :
    context(context) {
}

AbstractConfigValuePtr SimpleConfigObjectModifier::modify_child_may_throw(
    const std::string& key,
    const AbstractConfigValuePtr& v) {
    if (context->is_restricted_to_child()) {
        if (key == context->restrict_to_child()->first()) {
            auto remainder = context->restrict_to_child()->remainder();

            if (remainder) {
                return context->restrict(remainder)->resolve(v);
            } else {
                // we don't want to resolve the leaf child.
                return v;
            }
        } else {
            // not in the restrict_to_child path
            return v;
        }
    } else {
        // no restrict_to_child, resolve everything
        return context->unrestricted()->resolve(v);
    }
}

AbstractConfigValuePtr SimpleConfigObject::relativized(const PathPtr& prefix) {
    return modify(SimpleConfigObjectNoExceptionsModifier::make_instance(prefix));
}

SimpleConfigObjectNoExceptionsModifier::SimpleConfigObjectNoExceptionsModifier(
    const PathPtr& prefix) :
    prefix(prefix) {
}

AbstractConfigValuePtr SimpleConfigObjectNoExceptionsModifier::modify_child(
    const std::string& key,
    const AbstractConfigValuePtr& v) {
    return v->relativized(prefix);
}

void SimpleConfigObject::render(std::string& s, uint32_t indent_,
                                   const ConfigRenderOptionsPtr& options) {
    if (value.empty()) {
        s += "{}";
    } else {
        bool outer_braces = indent_ > 0 || options->get_json();

        if (outer_braces) {
            s += "{";
        }

        if (options->get_formatted()) {
            s += "\n";
        }

        uint32_t separator_count = 0;

        for (auto& kv : value) {
            if (options->get_origin_comments()) {
                indent(s, indent_ + 1, options);
                s += "# ";
                s += kv.second->origin()->description();
                s += "\n";
            }

            if (options->get_comments()) {
                for (auto& comment : kv.second->origin()->comments()) {
                    indent(s, indent_ + 1, options);
                    s += "# ";
                    s += comment;
                    s += "\n";
                }
            }

            indent(s, indent_ + 1, options);
            std::dynamic_pointer_cast<AbstractConfigValue>(kv.second)->render(s, indent_ + 1, kv.first,
                    options);

            if (options->get_formatted()) {
                if (options->get_json()) {
                    s += ",";
                    separator_count = 2;
                } else {
                    separator_count = 1;
                }

                s += "\n";
            } else {
                s += ",";
                separator_count = 1;
            }
        }

        // chop last commas/newlines
        s.resize(s.length() - separator_count);

        if (options->get_formatted()) {
            s += "\n"; // put a newline back
            indent(s, indent_, options);
        }

        if (outer_braces) {
            s += "}";
        }
    }
}

ConfigValuePtr SimpleConfigObject::get(const std::string& key) {
    auto v = value.find(key);
    return v == value.end() ? nullptr : v->second;
}

bool SimpleConfigObject::map_equals(const MapConfigValue& a, const MapConfigValue& b) {
    return MiscUtils::map_equals(a, b);
}

uint32_t SimpleConfigObject::map_hash(const MapConfigValue& m) {
    // the keys have to be sorted, otherwise we could be equal
    // to another map but have a different hashcode.
    std::set<std::string> keys;
    MiscUtils::key_set(m.begin(), m.end(), std::inserter(keys, keys.end()));

    uint32_t values_hash = 0;
    size_t keys_hash = 0;

    for (auto& k : keys) {
        values_hash += std::dynamic_pointer_cast<AbstractConfigValue>(m.find(k)->second)->hash_code();
        boost::hash_combine(keys_hash, std::hash<std::string>()(k));
    }

    return 41 * (41 + static_cast<uint32_t>(keys_hash)) + values_hash;
}

bool SimpleConfigObject::can_equal(const ConfigVariant& other) {
    return instanceof<SimpleConfigObject>(other);
}

bool SimpleConfigObject::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    // neither are other "extras" like ignores_fallbacks or resolve status.
    if (instanceof<SimpleConfigObject>(other)) {
        return can_equal(other) && map_equals(value, dynamic_get<SimpleConfigObject>(other)->value);
    } else {
        return false;
    }
}

uint32_t SimpleConfigObject::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    // neither are other "extras" like ignores_fallbacks or resolve status.
    return map_hash(value);
}

MapConfigValue::const_iterator SimpleConfigObject::begin() const {
    return value.begin();
}

MapConfigValue::const_iterator SimpleConfigObject::end() const {
    return value.end();
}

MapConfigValue::mapped_type SimpleConfigObject::operator[](const MapConfigValue::key_type&
        key)
const {
    auto val = value.find(key);
    return val == value.end() ? nullptr : val->second;
}

bool SimpleConfigObject::empty() const {
    return value.empty();
}

MapConfigValue::size_type SimpleConfigObject::size() const {
    return value.size();
}

MapConfigValue::size_type SimpleConfigObject::count(const MapConfigValue::key_type& key)
const {
    return value.count(key);
}

MapConfigValue::const_iterator SimpleConfigObject::find(const MapConfigValue::key_type&
        key) const {
    return value.find(key);
}

SimpleConfigObjectPtr SimpleConfigObject::make_empty() {
    static std::string EMPTY_NAME = "empty config";
    static auto empty_instance = make_empty(SimpleConfigOrigin::new_simple(EMPTY_NAME));
    return empty_instance;
}

SimpleConfigObjectPtr SimpleConfigObject::make_empty(const ConfigOriginPtr& origin) {
    if (!origin) {
        return make_empty();
    } else {
        return SimpleConfigObject::make_instance(origin, MapAbstractConfigValue());
    }
}

SimpleConfigObjectPtr SimpleConfigObject::make_empty_missing(const ConfigOriginPtr&
        base_origin) {
    return SimpleConfigObject::make_instance(SimpleConfigOrigin::new_simple(
                base_origin->description() + " (not found)"), MapAbstractConfigValue());
}

}
