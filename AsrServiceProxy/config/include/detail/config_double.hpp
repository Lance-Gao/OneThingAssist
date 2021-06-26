
#ifndef CONFIG_CONFIG_DOUBLE_HPP
#define CONFIG_CONFIG_DOUBLE_HPP

#include "detail/config_number.hpp"

namespace config {

class ConfigDouble : public ConfigNumber {
public:
    CONFIG_CLASS(ConfigDouble);

    ConfigDouble(const ConfigOriginPtr& origin, double value, const std::string& original_text);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual std::string transform_to_string() override;

protected:
    virtual int64_t int64_value() override;
    virtual double double_value() override;
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin);

private:
    double value;
};

}

#endif // CONFIG_CONFIG_DOUBLE_HPP
