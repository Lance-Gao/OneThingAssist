
#ifndef CONFIG_SUBSTITUTION_EXPRESSION_HPP
#define CONFIG_SUBSTITUTION_EXPRESSION_HPP

#include "detail/config_base.hpp"

namespace config {

class SubstitutionExpression : public ConfigBase {
public:
    CONFIG_CLASS(SubstitutionExpression);

    SubstitutionExpression(const PathPtr& path, bool optional);

    PathPtr path();
    bool optional();

    SubstitutionExpressionPtr change_path(const PathPtr& new_path);

    virtual std::string to_string() override;
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    PathPtr path_;
    bool optional_;
};

}

#endif // CONFIG_SUBSTITUTION_EXPRESSION_HPP
