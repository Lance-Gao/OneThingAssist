
#ifndef CONFIG_CONFIG_INT_HPP
#define CONFIG_CONFIG_INT_HPP

#include "detail/config_number.hpp"

namespace config {

class ConfigInt : public ConfigNumber {
public:
    CONFIG_CLASS(ConfigInt);

    ConfigInt(const ConfigOriginPtr& origin, int32_t value, const std::string& original_text);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual std::string transform_to_string() override;

protected:
    virtual int64_t int64_value() override;
    virtual double double_value() override;
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin);

private:
    int32_t value;
};

}

#endif // CONFIG_CONFIG_INT_HPP
