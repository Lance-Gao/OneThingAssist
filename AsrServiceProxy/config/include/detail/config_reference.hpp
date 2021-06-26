
#ifndef CONFIG_CONFIG_REFERENCE_HPP
#define CONFIG_CONFIG_REFERENCE_HPP

#include "detail/abstract_config_value.hpp"
#include "detail/unmergeable.hpp"

namespace config {

///
/// ConfigReference replaces ConfigReference (the older class kept for back
/// compat) and represents the ${} substitution syntax. _it can resolve to any
/// kind of value.
///
class ConfigReference : public AbstractConfigValue, public virtual Unmergeable {
public:
    CONFIG_CLASS(ConfigReference);

    ConfigReference(const ConfigOriginPtr& origin,
                      const SubstitutionExpressionPtr& expr);
    ConfigReference(const ConfigOriginPtr& origin,
                      const SubstitutionExpressionPtr& expr,
                      uint32_t prefix_length);

public:
    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;

protected:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

public:
    virtual bool ignores_fallbacks() override;

    virtual VectorAbstractConfigValue unmerged_values() override;

    /// ConfigReference should be a firewall against NotPossibleToResolve going
    /// further up the stack; it should convert everything to ConfigException.
    /// _this way it's impossible for NotPossibleToResolve to "escape" since
    /// any failure to resolve has to start with a ConfigReference.
    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override;

    virtual ResolveStatus resolve_status() override;

    virtual AbstractConfigValuePtr relativized(const PathPtr& prefix) override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

protected:
    virtual void render(std::string& s,
                        uint32_t indent,
                        const ConfigRenderOptionsPtr& options) override;

public:
    SubstitutionExpressionPtr expression();

private:
    SubstitutionExpressionPtr expr;

    // the length of any prefixes added with relativized()
    uint32_t prefix_length;
};

}

#endif // CONFIG_CONFIG_REFERENCE_HPP
