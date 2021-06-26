
#include "detail/token.hpp"
#include "detail/token_type.hpp"
#include "detail/variant_utils.hpp"
#include "config_exception.hpp"
#include "config_origin.hpp"

namespace config {

Token::Token(TokenType token_type, const ConfigOriginPtr& origin,
               const std::string& debug_string) :
    token_type_(token_type),
    debug_string_(debug_string),
    origin_(origin) {
}

TokenPtr Token::new_without_origin(TokenType token_type, const std::string& debug_string) {
    return make_instance(token_type, nullptr, debug_string);
}

TokenType Token::token_type() {
    return token_type_;
}

ConfigOriginPtr Token::origin() {
    // code is only supposed to call origin() on token types that are
    // expected to have an origin.
    if (!origin_) {
        throw ConfigExceptionBugOrBroken("tried to get origin from token that doesn't have one: " +
                                              to_string());
    }

    return origin_;
}

int32_t Token::line_number() {
    if (origin_) {
        return origin_->line_number();
    } else {
        return -1;
    }
}

std::string Token::to_string() {
    if (!debug_string_.empty()) {
        return debug_string_;
    } else {
        return TokenTypeEnum::name(token_type_);
    }
}

bool Token::can_equal(const ConfigVariant& other) {
    return instanceof<Token>(other);
}

bool Token::equals(const ConfigVariant& other) {
    if (instanceof<Token>(other)) {
        // origin is deliberately left out
        return can_equal(other) && this->token_type_ == dynamic_get<Token>(other)->token_type_;
    } else {
        return false;
    }
}

uint32_t Token::hash_code() {
    // origin is deliberately left out
    return std::hash<uint32_t>()(static_cast<uint32_t>(token_type_));
}

}

