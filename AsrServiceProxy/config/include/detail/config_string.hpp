
#ifndef CONFIG_CONFIG_STRING_HPP
#define CONFIG_CONFIG_STRING_HPP

#include "detail/abstract_config_value.hpp"

namespace config {

class ConfigString : public AbstractConfigValue {
public:
    CONFIG_CLASS(ConfigString);

    ConfigString(const ConfigOriginPtr& origin, const std::string& value);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual std::string transform_to_string() override;

protected:
    virtual void render(std::string& s,
                        uint32_t indent,
                        const ConfigRenderOptionsPtr& options);
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin) override;

private:
    std::string value;
};

}

#endif // CONFIG_CONFIG_STRING_HPP
