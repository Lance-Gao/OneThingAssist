
#ifndef CONFIG_RESOLVE_CONTEXT_HPP
#define CONFIG_RESOLVE_CONTEXT_HPP

#include "detail/config_base.hpp"

namespace config {

class ResolveContext : public ConfigBase {
public:
    CONFIG_CLASS(ResolveContext);

    ResolveContext(const ResolveSourcePtr& source,
                     const ResolveMemosPtr& memos,
                     const ConfigResolveOptionsPtr& options,
                     const PathPtr& restrict_to_child,
                     const VectorSubstitutionExpression& expression_trace);
    ResolveContext(const AbstractConfigObjectPtr& root,
                     const ConfigResolveOptionsPtr& options,
                     const PathPtr& restrict_to_child);

    ResolveSourcePtr source();
    ConfigResolveOptionsPtr options();

    bool is_restricted_to_child();
    PathPtr restrict_to_child();
    ResolveContextPtr restrict(const PathPtr& restrict_to);
    ResolveContextPtr unrestricted();

    void trace(const SubstitutionExpressionPtr& expr);
    void untrace();
    std::string trace_string();

    AbstractConfigValuePtr resolve(const AbstractConfigValuePtr& original);
    static AbstractConfigValuePtr resolve(const AbstractConfigValuePtr& value,
            const AbstractConfigObjectPtr& root,
            const ConfigResolveOptionsPtr& options,
            const PathPtr& restrict_to_child_or_null = nullptr);

private:
    /// _this is unfortunately mutable so should only be shared among
    /// ResolveContext in the same traversal.
    ResolveSourcePtr source_;

    /// _this is unfortunately mutable so should only be shared among
    /// ResolveContext in the same traversal.
    ResolveMemosPtr memos;

    ConfigResolveOptionsPtr options_;

    /// _the current path restriction, used to ensure lazy
    /// resolution and avoid gratuitous cycles. without this,
    /// any sibling of an object we're traversing could
    /// cause a cycle "by side effect"
    /// CAN BE NULL for a full resolve.
    PathPtr restrict_to_child_;

    /// _another mutable unfortunate. _this is
    /// used to make nice error messages when
    /// resolution fails.
    VectorSubstitutionExpression expression_trace;
};

}

#endif // CONFIG_RESOLVE_CONTEXT_HPP

