
#include "detail/simple_config_list.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/resolve_status.hpp"
#include "detail/resolve_context.hpp"
#include "config_value_type.hpp"
#include "config_render_options.hpp"
#include "config_origin.hpp"

namespace config {

SimpleConfigList::SimpleConfigList(const ConfigOriginPtr& origin,
        const VectorAbstractConfigValue& value) :
    SimpleConfigList(origin, value, ResolveStatusEnum::from_values(value)) {
}

SimpleConfigList::SimpleConfigList(const ConfigOriginPtr& origin,
        const VectorAbstractConfigValue& value, ResolveStatus status) :
    AbstractConfigValue(origin),
    value(value.begin(), value.end()) {
    resolved = (status == ResolveStatus::RESOLVED);

    // kind of an expensive debug check (makes this constructor pointless)
    if (status != ResolveStatusEnum::from_values(value)) {
        throw ConfigExceptionBugOrBroken("SimpleConfigList created with wrong resolve status");
    }
}

ConfigValueType SimpleConfigList::value_type() {
    return ConfigValueType::LIST;
}

ConfigVariant SimpleConfigList::unwrapped() {
    VectorVariant list;
    list.reserve(value.size());

    for (auto& v : value) {
        list.push_back(v->unwrapped());
    }

    return list;
}

ResolveStatus SimpleConfigList::resolve_status() {
    return ResolveStatusEnum::from_bool(resolved);
}

SimpleConfigList SimpleConfigList::modify(const _no_exceptionsModifierPtr& modifier,
        ResolveStatus new_resolve_status) {
    try {
        return modify_may_throw(modifier, new_resolve_status);
    } catch (ConfigException&) {
        throw;
    } catch (std::exception& e) {
        throw ConfigExceptionBugOrBroken(std::string("unexpected checked exception:") + e.what());
    }
}

SimpleConfigList SimpleConfigList::modify_may_throw(const ModifierPtr& modifier,
        ResolveStatus new_resolve_status) {
    // lazy-create for optimization
    boost::optional<VectorAbstractConfigValue> changed;
    uint32_t i = 0;

    for (auto& v : value) {
        auto modified = modifier->modify_child_may_throw("",
                        std::dynamic_pointer_cast<AbstractConfigValue>(v));

        if (!changed && modified != v) {
            changed = VectorAbstractConfigValue();

            for (uint32_t j = 0; j < i; ++j) {
                changed->push_back(std::dynamic_pointer_cast<AbstractConfigValue>(value[j]));
            }
        }

        // once the new list is created, all elements
        // have to go in it. if modify_child returned
        // null, we drop that element.
        if (changed && modified) {
            changed->push_back(modified);
        }

        i++;
    }

    if (changed) {
        return SimpleConfigList::make_instance(origin(), *changed, new_resolve_status);
    } else {
        return shared_from_this();
    }
}

AbstractConfigValuePtr SimpleConfigList::resolve_substitutions(
    const ResolveContextPtr& context) {
    if (resolved) {
        return shared_from_this();
    }

    if (context->is_restricted_to_child()) {
        // if a list restricts to a child path, then it has no child paths,
        // so nothing to do.
        return shared_from_this();
    } else {
        try {
            return modify_may_throw(SimpleConfigListModifier::make_instance(context),
                                    ResolveStatus::RESOLVED);
        } catch (NotPossibleToResolve&) {
            throw;
        } catch (ConfigException&) {
            throw;
        } catch (std::exception& e) {
            throw ConfigExceptionBugOrBroken(std::string("unexpected checked exception:") + e.what());
        }
    }
}

SimpleConfigListModifier::SimpleConfigListModifier(const ResolveContextPtr& context) :
    context(context) {
}

AbstractConfigValuePtr SimpleConfigListModifier::modify_child_may_throw(
    const std::string& key_or_null,
    const AbstractConfigValuePtr& v) {
    return context->resolve(v);
}

SimpleConfigListNoExceptionsModifier::SimpleConfigListNoExceptionsModifier(
    const PathPtr& prefix) :
    prefix(prefix) {
}

AbstractConfigValuePtr SimpleConfigListNoExceptionsModifier::modify_child(
    const std::string& key_or_null, const AbstractConfigValuePtr& v) {
    return v->relativized(prefix);
}

AbstractConfigValuePtr SimpleConfigList::relativized(const PathPtr& prefix) {
    return modify(SimpleConfigListNoExceptionsModifier::make_instance(prefix), resolve_status());
}

bool SimpleConfigList::can_equal(const ConfigVariant& other) {
    return instanceof<SimpleConfigList>(other);
}

bool SimpleConfigList::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<SimpleConfigList>(other)) {
        return can_equal(other) &&
               this->value.size() == dynamic_get<SimpleConfigList>(other)->value.size() &&
               std::equal(this->value.begin(), this->value.end(),
                          dynamic_get<SimpleConfigList>(other)->value.begin(),
                          [&](const VectorConfigValue::value_type & first,
        const VectorConfigValue::value_type & second) {
            return config_equals<AbstractConfigValuePtr>()(
                       std::dynamic_pointer_cast<AbstractConfigValue>(first),
                       std::dynamic_pointer_cast<AbstractConfigValue>(second));
        });
    } else {
        return false;
    }
}

uint32_t SimpleConfigList::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    size_t hash = 0;

    for (auto& v : value) {
        boost::hash_combine(hash, std::dynamic_pointer_cast<AbstractConfigValue>(v)->hash_code());
    }

    return static_cast<uint32_t>(hash);
}

void SimpleConfigList::render(std::string& s, uint32_t indent_,
                                 const ConfigRenderOptionsPtr& options) {
    if (value.empty()) {
        s += "[]";
    } else {
        s += "[";

        if (options->get_formatted()) {
            s += "\n";
        }

        for (auto& v : value) {
            if (options->get_origin_comments()) {
                indent(s, indent_ + 1, options);
                s += "# ";
                s += v->origin()->description();
                s += "\n";
            }

            if (options->get_comments()) {
                for (auto& comment : v->origin()->comments()) {
                    indent(s, indent_ + 1, options);
                    s += "# ";
                    s += comment;
                    s += "\n";
                }
            }

            indent(s, indent_ + 1, options);

            std::dynamic_pointer_cast<AbstractConfigValue>(v)->render(s, indent_ + 1, options);
            s += ",";

            if (options->get_formatted()) {
                s += "\n";
            }
        }

        s.resize(s.length() - 1); // chop or newline

        if (options->get_formatted()) {
            s.resize(s.length() - 1); // also chop comma
            s += "\n";
            indent(s, indent_, options);
        }

        s += "]";
    }
}

VectorConfigValue::const_iterator SimpleConfigList::begin() const {
    return value.begin();
}

VectorConfigValue::const_iterator SimpleConfigList::end() const {
    return value.end();
}

VectorConfigValue::const_reference SimpleConfigList::at(VectorConfigValue::size_type n)
const {
    return value.at(n);
}

VectorConfigValue::const_reference SimpleConfigList::front() const {
    return value.front();
}

VectorConfigValue::const_reference SimpleConfigList::back() const {
    return value.back();
}

VectorConfigValue::const_reference SimpleConfigList::operator[](
    VectorConfigValue::size_type n)
const {
    return value.at(n);
}

bool SimpleConfigList::empty() const {
    return value.empty();
}

VectorConfigValue::size_type SimpleConfigList::size() const {
    return value.size();
}

void SimpleConfigList::clear() {
    throw we_are_immutable("clear");
}

void SimpleConfigList::pop_back() {
    throw we_are_immutable("pop_back");
}

void SimpleConfigList::resize(VectorConfigValue::size_type n,
                                 const VectorConfigValue::value_type& val) {
    throw we_are_immutable("resize");
}

VectorConfigValue::const_iterator SimpleConfigList::erase(VectorConfigValue::const_iterator
        pos) {
    throw we_are_immutable("erase");
}

VectorConfigValue::const_iterator SimpleConfigList::insert(
    VectorConfigValue::const_iterator pos,
    const VectorConfigValue::value_type& val) {
    throw we_are_immutable("insert");
}

ConfigExceptionUnsupportedOperation SimpleConfigList::we_are_immutable(
    const std::string& method) {
    return ConfigExceptionUnsupportedOperation(" ConfigList is immutable, you can't call _list.'" +
            method
            + "'");
}

AbstractConfigValuePtr SimpleConfigList::new_copy(const ConfigOriginPtr& new_origin) {
    return SimpleConfigList::make_instance(new_origin,
            MiscUtils::dynamic_vector<VectorAbstractConfigValue>(value));
}

SimpleConfigList SimpleConfigList::concatenate(const SimpleConfigList& other) {
    auto combined_origin = SimpleConfigOrigin::merge_origins(origin(), other->origin());
    VectorConfigValue combined(value);
    combined.insert(combined.end(), other->value.begin(), other->value.end());
    return SimpleConfigList::make_instance(combined_origin,
            MiscUtils::dynamic_vector<VectorAbstractConfigValue>(combined));
}

}
