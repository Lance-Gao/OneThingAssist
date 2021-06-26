
#ifndef CONFIG_TOKENS_HPP
#define CONFIG_TOKENS_HPP

#include "detail/token.hpp"

namespace config {

class Tokens : public ConfigBase {
public:
    CONFIG_CLASS(Tokens);

    static bool is_value(const TokenPtr& token);
    static AbstractConfigValuePtr get_value(const TokenPtr& token);
    static bool is_value_with_type(const TokenPtr& t, ConfigValueType value_type);
    static bool is_newline(const TokenPtr& token);
    static bool is_problem(const TokenPtr& token);
    static std::string get_problem_what(const TokenPtr& token);
    static std::string get_problem_message(const TokenPtr& token);
    static bool get_problem_suggest_quotes(const TokenPtr& token);
    static bool is_comment(const TokenPtr& token);
    static std::string get_comment_text(const TokenPtr& token);
    static bool is_unquoted_text(const TokenPtr& token);
    static std::string get_unquoted_text(const TokenPtr& token);
    static bool is_substitution(const TokenPtr& token);
    static VectorToken get_substitution_path_expression(const TokenPtr& token);
    static bool get_substitution_optional(const TokenPtr& token);

    static TokenPtr start();
    static TokenPtr end();
    static TokenPtr comma();
    static TokenPtr equals();
    static TokenPtr colon();
    static TokenPtr open_curly();
    static TokenPtr close_curly();
    static TokenPtr open_square();
    static TokenPtr close_square();
    static TokenPtr plus_equals();

    static TokenPtr new_line(const ConfigOriginPtr& origin);
    static TokenPtr new_problem(const ConfigOriginPtr& origin,
                                  const std::string& what,
                                  const std::string& message,
                                  bool suggest_quotes);
    static TokenPtr new_comment(const ConfigOriginPtr& origin,
                                  const std::string& text);
    static TokenPtr new_unquoted_text(const ConfigOriginPtr& origin,
                                        const std::string& s);
    static TokenPtr new_substitution(const ConfigOriginPtr& origin,
                                       bool optional,
                                       const VectorToken& expression);
    static TokenPtr new_value(const AbstractConfigValuePtr& value);
    static TokenPtr new_string(const ConfigOriginPtr& origin,
                                 const std::string& value);
    static TokenPtr new_int(const ConfigOriginPtr& origin,
                              int32_t value,
                              const std::string& original_text);
    static TokenPtr new_double(const ConfigOriginPtr& origin,
                                 double value,
                                 const std::string& original_text);
    static TokenPtr new_int64(const ConfigOriginPtr& origin,
                                int64_t value,
                                const std::string& original_text);
    static TokenPtr new_null(const ConfigOriginPtr& origin);
    static TokenPtr new_boolean(const ConfigOriginPtr& origin,
                                  bool value);
};

class ValueToken : public Token {
public:
    CONFIG_CLASS(ValueToken);

    ValueToken(const AbstractConfigValuePtr& value);

    AbstractConfigValuePtr value();

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    AbstractConfigValuePtr value_;
};

class LineToken : public Token {
public:
    CONFIG_CLASS(LineToken);

    LineToken(const ConfigOriginPtr& origin);

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;
};

///
/// _this is not a ValueToken, because it requires special processing
///
class UnquotedTextToken : public Token {
public:
    CONFIG_CLASS(UnquotedTextToken);

    UnquotedTextToken(const ConfigOriginPtr& origin, const std::string& s);

    std::string value();

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    std::string value_;
};

class ProblemToken : public Token {
public:
    CONFIG_CLASS(ProblemToken);

    ProblemToken(const ConfigOriginPtr& origin,
                   const std::string& what,
                   const std::string& message,
                   bool suggest_quotes);

    std::string what();
    std::string message();
    bool suggest_quotes();

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    std::string what_;
    std::string message_;
    bool suggest_quotes_;
};

class CommentToken : public Token {
public:
    CONFIG_CLASS(CommentToken);

    CommentToken(const ConfigOriginPtr& origin, const std::string& text);

    std::string text();

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    std::string text_;
};

///
/// _this is not a ValueToken, because it requires special processing
///
class SubstitutionToken : public Token {
public:
    CONFIG_CLASS(SubstitutionToken);

    SubstitutionToken(const ConfigOriginPtr& origin,
                        bool optional,
                        const VectorToken& expression);

    bool optional();
    VectorToken value();

    virtual std::string to_string() override;

protected:
    virtual bool can_equal(const ConfigVariant& other) override;

public:
    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

private:
    bool optional_;
    VectorToken value_;
};

}

#endif // CONFIG_TOKENS_HPP
