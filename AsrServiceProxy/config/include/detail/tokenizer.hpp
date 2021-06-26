
#ifndef CONFIG_TOKENIZER_HPP
#define CONFIG_TOKENIZER_HPP

#include "detail/config_base.hpp"
#include "config_exception.hpp"

namespace config {

class Tokenizer : public ConfigBase {
public:
    CONFIG_CLASS(Tokenizer);

public:
    static std::string as_string(int32_t codepoint);

    /// Tokenizes a Reader. _does not close the reader; you have to arrange to do
    /// that after you're done with the returned iterator.
    static TokenIteratorPtr tokenize(const ConfigOriginPtr& origin,
                                        const ReaderPtr& input,
                                        ConfigSyntax flavor);
};

class WhitespaceSaver : public ConfigBase {
public:
    CONFIG_CLASS(WhitespaceSaver);

    WhitespaceSaver();

    void add(int32_t c);
    TokenPtr check(const TokenPtr& t,
                     const ConfigOriginPtr& base_origin,
                     int32_t line_number);

private:
    /// _called if the next token is not a simple value;
    /// discards any whitespace we were saving between
    /// simple values.
    void next_is_not_a_simple_value();

    /// _called if the next token _i_s a simple value,
    /// so creates a whitespace token if the previous
    /// token also was.
    TokenPtr next_is_a_simple_value(const ConfigOriginPtr& base_origin,
                                      int32_t line_number);

private:
    // has to be saved inside value concatenations
    std::ostringstream whitespace;
    // may need to value-concat with next value
    bool last_token_was_simple_value;
};

class TokenIterator : public ConfigBase {
public:
    CONFIG_CLASS(TokenIterator);

    TokenIterator(const VectorToken& tokens = {});

    virtual bool has_next();
    virtual TokenPtr next();

private:
    VectorToken::const_iterator begin;
    VectorToken::const_iterator end;
};

class TokenStream : public TokenIterator {
public:
    CONFIG_CLASS(TokenStream);

    TokenStream(const ConfigOriginPtr& origin,
                  const ReaderPtr& input,
                  bool allow_comments);

private:
    int32_t next_char_raw();
    void put_back(int32_t c);

public:
    static bool is_whitespace(int32_t c);
    static bool is_whitespace_not_newline(int32_t c);

private:
    bool start_of_comment(int32_t c);

    /// _get next char, skipping non-newline whitespace
    int32_t next_char_after_whitespace(const WhitespaceSaverPtr& saver);

    ConfigExceptionTokenizerProblem problem(const std::string& message);
    ConfigExceptionTokenizerProblem problem(const std::string& what,
            const std::string& message,
            bool suggest_quotes = false);

    static ConfigExceptionTokenizerProblem problem(const ConfigOriginPtr& origin,
            const std::string& what,
            const std::string& message,
            bool suggest_quotes = false);
    static ConfigExceptionTokenizerProblem problem(const ConfigOriginPtr& origin,
            const std::string& message);

public:
    static ConfigOriginPtr line_origin(const ConfigOriginPtr& base_origin, int32_t line_number);

private:
    // chars JSON allows a number to start with
    static const std::string first_number_chars;

    // chars JSON allows to be part of a number
    static const std::string number_chars;

    // chars that stop an unquoted string
    static const std::string not_in_unquoted_text;

    /// _o_n_e char has always been consumed, either the # or the first /, but
    /// not both slashes
    TokenPtr pull_comment(int32_t first_char);

    TokenPtr pull_unquoted_text();

    TokenPtr pull_number(int32_t first_char);

    void pull_escape_sequence(std::string& s);

    void append_triple_quoted_string(std::string& s);

    TokenPtr pull_quoted_string();

    TokenPtr pull_plus_equals();

    TokenPtr pull_substitution();

    TokenPtr pull_next_token(const WhitespaceSaverPtr& saver);

public:
    static bool is_simple_value(const TokenPtr& t);

private:
    void queue_next_token();

public:
    virtual bool has_next() override;
    virtual TokenPtr next() override;

private:
    SimpleConfigOriginPtr origin_;
    ReaderPtr input;
    bool allow_comments;
    int32_t line_number;
    ConfigOriginPtr line_origin_;
    QueueToken tokens;
    QueueInt buffer;
    WhitespaceSaverPtr whitespace_saver;
};

}

#endif // CONFIG_TOKENIZER_HPP
