
#ifndef CONFIG_TOKEN_TYPE_H_PP
#define CONFIG_TOKEN_TYPE_H_PP

#include "config_types.hpp"

namespace config {

enum class TokenType : public uint32_t {
    START,
    END,
    COMMA,
    EQUALS,
    COLON,
    OPEN_CURLY,
    CLOSE_CURLY,
    OPEN_SQUARE,
    CLOSE_SQUARE,
    VALUE,
    NEWLINE,
    UNQUOTED_TEXT,
    SUBSTITUTION,
    PROBLEM,
    COMMENT,
    PLUS_EQUALS
};

class TokenTypeEnum {
public:
    static std::string name(TokenType token_type);
};

}

#endif // CONFIG_TOKEN_TYPE_H_PP
