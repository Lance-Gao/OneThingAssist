
#include "detail/config_concatenation.hpp"
#include "detail/simple_config_list.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/config_string.hpp"
#include "detail/resolve_context.hpp"
#include "config_object.hpp"
#include "config_exception.hpp"

namespace config {

ConfigConcatenation::ConfigConcatenation(const ConfigOriginPtr& origin,
        const VectorAbstractConfigValue& pieces) :
    AbstractConfigValue(origin),
    pieces(pieces) {
    if (pieces.size() < 2) {
        throw ConfigExceptionBugOrBroken("_created concatenation with less than 2 items: " +
                                              to_string());
    }

    bool had_unmergeable = false;

    for (auto& p : pieces) {
        if (instanceof<ConfigConcatenation>(p)) {
            throw ConfigExceptionBugOrBroken("ConfigConcatenation should never be nested: " +
                                                  to_string());
        }

        if (instanceof<Unmergeable>(p)) {
            had_unmergeable = true;
        }
    }

    if (!had_unmergeable) {
        throw ConfigExceptionBugOrBroken("_created concatenation without an unmergeable in it: " +
                                              to_string());
    }
}

ConfigExceptionNotResolved ConfigConcatenation::not_resolved() {
    return ConfigExceptionNotResolved("need to Config::resolve(), see the API docs for Config::resolve(); substitution not resolved: "
                                          + to_string());
}

ConfigValueType ConfigConcatenation::value_type() {
    throw not_resolved();
}

ConfigVariant ConfigConcatenation::unwrapped() {
    throw not_resolved();
}

AbstractConfigValuePtr ConfigConcatenation::new_copy(const ConfigOriginPtr& new_origin) {
    return make_instance(new_origin, pieces);
}

bool ConfigConcatenation::ignores_fallbacks() {
    // we can never ignore fallbacks because if a child ConfigReference
    // is self-referential we have to look lower in the merge stack
    // for its value.
    return false;
}

VectorAbstractConfigValue ConfigConcatenation::unmerged_values() {
    return {shared_from_this()};
}

void ConfigConcatenation::join(VectorAbstractConfigValue& builder,
                                 const AbstractConfigValuePtr& right) {
    auto left = builder.back();
    // _since this depends on the type of two instances, _i couldn't think
    // of much alternative to an instanceof chain. _visitors are sometimes
    // used for multiple dispatch but seems like overkill.
    AbstractConfigValuePtr joined;

    if (instanceof<ConfigObject>(left) && instanceof<ConfigObject>(right)) {
        joined = std::dynamic_pointer_cast<AbstractConfigValue>(right->with_fallback(left));
    } else if (instanceof<SimpleConfigList>(left) && instanceof<SimpleConfigList>(right)) {
        joined = std::static_pointer_cast<SimpleConfigList>(left)->concatenate(
                     std::static_pointer_cast<SimpleConfigList>(right));
    } else if (instanceof<ConfigConcatenation>(left) || instanceof<ConfigConcatenation>(right)) {
        throw ConfigExceptionBugOrBroken("unflattened ConfigConcatenation");
    } else if (instanceof<Unmergeable>(left) || instanceof<Unmergeable>(right)) {
        // leave joined=null, cannot join
    } else {
        // handle primitive type or primitive type mixed with object or list
        std::string s1 = left->transform_to_string();
        std::string s2 = right->transform_to_string();

        if (s1.empty() || s2.empty()) {
            throw ConfigExceptionWrongType(left->origin(),
                                               "_cannot concatenate object or list with a non-object-or-list, " +
                                               left->to_string() + " and " + right->to_string() + " are not compatible");
        } else {
            auto joined_origin = SimpleConfigOrigin::merge_origins(left->origin(), right->origin());
            joined = ConfigString::make_instance(joined_origin, s1 + s2);
        }
    }

    if (!joined) {
        builder.push_back(right);
    } else {
        builder.resize(builder.size() - 1);
        builder.push_back(joined);
    }
}

VectorAbstractConfigValue ConfigConcatenation::consolidate(const VectorAbstractConfigValue&
        pieces) {
    if (pieces.size() < 2) {
        return pieces;
    } else {
        VectorAbstractConfigValue flattened;
        flattened.reserve(pieces.size());

        for (auto& v : pieces) {
            if (instanceof<ConfigConcatenation>(v)) {
                flattened.insert(flattened.end(),
                                 std::static_pointer_cast<ConfigConcatenation>(v)->pieces.begin(),
                                 std::static_pointer_cast<ConfigConcatenation>(v)->pieces.end());
            } else {
                flattened.push_back(v);
            }
        }

        VectorAbstractConfigValue consolidated;
        consolidated.reserve(flattened.size());

        for (auto& v : flattened) {
            if (consolidated.empty()) {
                consolidated.push_back(v);
            } else {
                join(consolidated, v);
            }
        }

        return consolidated;
    }
}

AbstractConfigValuePtr ConfigConcatenation::concatenate(const VectorAbstractConfigValue&
        pieces) {
    VectorAbstractConfigValue consolidated = consolidate(pieces);

    if (consolidated.empty()) {
        return nullptr;
    } else if (consolidated.size() == 1) {
        return consolidated.front();
    } else {
        auto merged_origin = SimpleConfigOrigin::merge_origins(consolidated);
        return make_instance(merged_origin, consolidated);
    }
}

AbstractConfigValuePtr ConfigConcatenation::resolve_substitutions(
    const ResolveContextPtr& context) {
    VectorAbstractConfigValue resolved;
    resolved.reserve(pieces.size());

    for (auto& p : pieces) {
        // to concat into a string we have to do a full resolve,
        // so unrestrict the context
        auto r = context->unrestricted()->resolve(p);

        if (!r) {
            // it was optional... omit
        } else {
            resolved.push_back(r);
        }
    }

    // now need to concat everything
    VectorAbstractConfigValue joined = consolidate(resolved);

    if (joined.size() != 1) {
        throw ConfigExceptionBugOrBroken("_resolved list should always join to exactly one value, not "
                                              +
                                              boost::lexical_cast<std::string>(joined.size()));
    }

    return joined.front();
}

ResolveStatus ConfigConcatenation::resolve_status() {
    return ResolveStatus::UNRESOLVED;
}

AbstractConfigValuePtr ConfigConcatenation::relativized(const PathPtr& prefix) {
    VectorAbstractConfigValue new_pieces;
    new_pieces.reserve(pieces.size());

    for (auto& p : pieces) {
        new_pieces.push_back(p->relativized(prefix));
    }

    return make_instance(origin(), new_pieces);
}

bool ConfigConcatenation::can_equal(const ConfigVariant& other) {
    return instanceof<ConfigConcatenation>(other);
}

bool ConfigConcatenation::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<ConfigConcatenation>(other)) {
        return can_equal(other) &&
               this->pieces.size() == dynamic_get<ConfigConcatenation>(other)->pieces.size() &&
               std::equal(this->pieces.begin(), this->pieces.end(),
                          dynamic_get<ConfigConcatenation>(other)->pieces.begin(),
                          config_equals<AbstractConfigValuePtr>());
    } else {
        return false;
    }
}

uint32_t ConfigConcatenation::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    size_t hash = 0;

    for (auto& v : pieces) {
        boost::hash_combine(hash, v->hash_code());
    }

    return static_cast<uint32_t>(hash);
}

void ConfigConcatenation::render(std::string& s, uint32_t indent,
                                   const ConfigRenderOptionsPtr& options) {
    for (auto& p : pieces) {
        p->render(s, indent, options);
    }
}

}
