
#include "detail/config_reference.hpp"
#include "detail/substitution_expression.hpp"
#include "detail/resolve_replacer.hpp"
#include "detail/resolve_context.hpp"
#include "detail/resolve_source.hpp"
#include "detail/abstract_config_value.hpp"
#include "detail/path.hpp"
#include "config_exception.hpp"
#include "config_value_type.hpp"

namespace config {

ConfigReference::ConfigReference(const ConfigOriginPtr& origin,
                                     const SubstitutionExpressionPtr& expr) :
    ConfigReference(origin, expr, 0) {
}

ConfigReference::ConfigReference(const ConfigOriginPtr& origin,
                                     const SubstitutionExpressionPtr& expr, uint32_t prefix_length) :
    AbstractConfigValue(origin),
    expr(expr),
    prefix_length(prefix_length) {
}

ConfigValueType ConfigReference::value_type() {
    throw ConfigExceptionNotResolved("need to Config#resolve(), see the API docs for Config#resolve(); substitution not resolved: "
                                         + ConfigBase::to_string());
}

ConfigVariant ConfigReference::unwrapped() {
    throw ConfigExceptionNotResolved("need to Config#resolve(), see the API docs for Config#resolve(); substitution not resolved: "
                                         + ConfigBase::to_string());
}

AbstractConfigValuePtr ConfigReference::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin, expr, prefix_length);
}

bool ConfigReference::ignores_fallbacks() {
    return false;
}

VectorAbstractConfigValue ConfigReference::unmerged_values() {
    return {shared_from_this()};
}

AbstractConfigValuePtr ConfigReference::resolve_substitutions(const ResolveContextPtr&
        context) {
    context->source()->replace(shared_from_this(), ResolveReplacer::cycle_resolve_replacer());
    ConfigExceptionPtr finally;
    AbstractConfigValuePtr v;

    try {
        try {
            v = context->source()->lookup_subst(context, expr, prefix_length);
        } catch (NotPossibleToResolve& e) {
            if (expr->optional()) {
                v.reset();
            } else {
                throw ConfigExceptionUnresolvedSubstitution(
                    origin(),
                    expr->to_string() + " was part of a cycle of substitutions involving " + e.trace_string());
            }
        }

        if (!v && !expr->optional()) {
            throw ConfigExceptionUnresolvedSubstitution(origin(), expr->to_string());
        }

    } catch (ConfigException& e) {
        finally = e.clone();
    }

    context->source()->unreplace(shared_from_this());

    if (finally) {
        finally->raise();
    }

    return v;
}

ResolveStatus ConfigReference::resolve_status() {
    return ResolveStatus::UNRESOLVED;
}

AbstractConfigValuePtr ConfigReference::relativized(const PathPtr& prefix) {
    auto new_expr = expr->change_path(expr->path()->prepend(prefix));
    return make_instance(origin(), new_expr, prefix_length + prefix->length());
}

bool ConfigReference::can_equal(const ConfigVariant& other) {
    return instanceof<ConfigReference>(other);
}

bool ConfigReference::equals(const ConfigVariant& other) {
    // note that "origin" is deliberately NOT part of equality
    if (instanceof<ConfigReference>(other)) {
        return can_equal(other) && this->expr->equals(static_get<ConfigReference>(other)->expr);
    } else {
        return false;
    }
}

uint32_t ConfigReference::hash_code() {
    // note that "origin" is deliberately NOT part of equality
    return expr->hash_code();
}

void ConfigReference::render(std::string& s, uint32_t indent,
                               const ConfigRenderOptionsPtr& options) {
    s += expr->to_string();
}

SubstitutionExpressionPtr ConfigReference::expression() {
    return expr;
}

}
