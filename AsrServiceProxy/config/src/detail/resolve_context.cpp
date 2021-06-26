
#include "detail/resolve_context.hpp"
#include "detail/resolve_source.hpp"
#include "detail/resolve_status.hpp"
#include "detail/resolve_memos.hpp"
#include "detail/abstract_config_value.hpp"
#include "detail/path.hpp"
#include "detail/memo_key.hpp"
#include "detail/substitution_expression.hpp"
#include "config_exception.hpp"

namespace config {

ResolveContext::ResolveContext(const ResolveSourcePtr& source,
                                   const ResolveMemosPtr& memos,
                                   const ConfigResolveOptionsPtr& options,
                                   const PathPtr& restrict_to_child,
                                   const VectorSubstitutionExpression& expression_trace) :
    source_(source),
    memos(memos),
    options_(options),
    restrict_to_child_(restrict_to_child),
    expression_trace(expression_trace) {
}

ResolveContext::ResolveContext(const AbstractConfigObjectPtr& root,
                                   const ConfigResolveOptionsPtr& options,
                                   const PathPtr& restrict_to_child) :
    ResolveContext(ResolveSource::make_instance(root), ResolveMemos::make_instance(), options,
                     restrict_to_child, {}) {
}

ResolveSourcePtr ResolveContext::source() {
    return source_;
}

ConfigResolveOptionsPtr ResolveContext::options() {
    return options_;
}

bool ResolveContext::is_restricted_to_child() {
    return !!restrict_to_child_;
}

PathPtr ResolveContext::restrict_to_child() {
    return restrict_to_child_;
}

ResolveContextPtr ResolveContext::restrict(const PathPtr& restrict_to) {
    if (restrict_to == restrict_to_child_) {
        return shared_from_this();
    } else {
        return ResolveContext::make_instance(source_, memos, options_, restrict_to, expression_trace);
    }
}

ResolveContextPtr ResolveContext::unrestricted() {
    return restrict(nullptr);
}

void ResolveContext::trace(const SubstitutionExpressionPtr& expr) {
    expression_trace.push_back(expr);
}

void ResolveContext::untrace() {
    expression_trace.pop_back();
}

std::string ResolveContext::trace_string() {
    std::string separator = ", ";
    std::ostringstream stream;

    for (auto& expr : expression_trace) {
        stream << expr->to_string() << separator;
    }

    std::string trace = stream.str();

    if (!trace.empty()) {
        trace.resize(trace.length() - separator.length());
    }

    return trace;
}

AbstractConfigValuePtr ResolveContext::resolve(const AbstractConfigValuePtr& original) {
    // a fully-resolved (no restrict_to_child) object can satisfy a
    // request for a restricted object, so always check that first.
    auto full_key = MemoKey::make_instance(original, nullptr);
    MemoKeyPtr restricted_key;

    auto cached = memos->get(full_key);

    // but if there was no fully-resolved object cached, we'll only
    // compute the restrict_to_child object so use a more limited
    // memo key
    if (!cached && is_restricted_to_child()) {
        restricted_key = MemoKey::make_instance(original, restrict_to_child());
        cached = memos->get(restricted_key);
    }

    if (cached) {
        return cached;
    } else {
        auto resolved = source_->resolve_checking_replacement(shared_from_this(), original);

        if (!resolved || resolved->resolve_status() == ResolveStatus::RESOLVED) {
            // if the resolved object is fully resolved by resolving
            // only the restrict_to_child_or_null, then it can be cached
            // under full_key since the child we were restricted to
            // turned out to be the only unresolved thing.
            memos->put(full_key, resolved);
        } else {
            // if we have an unresolved object then either we did a
            // partial resolve restricted to a certain child, or it's
            // a bug.
            if (is_restricted_to_child()) {
                if (!restricted_key) {
                    throw ConfigExceptionBugOrBroken("restricted_key should not be null here");
                }

                memos->put(restricted_key, resolved);
            } else {
                throw ConfigExceptionBugOrBroken("resolve_substitutions() did not give us a resolved object");
            }
        }

        return resolved;
    }
}

AbstractConfigValuePtr ResolveContext::resolve(const AbstractConfigValuePtr& value,
        const AbstractConfigObjectPtr& root, const ConfigResolveOptionsPtr& options,
        const PathPtr& restrict_to_child_or_null) {
    auto context = ResolveContext::make_instance(root, options, nullptr);

    try {
        return context->resolve(value);
    } catch (NotPossibleToResolve&) {
        // ConfigReference was supposed to catch NotPossibleToResolve
        throw ConfigExceptionBugOrBroken("NotPossibleToResolve was thrown from an outermost resolve");
    }
}

}
