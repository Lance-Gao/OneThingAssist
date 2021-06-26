
#ifndef CONFIG_CONFIG_NULL_HPP
#define CONFIG_CONFIG_NULL_HPP

#include "detail/abstract_config_value.hpp"

namespace config {

///
/// _this exists because sometimes null is not the same as missing. _specifically,
/// if a value is set to null we can give a better error message (indicating
/// where it was set to null) in case someone asks for the value. _also, null
/// overrides values set "earlier" in the search path, while missing values do
/// not.
///
class ConfigNull : public AbstractConfigValue {
public:
    CONFIG_CLASS(ConfigNull);

    ConfigNull(const ConfigOriginPtr& origin);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual std::string transform_to_string() override;

protected:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;
};

}

#endif // CONFIG_CONFIG_NULL_HPP
