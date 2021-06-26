
#include "detail/tokens.hpp"
#include "detail/token.hpp"
#include "detail/token_type.hpp"
#include "detail/abstract_config_value.hpp"
#include "detail/config_string.hpp"
#include "detail/config_number.hpp"
#include "detail/config_null.hpp"
#include "detail/config_boolean.hpp"
#include "config_value_type.hpp"

namespace config {

ValueToken::ValueToken(const AbstractConfigValuePtr& value) :
    Token(TokenType::VALUE, value->origin()),
    value_(value) {
}

AbstractConfigValuePtr ValueToken::value() {
    return value_;
}

std::string ValueToken::to_string() {
    std::ostringstream stream;
    ConfigVariant u = value()->unwrapped();
    stream << "'" << boost::apply_visitor(_variant_string(), u);
    stream << "' (" << ConfigValueTypeEnum::name(value_->value_type()) << ")";
    return stream.str();
}

bool ValueToken::can_equal(const ConfigVariant& other) {
    return instanceof<ValueToken>(other);
}

bool ValueToken::equals(const ConfigVariant& other) {
    return Token::equals(other) && static_get<ValueToken>(other)->value_->equals(value_);
}

uint32_t ValueToken::hash_code() {
    uint32_t value_hash = 0;

    if (value_->value_type() == ConfigValueType::BOOLEAN) {
        value_hash = variant_get<bool>(value_->unwrapped()) ? 1231 : 1237;
    } else {
        value_hash = value_->hash_code();
    }

    return 41 * (41 + Token::hash_code()) + value_hash;
}

LineToken::LineToken(const ConfigOriginPtr& origin) :
    Token(TokenType::NEWLINE, origin) {
}

std::string LineToken::to_string() {
    return "'\\n'@" + boost::lexical_cast<std::string>(line_number());
}

bool LineToken::can_equal(const ConfigVariant& other) {
    return instanceof<LineToken>(other);
}

bool LineToken::equals(const ConfigVariant& other) {
    return Token::equals(other) && static_get<LineToken>(other)->line_number() == line_number();
}

uint32_t LineToken::hash_code() {
    return 41 * (41 + Token::hash_code()) + line_number();
}

UnquotedTextToken::UnquotedTextToken(const ConfigOriginPtr& origin, const std::string& s) :
    Token(TokenType::UNQUOTED_TEXT, origin),
    value_(s) {
}

std::string UnquotedTextToken::value() {
    return value_;
}

std::string UnquotedTextToken::to_string() {
    return "'" + value_ + "'";
}

bool UnquotedTextToken::can_equal(const ConfigVariant& other) {
    return instanceof<UnquotedTextToken>(other);
}

bool UnquotedTextToken::equals(const ConfigVariant& other) {
    return Token::equals(other) && static_get<UnquotedTextToken>(other)->value_ == value_;
}

uint32_t UnquotedTextToken::hash_code() {
    return 41 * (41 + Token::hash_code()) + std::hash<std::string>()(value_);
}

ProblemToken::ProblemToken(const ConfigOriginPtr& origin, const std::string& what,
                               const std::string& message, bool suggest_quotes) :
    Token(TokenType::PROBLEM, origin),
    what_(what),
    message_(message),
    suggest_quotes_(suggest_quotes) {
}

std::string ProblemToken::what() {
    return what_;
}

std::string ProblemToken::message() {
    return message_;
}

bool ProblemToken::suggest_quotes() {
    return suggest_quotes_;
}

std::string ProblemToken::to_string() {
    return "'" + what_ + "' (" + message_ + ")";
}

bool ProblemToken::can_equal(const ConfigVariant& other) {
    return instanceof<ProblemToken>(other);
}

bool ProblemToken::equals(const ConfigVariant& other) {
    return Token::equals(other) &&
           static_get<ProblemToken>(other)->what_ == what_ &&
           static_get<ProblemToken>(other)->message_ == message_ &&
           static_get<ProblemToken>(other)->suggest_quotes_ == suggest_quotes_;
}

uint32_t ProblemToken::hash_code() {
    uint32_t h = 41 * (41 + Token::hash_code());
    h = 41 * (h + std::hash<std::string>()(what_));
    h = 41 * (h + std::hash<std::string>()(message_));
    h = 41 * (h + std::hash<bool>()(suggest_quotes_));
    return h;
}

CommentToken::CommentToken(const ConfigOriginPtr& origin, const std::string& text) :
    Token(TokenType::COMMENT, origin),
    text_(text) {
}

std::string CommentToken::text() {
    return text_;
}

std::string CommentToken::to_string() {
    return "'#" + text_ + "' (comment)";
}

bool CommentToken::can_equal(const ConfigVariant& other) {
    return instanceof<CommentToken>(other);
}

bool CommentToken::equals(const ConfigVariant& other) {
    return Token::equals(other) && static_get<CommentToken>(other)->text_ == text_;
}

uint32_t CommentToken::hash_code() {
    return 41 * (41 + Token::hash_code()) + std::hash<std::string>()(text_);
}

SubstitutionToken::SubstitutionToken(const ConfigOriginPtr& origin, bool optional,
        const VectorToken& expression) :
    Token(TokenType::SUBSTITUTION, origin),
    optional_(optional),
    value_(expression) {
}

bool SubstitutionToken::optional() {
    return optional_;
}

VectorToken SubstitutionToken::value() {
    return value_;
}

std::string SubstitutionToken::to_string() {
    std::ostringstream stream;

    for (auto& t : value_) {
        stream << t->to_string();
    }

    return "'${" + stream.str() + "}'";
}

bool SubstitutionToken::can_equal(const ConfigVariant& other) {
    return instanceof<SubstitutionToken>(other);
}

bool SubstitutionToken::equals(const ConfigVariant& other) {
    return Token::equals(other) &&
           this->value_.size() == static_get<SubstitutionToken>(other)->value_.size() &&
           std::equal(this->value_.begin(), this->value_.end(),
                      static_get<SubstitutionToken>(other)->value_.begin(), config_equals<TokenPtr>());
}

uint32_t SubstitutionToken::hash_code() {
    size_t hash = 41 * (41 + Token::hash_code());

    for (auto& t : value_) {
        boost::hash_combine(hash, t->hash_code());
    }

    return static_cast<uint32_t>(hash);
}

bool Tokens::is_value(const TokenPtr& token) {
    return instanceof<ValueToken>(token);
}

AbstractConfigValuePtr Tokens::get_value(const TokenPtr& token) {
    if (instanceof<ValueToken>(token)) {
        return std::static_pointer_cast<ValueToken>(token)->value();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get value of non-value token " +
                                              token->to_string());
    }
}

bool Tokens::is_value_with_type(const TokenPtr& t, ConfigValueType value_type) {
    return is_value(t) && get_value(t)->value_type() == value_type;
}

bool Tokens::is_newline(const TokenPtr& token) {
    return instanceof<LineToken>(token);
}

bool Tokens::is_problem(const TokenPtr& token) {
    return instanceof<ProblemToken>(token);
}

std::string Tokens::get_problem_what(const TokenPtr& token) {
    if (instanceof<ProblemToken>(token)) {
        return std::static_pointer_cast<ProblemToken>(token)->what();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get problem what from " + token->to_string());
    }
}

std::string Tokens::get_problem_message(const TokenPtr& token) {
    if (instanceof<ProblemToken>(token)) {
        return std::static_pointer_cast<ProblemToken>(token)->message();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get problem message from " + token->to_string());
    }
}

bool Tokens::get_problem_suggest_quotes(const TokenPtr& token) {
    if (instanceof<ProblemToken>(token)) {
        return std::static_pointer_cast<ProblemToken>(token)->suggest_quotes();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get problem suggest_quotes from " +
                                              token->to_string());
    }
}

bool Tokens::is_comment(const TokenPtr& token) {
    return instanceof<CommentToken>(token);
}

std::string Tokens::get_comment_text(const TokenPtr& token) {
    if (instanceof<CommentToken>(token)) {
        return std::static_pointer_cast<CommentToken>(token)->text();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get comment text from " + token->to_string());
    }
}

bool Tokens::is_unquoted_text(const TokenPtr& token) {
    return instanceof<UnquotedTextToken>(token);
}

std::string Tokens::get_unquoted_text(const TokenPtr& token) {
    if (instanceof<UnquotedTextToken>(token)) {
        return std::static_pointer_cast<UnquotedTextToken>(token)->value();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get unquoted text from " + token->to_string());
    }
}

bool Tokens::is_substitution(const TokenPtr& token) {
    return instanceof<SubstitutionToken>(token);
}

VectorToken Tokens::get_substitution_path_expression(const TokenPtr& token) {
    if (instanceof<SubstitutionToken>(token)) {
        return std::static_pointer_cast<SubstitutionToken>(token)->value();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get substitution from " + token->to_string());
    }
}

bool Tokens::get_substitution_optional(const TokenPtr& token) {
    if (instanceof<SubstitutionToken>(token)) {
        return std::static_pointer_cast<SubstitutionToken>(token)->optional();
    } else {
        throw ConfigExceptionBugOrBroken("tried to get substitution optionality from " +
                                              token->to_string());
    }
}

TokenPtr Tokens::start() {
    static auto start = Token::new_without_origin(TokenType::START, "start of file");
    return start;
}

TokenPtr Tokens::end() {
    static auto end = Token::new_without_origin(TokenType::END, "end of file");
    return end;
}

TokenPtr Tokens::comma() {
    static auto comma = Token::new_without_origin(TokenType::COMMA, "','");
    return comma;
}

TokenPtr Tokens::equals() {
    static auto equals = Token::new_without_origin(TokenType::EQUALS, "'='");
    return equals;
}

TokenPtr Tokens::colon() {
    static auto colon = Token::new_without_origin(TokenType::COLON, "':'");
    return colon;
}

TokenPtr Tokens::open_curly() {
    static auto open_curly = Token::new_without_origin(TokenType::OPEN_CURLY, "'{'");
    return open_curly;
}

TokenPtr Tokens::close_curly() {
    static auto close_curly = Token::new_without_origin(TokenType::CLOSE_CURLY, "'}'");
    return close_curly;
}

TokenPtr Tokens::open_square() {
    static auto open_square = Token::new_without_origin(TokenType::OPEN_SQUARE, "'['");
    return open_square;
}

TokenPtr Tokens::close_square() {
    static auto close_square = Token::new_without_origin(TokenType::CLOSE_SQUARE, "']'");
    return close_square;
}

TokenPtr Tokens::plus_equals() {
    static auto plus_equals = Token::new_without_origin(TokenType::PLUS_EQUALS, "'+='");
    return plus_equals;
}

TokenPtr Tokens::new_line(const ConfigOriginPtr& origin) {
    return LineToken::make_instance(origin);
}

TokenPtr Tokens::new_problem(const ConfigOriginPtr& origin, const std::string& what,
                                const std::string& message, bool suggest_quotes) {
    return ProblemToken::make_instance(origin, what, message, suggest_quotes);
}

TokenPtr Tokens::new_comment(const ConfigOriginPtr& origin, const std::string& text) {
    return CommentToken::make_instance(origin, text);
}

TokenPtr Tokens::new_unquoted_text(const ConfigOriginPtr& origin, const std::string& s) {
    return UnquotedTextToken::make_instance(origin, s);
}

TokenPtr Tokens::new_substitution(const ConfigOriginPtr& origin, bool optional,
                                     const VectorToken& expression) {
    return SubstitutionToken::make_instance(origin, optional, expression);
}

TokenPtr Tokens::new_value(const AbstractConfigValuePtr& value) {
    return ValueToken::make_instance(value);
}

TokenPtr Tokens::new_string(const ConfigOriginPtr& origin, const std::string& value) {
    return new_value(ConfigString::make_instance(origin, value));
}

TokenPtr Tokens::new_int(const ConfigOriginPtr& origin, int32_t value,
                            const std::string& original_text) {
    return new_value(ConfigNumber::new_number(origin, static_cast<int64_t>(value), original_text));
}

TokenPtr Tokens::new_double(const ConfigOriginPtr& origin, double value,
                               const std::string& original_text) {
    return new_value(ConfigNumber::new_number(origin, value, original_text));
}

TokenPtr Tokens::new_int64(const ConfigOriginPtr& origin, int64_t value,
                              const std::string& original_text) {
    return new_value(ConfigNumber::new_number(origin, value, original_text));
}

TokenPtr Tokens::new_null(const ConfigOriginPtr& origin) {
    return new_value(ConfigNull::make_instance(origin));
}

TokenPtr Tokens::new_boolean(const ConfigOriginPtr& origin, bool value) {
    return new_value(ConfigBoolean::make_instance(origin, value));
}

}
