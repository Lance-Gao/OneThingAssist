
#ifndef CONFIG_SIMPLE_CONFIG_ORIGIN_HPP
#define CONFIG_SIMPLE_CONFIG_ORIGIN_HPP

#include "detail/config_base.hpp"
#include "config_origin.hpp"

namespace config {

///
/// _it would be cleaner to have a class hierarchy for various origin types,
/// but was hoping this would be enough simpler to be a little messy. eh.
///
class SimpleConfigOrigin : public virtual ConfigOrigin, public ConfigBase {
public:
    CONFIG_CLASS(SimpleConfigOrigin);

    SimpleConfigOrigin(const std::string& description,
                          int32_t line_number,
                          int32_t end_line_number,
                          OriginType origin_type,
                          const VectorString& comments_or_null);

public:
    static SimpleConfigOriginPtr new_simple(const std::string& description);
    static SimpleConfigOriginPtr new_file(const std::string& filename);

    SimpleConfigOriginPtr set_line_number(int32_t line_number);
    SimpleConfigOriginPtr set_comments(const VectorString& comments);

    virtual std::string description() override;

    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;
    virtual std::string to_string() override;

    virtual std::string filename() override;
    virtual int32_t line_number() override;
    virtual VectorString comments() override;

private:
    static SimpleConfigOriginPtr merge_two(const SimpleConfigOriginPtr& a,
            const SimpleConfigOriginPtr& b);
    static uint32_t similarity(const SimpleConfigOriginPtr& a,
                               const SimpleConfigOriginPtr& b);
    static SimpleConfigOriginPtr merge_three(const SimpleConfigOriginPtr& a,
            const SimpleConfigOriginPtr& b,
            const SimpleConfigOriginPtr& c);

public:
    static ConfigOriginPtr merge_origins(const ConfigOriginPtr& a,
                                            const ConfigOriginPtr& b);
    static ConfigOriginPtr merge_origins(const VectorAbstractConfigValue& stack);
    static ConfigOriginPtr merge_origins(const VectorConfigOrigin& stack);

private:
    std::string description_;
    int32_t line_number_;
    int32_t end_line_number;
    OriginType origin_type;
    VectorString comments_or_null;
};

}

#endif // CONFIG_SIMPLE_CONFIG_ORIGIN_HPP
