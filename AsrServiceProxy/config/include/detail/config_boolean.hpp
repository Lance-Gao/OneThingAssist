
#ifndef CONFIG_CONFIG_BOOLEAN_HPP
#define CONFIG_CONFIG_BOOLEAN_HPP

#include "detail/abstract_config_value.hpp"

namespace config {

class ConfigBoolean : public AbstractConfigValue {
public:
    CONFIG_CLASS(ConfigBoolean);

    ConfigBoolean(const ConfigOriginPtr& origin, bool value);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual std::string transform_to_string() override;

protected:
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

private:
    bool value;
};

}

#endif // CONFIG_CONFIG_BOOLEAN_HPP
