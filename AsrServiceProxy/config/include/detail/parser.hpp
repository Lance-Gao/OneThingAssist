
#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "detail/config_base.hpp"
#include "config_exception.hpp"

namespace config {

class Parser : public ConfigBase {
public:
    CONFIG_CLASS(Parser);

    static AbstractConfigValuePtr parse(const TokenIteratorPtr& tokens,
                                            const ConfigOriginPtr& origin,
                                            const ConfigParseOptionsPtr& options,
                                            const ConfigIncludeContextPtr& include_context);

private:
    static void add_path_text(VectorElement& buf, bool was_quoted, const std::string& new_text);

    static PathPtr parse_path_expression(const TokenIteratorPtr& expression,
                                           const ConfigOriginPtr& origin,
                                           const std::string& original_text = "");

public:
    static PathPtr parse_path(const std::string& path);

private:
    /// _the idea is to see if the string has any chars that might require the
    /// full parser to deal with.
    static bool has_unsafe_chars(const std::string& s);

    static void append_path_string(const PathBuilderPtr& pb, const std::string& s);

    /// _do something much faster than the full parser if
    /// we just have something like "foo" or "foo.bar"
    static PathPtr speculative_fast_parse_path(const std::string& path);

    friend class ParseContext;
};

class TokenWithComments : public ConfigBase {
public:
    CONFIG_CLASS(TokenWithComments);

    TokenWithComments(const TokenPtr& token, const VectorToken& comments = VectorToken());

    TokenWithCommentsPtr prepend(const VectorToken& earlier);
    SimpleConfigOriginPtr set_comments(const SimpleConfigOriginPtr& origin);

    virtual std::string to_string() override;

private:
    TokenPtr token;
    VectorToken comments;

    friend class ParseContext;
};

class ParseContext : public ConfigBase {
public:
    CONFIG_CLASS(ParseContext);

    ParseContext(ConfigSyntax flavor, const ConfigOriginPtr& origin,
                   const TokenIteratorPtr& tokens, const FullIncluderPtr& includer,
                   const ConfigIncludeContextPtr& include_context);

private:
    void consolidate_comment_block(const TokenPtr& comment_token);
    TokenWithCommentsPtr pop_token();
    TokenWithCommentsPtr next_token();
    void put_back(const TokenWithCommentsPtr& token);
    TokenWithCommentsPtr next_token_ignoring_newline();
    bool check_element_separator();

    // _merge a bunch of adjacent values into one
    // value; change unquoted text into a string
    // value.
    void consolidate_value_tokens();

    static SubstitutionExpressionPtr token_to_substitution_expression(const TokenPtr& value_token);

    ConfigOriginPtr line_origin();

    ConfigExceptionParse parse_error(const std::string& message);

    std::string previous_field_name(const PathPtr& last_path);
    PathPtr full_current_path();
    std::string previous_field_name();

    std::string add_key_name(const std::string& message);
    std::string add_quote_suggestion(const std::string& bad_token,
                                     const std::string& message);
    std::string add_quote_suggestion(const PathPtr& last_path,
                                     bool inside_equals,
                                     const std::string& bad_token,
                                     const std::string& message);

    AbstractConfigValuePtr parse_value(const TokenWithCommentsPtr& t);

    static AbstractConfigObjectPtr create_value_under_path(const PathPtr& path,
            const AbstractConfigValuePtr& value);

    PathPtr parse_key(const TokenWithCommentsPtr& token);

    static bool is_include_keyword(const TokenPtr& t);
    static bool is_unquoted_whitespace(const TokenPtr& t);

    void parse_include(MapAbstractConfigValue& values);

    bool is_key_value_separator_token(const TokenPtr& t);

    AbstractConfigObjectPtr parse_object(bool had_open_curly);
    SimpleConfigList parse_array();

public:
    AbstractConfigValuePtr parse();

private:
    int32_t line_number;
    StackTokenWithComments buffer;
    TokenIteratorPtr tokens;
    FullIncluderPtr includer;
    ConfigIncludeContextPtr include_context;
    ConfigSyntax flavor;
    ConfigOriginPtr base_origin;
    StackPath path_stack;
    int32_t equals_count;
};

class Element : public ConfigBase {
public:
    CONFIG_CLASS(Element);

    Element(const std::string& initial, bool can_be_empty);

    virtual std::string to_string() override;

    std::string element;
    // an element can be empty if it has a quoted empty string "" in it
    bool can_be_empty;
};

}

#endif // CONFIG_PARSER_HPP
