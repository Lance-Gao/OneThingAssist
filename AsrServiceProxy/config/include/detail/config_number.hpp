
#ifndef CONFIG_CONFIG_NUMBER_HPP
#define CONFIG_CONFIG_NUMBER_HPP

#include "detail/abstract_config_value.hpp"

namespace config {

class ConfigNumber : public AbstractConfigValue {
public:
    CONFIG_CLASS(ConfigNumber);

protected:
    ConfigNumber(const ConfigOriginPtr& origin, const std::string& original_text);

public:
    virtual ConfigVariant unwrapped() = 0;

    /// _alternative to unwrapping the value to a ConfigVariant.
    template <typename _t> _t unwrapped() {
        return variant_get<_t>(unwrapped());
    }

    virtual std::string transform_to_string() override;

    int32_t int_value_range_checked(const std::string& path);

    virtual int64_t int64_value() = 0;
    virtual double double_value() = 0;

private:
    bool is_whole();

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

    static ConfigNumberPtr new_number(const ConfigOriginPtr& origin,
                                         int64_t number,
                                         const std::string& original_text);
    static ConfigNumberPtr new_number(const ConfigOriginPtr& origin,
                                         double number,
                                         const std::string& original_text);

protected:
    /// _this is so when we concatenate a number into a string (say it appears in
    /// a sentence) we always have it exactly as the person typed it into the
    /// config file. _it's purely cosmetic; equals/hash_code don't consider this
    /// for example.
    std::string original_text;
};

}

#endif // CONFIG_CONFIG_NUMBER_HPP
