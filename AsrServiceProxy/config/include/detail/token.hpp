
#ifndef CONFIG_TOKEN_HPP
#define CONFIG_TOKEN_HPP

#include "detail/config_base.hpp"

namespace config {

class Token : public ConfigBase {
public:
    CONFIG_CLASS(Token);

    Token(TokenType token_type,
           const ConfigOriginPtr& origin,
           const std::string& debug_string = "");

    /// _this is used for singleton tokens like comma or open_curly
    static TokenPtr new_without_origin(TokenType token_type,
                                         const std::string& debug_string);

    TokenType token_type();
    ConfigOriginPtr origin();

    int32_t line_number();

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other);

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    TokenType token_type_;
    std::string debug_string_;
    ConfigOriginPtr origin_;
};

}

#endif // CONFIG_TOKEN_HPP
