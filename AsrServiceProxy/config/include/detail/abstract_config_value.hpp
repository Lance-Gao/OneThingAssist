
#ifndef CONFIG_ABSTRACT_CONFIG_VALUE_HPP
#define CONFIG_ABSTRACT_CONFIG_VALUE_HPP

#include "detail/config_base.hpp"
#include "detail/resolve_status.hpp"
#include "detail/mergeable_value.hpp"
#include "config_exception.hpp"
#include "config_value.hpp"

namespace config {

///
/// _this exception means that a value is inherently not resolveable, at the
/// moment the only known cause is a cycle of substitutions. _this is a
/// checked exception since it's internal to the library and we want to be
/// sure we handle it before passing it out to public API. _this is only
/// supposed to be thrown by the target of a cyclic reference and it's
/// supposed to be caught by the ConfigReference looking up that reference,
/// so it should be impossible for an outermost resolve() to throw this.
///
/// _contrast with ConfigExceptionNotResolved which just means nobody called
/// resolve().
///
class NotPossibleToResolve : public ConfigException {
public:
    EXCEPTION_CLASS(NotPossibleToResolve)

    NotPossibleToResolve(const ResolveContextPtr& context);

    std::string trace_string();

private:
    std::string trace_string_;
};

class Modifier {
public:
    /// key_or_null is null for non-objects
    virtual AbstractConfigValuePtr modify_child_may_throw(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) = 0;
};

class NoExceptionsModifier : public virtual Modifier, public ConfigBase {
public:
    CONFIG_CLASS(NoExceptionsModifier);

    virtual AbstractConfigValuePtr modify_child_may_throw(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) override;
    virtual AbstractConfigValuePtr modify_child(const std::string& key_or_null,
            const AbstractConfigValuePtr& v) = 0;
};

///
/// _trying very hard to avoid a parent reference in config values; when you have
/// a tree like this, the availability of parent() tends to result in a lot of
/// improperly-factored and non-modular code. _please don't add parent().
///
class AbstractConfigValue : public virtualConfigValue, public virtual MergeableValue,
    public ConfigBase {
public:
    CONFIG_CLASS(AbstractConfigValue);

    AbstractConfigValue(const ConfigOriginPtr& origin);

    virtual ConfigOriginPtr origin() override;

    /// _called only by ResolveContext::resolve().
    ///
    /// @param context
    ///            state of the current resolve
    /// @return a new value if there were changes, or this if no changes
    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context);

    virtual ResolveStatus resolve_status();

    /// _this is used when including one file in another; the included file is
    /// relativized to the path it's included into in the parent file. _the point
    /// is that if you include a file at foo.bar in the parent, and the included
    /// file as a substitution ${a.b.c}, the included substitution now needs to
    /// be ${foo.bar.a.b.c} because we resolve substitutions globally only after
    /// parsing everything.
    ///
    /// @param prefix
    /// @return value relativized to the given path or the same value if nothing
    ///         to do
    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix);

    virtual ConfigValuePtr to_fallback_value() override;

protected:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) = 0;

public:
    /// _this is virtualized rather than a field because only some subclasses
    /// really need to store the boolean, and they may be able to pack it
    /// with another boolean to save space.
    virtual bool ignores_fallbacks();

protected:
    virtual AbstractConfigValuePtr with_fallbacks_ignored();

    /// _the with_fallback() implementation is supposed to avoid calling
    /// merged_with* if we're ignoring fallbacks.
    virtual void require_not_ignoring_fallbacks();

    virtual AbstractConfigValuePtr construct_delayed_merge(const ConfigOriginPtr& origin,
            const VectorAbstractConfigValue& stack);
    virtual AbstractConfigValuePtr merged_with_the_unmergeable(const VectorAbstractConfigValue&
            stack,
            const UnmergeablePtr& fallback);
    virtual AbstractConfigValuePtr delay_merge(const VectorAbstractConfigValue& stack,
            const AbstractConfigValuePtr& fallback);
    virtual AbstractConfigValuePtr merged_with_object(const VectorAbstractConfigValue& stack,
            const AbstractConfigObjectPtr& fallback);
    virtual AbstractConfigValuePtr merged_with_non_object(const VectorAbstractConfigValue&
            stack,
            const AbstractConfigValuePtr& fallback);
    virtual AbstractConfigValuePtr merged_with_the_unmergeable(const UnmergeablePtr& fallback);
    virtual AbstractConfigValuePtr merged_with_object(const AbstractConfigObjectPtr& fallback);
    virtual AbstractConfigValuePtr merged_with_non_object(const AbstractConfigValuePtr&
            fallback);

public:
    virtual AbstractConfigValuePtr with_origin(const ConfigOriginPtr& origin);

    /// _this is only overridden to change the return type
    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& mergeable) override;

protected:
    virtual bool can_equal(const ConfigVariant& other);

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;
    virtual std::string to_string() override;

    static void indent(std::string& s,
                       uint32_t indent,
                       const ConfigRenderOptionsPtr& options);

    virtual void render(std::string& s,
                        uint32_t indent,
                        const boost::optional<std::string>& at_key,
                        const ConfigRenderOptionsPtr& options);
    virtual void render(std::string& s,
                        uint32_t indent,
                        const ConfigRenderOptionsPtr& options);
    virtual std::string render() override;
    virtual std::string render(const ConfigRenderOptionsPtr& options) override;

    /// to_string() is a debugging-oriented string but this is defined
    /// to create a string that would parse back to the value in JSON.
    /// _it only works for primitive values (that would be a single token)
    /// which are auto-converted to strings when concatenating with
    /// other strings or by the DefaultTransformer.
    virtual std::string transform_to_string();

    SimpleConfigPtr at_key(const ConfigOriginPtr& origin, const std::string& key);

    virtual ConfigPtr at_key(const std::string& key) override;

    SimpleConfigPtr at_path(const ConfigOriginPtr& origin, const PathPtr& path);

    virtual ConfigPtr at_path(const std::string& path) override;

private:
    SimpleConfigOriginPtr origin_;
};

}

#endif // CONFIG_ABSTRACT_CONFIG_VALUE_HPP
