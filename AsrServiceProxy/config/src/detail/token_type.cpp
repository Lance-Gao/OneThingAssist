
#include "detail/token_type.hpp"

namespace config {

std::string TokenTypeEnum::name(TokenType value_type) {
    typedef std::map<TokenType, std::string> TokenType_name;
    static TokenType_name names = {
        {TokenType::START, "start"},
        {TokenType::END, "end"},
        {TokenType::COMMA, "comma"},
        {TokenType::EQUALS, "equals"},
        {TokenType::COLON, "colon"},
        {TokenType::OPEN_CURLY, "open_curly"},
        {TokenType::CLOSE_CURLY, "close_curly"},
        {TokenType::OPEN_SQUARE, "open_square"},
        {TokenType::CLOSE_SQUARE, "close_square"},
        {TokenType::VALUE, "value"},
        {TokenType::NEWLINE, "newline"},
        {TokenType::UNQUOTED_TEXT, "unquoted_text"},
        {TokenType::SUBSTITUTION, "substitution"},
        {TokenType::PROBLEM, "problem"},
        {TokenType::COMMENT, "comment"},
        {TokenType::PLUS_EQUALS, "plus_equals"}
    };
    return names[value_type];
}

}
