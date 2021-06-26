
#ifndef CONFIG_CONFIG_INT64_HPP
#define CONFIG_CONFIG_INT64_HPP

#include "detail/config_number.hpp"

namespace config {

class ConfigInt64 : public ConfigNumber {
public:
    CONFIG_CLASS(ConfigInt64);

    ConfigInt64(const ConfigOriginPtr& origin, int64_t value, const std::string& original_text);

    virtual ConfigValueType value_type() override;
    virtual ConfigVariant unwrapped() override;
    virtual std::string transform_to_string() override;

protected:
    virtual int64_t int64_value() override;
    virtual double double_value() override;
    virtual AbstractConfigValuePtr new_copy(const ConfigOriginPtr& origin);

private:
    int64_t value;
};

}

#endif // CONFIG_CONFIG_INT64_HPP
