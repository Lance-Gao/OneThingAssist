
#ifndef CONFIG_CONFIG_CONCATENATION_HPP
#define CONFIG_CONFIG_CONCATENATION_HPP

#include "detail/abstract_config_value.hpp"
#include "detail/unmergeable.hpp"

namespace config {

///
/// _a ConfigConcatenation represents a list of values to be concatenated (see the
/// spec). _it only has to exist if at least one value is an unresolved
/// substitution, otherwise we could go ahead and collapse the list into a single
/// value.
///
/// _right now this is always a list of strings and ${} references, but in the
/// future should support a list of ConfigList. _we may also support
/// concatenations of objects, but ConfigDelayedMerge should be used for that
/// since a concat of objects really will merge, not concatenate.
///
class ConfigConcatenation : public AbstractConfigValue, public virtual Unmergeable {
public:
    CONFIG_CLASS(ConfigConcatenation);

    ConfigConcatenation(const ConfigOriginPtr& origin,
                          const VectorAbstractConfigValue& pieces);

private:
    ConfigExceptionNotResolved not_resolved();

public:
    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;

protected:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

public:
    virtual bool ignores_fallbacks() override;

    virtual VectorAbstractConfigValue unmerged_values() override;

    static void join(VectorAbstractConfigValue& builder, const AbstractConfigValuePtr& right);
    static VectorAbstractConfigValue consolidate(const VectorAbstractConfigValue& pieces);
    static AbstractConfigValuePtr concatenate(const VectorAbstractConfigValue& pieces);

    virtual AbstractConfigValuePtr resolve_substitutions(const ResolveContextPtr& context)
    override;

    virtual ResolveStatus resolve_status() override;

    /// _when you graft a substitution into another object, you have to prefix
    /// it with the location in that object where you grafted it; but save
    /// prefix_length so system property and env variable lookups don't get
    /// broken.
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

private:
    VectorAbstractConfigValue pieces;
};

}

#endif // CONFIG_CONFIG_CONCATENATION_HPP
