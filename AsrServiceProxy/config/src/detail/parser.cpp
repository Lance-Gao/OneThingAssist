
#include "detail/parser.hpp"
#include "detail/abstract_config_object.hpp"
#include "detail/simple_includer.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/simple_config_object.hpp"
#include "detail/simple_config_list.hpp"
#include "detail/tokens.hpp"
#include "detail/token.hpp"
#include "detail/tokenizer.hpp"
#include "detail/config_string.hpp"
#include "detail/config_reference.hpp"
#include "detail/config_concatenation.hpp"
#include "detail/path.hpp"
#include "detail/path_builder.hpp"
#include "detail/substitution_expression.hpp"
#include "detail/string_reader.hpp"
#include "config_parse_options.hpp"
#include "config_syntax.hpp"
#include "config_value_type.hpp"

namespace config {

AbstractConfigValuePtr Parser::parse(const TokenIteratorPtr& tokens,
        const ConfigOriginPtr& origin,
        const ConfigParseOptionsPtr& options, const ConfigIncludeContextPtr& include_context) {
    auto context = ParseContext::make_instance(options->get_syntax(), origin, tokens,
                   SimpleIncluder::make_full(options->get_includer()), include_context);
    return context->parse();
}

TokenWithComments::TokenWithComments(const TokenPtr& token, const VectorToken& comments) :
    token(token),
    comments(comments) {
}

TokenWithCommentsPtr TokenWithComments::prepend(const VectorToken& earlier) {
    if (comments.empty()) {
        return TokenWithComments::make_instance(token, earlier);
    } else {
        VectorToken merged(earlier);
        merged.insert(merged.end(), comments.begin(), comments.end());
        return TokenWithComments::make_instance(token, merged);
    }
}

SimpleConfigOriginPtr TokenWithComments::set_comments(const SimpleConfigOriginPtr&
        origin) {
    if (comments.empty()) {
        return origin;
    } else {
        VectorString new_comments;

        for (auto& c : comments) {
            new_comments.push_back(Tokens::get_comment_text(c));
        }

        return origin->set_comments(new_comments);
    }
}

std::string TokenWithComments::to_string() {
    // this ends up in user-visible error messages, so we don't want the comments
    return token->to_string();
}

ParseContext::ParseContext(ConfigSyntax flavor, const ConfigOriginPtr& origin,
                               const TokenIteratorPtr& tokens, const FullIncluderPtr& includer,
                               const ConfigIncludeContextPtr& include_context) :
    line_number(1),
    tokens(tokens),
    includer(includer),
    include_context(include_context),
    flavor(flavor),
    base_origin(origin),
    equals_count(0) {
}

void ParseContext::consolidate_comment_block(const TokenPtr& comment_token) {
    // a comment block "goes with" the following token
    // unless it's separated from it by a blank line.
    // we want to build a list of newline tokens followed
    // by a non-newline non-comment token; with all comments
    // associated with that final non-newline non-comment token.
    VectorToken newlines;
    VectorToken comments;

    TokenPtr previous;
    auto next = comment_token;

    while (true) {
        if (Tokens::is_newline(next)) {
            if (previous && Tokens::is_newline(previous)) {
                // blank line; drop all comments to this point and
                // start a new comment block
                comments.clear();
            }

            newlines.push_back(next);
        } else if (Tokens::is_comment(next)) {
            comments.push_back(next);
        } else {
            // a non-newline non-comment token
            break;
        }

        previous = next;
        next = tokens->next();
    }

    // put our concluding token in the queue with all the comments attached
    buffer.push_front(TokenWithComments::make_instance(next, comments));

    // now put all the newlines back in front of it
    for (auto li = newlines.rbegin(); li != newlines.rend(); ++li) {
        buffer.push_front(TokenWithComments::make_instance(*li));
    }
}

TokenWithCommentsPtr ParseContext::pop_token() {
    if (buffer.empty()) {
        auto t = tokens->next();

        if (Tokens::is_comment(t)) {
            consolidate_comment_block(t);
            auto token = buffer.front();
            buffer.pop_front();
            return token;
        } else {
            return TokenWithComments::make_instance(t);
        }
    } else {
        auto token = buffer.front();
        buffer.pop_front();
        return token;
    }
}

TokenWithCommentsPtr ParseContext::next_token() {
    auto with_comments = pop_token();
    auto t = with_comments->token;

    if (Tokens::is_problem(t)) {
        auto origin = t->origin();
        std::string message = Tokens::get_problem_message(t);
        bool suggest_quotes = Tokens::get_problem_suggest_quotes(t);

        if (suggest_quotes) {
            message = add_quote_suggestion(t->to_string(), message);
        } else {
            message = add_key_name(message);
        }

        throw ConfigExceptionParse(origin, message);
    } else {
        if (flavor == ConfigSyntax::JSON) {
            if (Tokens::is_unquoted_text(t)) {
                throw parse_error(add_key_name("_token not allowed in valid JSON: '" +
                                               Tokens::get_unquoted_text(t) + "'"));
            } else if (Tokens::is_substitution(t)) {
                throw parse_error(add_key_name("_substitutions (${} syntax) not allowed in JSON"));
            }
        }

        return with_comments;
    }
}

void ParseContext::put_back(const TokenWithCommentsPtr& token) {
    buffer.push_front(token);
}

TokenWithCommentsPtr ParseContext::next_token_ignoring_newline() {
    auto t = next_token();

    while (Tokens::is_newline(t->token)) {
        // line number tokens have the line that was _ended_ by the
        // newline, so we have to add one.
        line_number = t->token->line_number() + 1;

        t = next_token();
    }

    return t;
}

bool ParseContext::check_element_separator() {
    if (flavor == ConfigSyntax::JSON) {
        auto t = next_token_ignoring_newline();

        if (t->token == Tokens::comma()) {
            return true;
        } else {
            put_back(t);
            return false;
        }
    } else {
        bool saw_separator_or_newline = false;
        auto t = next_token();

        while (true) {
            if (Tokens::is_newline(t->token)) {
                // newline number is the line just ended, so add one
                line_number = t->token->line_number() + 1;
                saw_separator_or_newline = true;

                // we want to continue to also eat
                // a comma if there is one.
            } else if (t->token == Tokens::comma()) {
                return true;
            } else {
                // non-newline-or-comma
                put_back(t);
                return saw_separator_or_newline;
            }

            t = next_token();
        }
    }
}

SubstitutionExpressionPtr ParseContext::token_to_substitution_expression(
    const TokenPtr& value_token) {
    auto expression = Tokens::get_substitution_path_expression(value_token);
    auto path = Parser::parse_path_expression(TokenIterator::make_instance(expression),
                value_token->origin());
    bool optional = Tokens::get_substitution_optional(value_token);
    return SubstitutionExpression::make_instance(path, optional);
}

void ParseContext::consolidate_value_tokens() {
    // this trick is not done in JSON
    if (flavor == ConfigSyntax::JSON) {
        return;
    }

    // create only if we have value tokens
    boost::optional<VectorAbstractConfigValue> values;
    TokenWithCommentsPtr first_value_with_comments;
    // ignore a newline up front
    auto t = next_token_ignoring_newline();

    while (true) {
        AbstractConfigValuePtr v;

        if (Tokens::is_value(t->token)) {
            // if we consolidate_value_tokens() multiple times then
            // this value could be a concatenation, object, array,
            // or substitution already.
            v = Tokens::get_value(t->token);
        } else if (Tokens::is_unquoted_text(t->token)) {
            v = ConfigString::make_instance(t->token->origin(), Tokens::get_unquoted_text(t->token));
        } else if (Tokens::is_substitution(t->token)) {
            v = ConfigReference::make_instance(t->token->origin(),
                                                 token_to_substitution_expression(t->token));
        } else if (t->token == Tokens::open_curly() || t->token == Tokens::open_square()) {
            // there may be newlines _within_ the objects and arrays
            v = parse_value(t);
        } else {
            break;
        }

        if (!v) {
            throw ConfigExceptionBugOrBroken("no value");
        }

        if (!values) {
            values = VectorAbstractConfigValue();
            first_value_with_comments = t;
        }

        values->push_back(v);

        t = next_token(); // but don't consolidate across a newline
    }

    // the last one wasn't a value token
    put_back(t);

    if (!values) {
        return;
    }

    auto consolidated = ConfigConcatenation::concatenate(*values);

    put_back(TokenWithComments::make_instance(Tokens::new_value(consolidated),
             first_value_with_comments->comments));
}

ConfigOriginPtr ParseContext::line_origin() {
    return std::dynamic_pointer_cast<SimpleConfigOrigin>(base_origin)->set_line_number(line_number);
}

ConfigExceptionParse ParseContext::parse_error(const std::string& message) {
    return ConfigExceptionParse(line_origin(), message);
}

std::string ParseContext::previous_field_name(const PathPtr& last_path) {
    if (last_path) {
        return last_path->render();
    } else if (path_stack.empty()) {
        return "";
    } else {
        return path_stack.front()->render();
    }
}

PathPtr ParseContext::full_current_path() {
    PathPtr full;

    // path_stack has top of stack at front
    for (auto& p : path_stack) {
        if (!full) {
            full = p;
        } else {
            full = full->prepend(p);
        }
    }

    return full;
}

std::string ParseContext::previous_field_name() {
    return previous_field_name(nullptr);
}

std::string ParseContext::add_key_name(const std::string& message) {
    std::string previous_field_name_ = previous_field_name();

    if (previous_field_name_.empty()) {
        return "in value for key '" + previous_field_name_ + "': " + message;
    } else {
        return message;
    }
}

std::string ParseContext::add_quote_suggestion(const std::string& bad_token,
        const std::string& message) {
    return add_quote_suggestion(nullptr, equals_count > 0, bad_token, message);
}

std::string ParseContext::add_quote_suggestion(const PathPtr& last_path, bool inside_equals,
        const std::string& bad_token, const std::string& message) {
    std::string previous_field_name_ = previous_field_name(last_path);

    std::string part;

    if (bad_token == Tokens::end()->to_string()) {
        // EOF requires special handling for the error to make sense.
        if (!previous_field_name_.empty()) {
            part = message + " (if you intended '" + previous_field_name_ +
                   "' to be part of a value, instead of a key, " +
                   "try adding double quotes around the whole value";
        } else {
            return message;
        }
    } else {
        if (!previous_field_name_.empty()) {
            part = message + " (if you intended " + bad_token +
                   " to be part of the value for '" + previous_field_name_ +
                   "', try enclosing the value in double quotes";
        } else {
            part = message + " (if you intended " + bad_token +
                   " to be part of a key or string value, "
                   "try enclosing the key or value in double quotes";
        }
    }

    if (inside_equals) {
        return part + ", or you may be able to rename the file .properties rather than .conf)";
    } else {
        return part + ")";
    }
}

AbstractConfigValuePtr ParseContext::parse_value(const TokenWithCommentsPtr& t) {
    AbstractConfigValuePtr v;

    if (Tokens::is_value(t->token)) {
        v = Tokens::get_value(t->token);
    } else if (t->token == Tokens::open_curly()) {
        v = std::static_pointer_cast<AbstractConfigValue>(parse_object(true));
    } else if (t->token == Tokens::open_square()) {
        v = std::static_pointer_cast<AbstractConfigValue>(parse_array());
    } else {
        throw parse_error(add_quote_suggestion(t->token->to_string(),
                                               "_expecting a value but got wrong token: " + t->token->to_string()));
    }

    v = v->with_origin(t->set_comments(std::dynamic_pointer_cast<SimpleConfigOrigin>(v->origin())));

    return v;
}

AbstractConfigObjectPtr ParseContext::create_value_under_path(const PathPtr& path,
        const AbstractConfigValuePtr& value) {
    // for path foo.bar, we are creating
    // { "foo" : { "bar" : value } }
    VectorString keys;

    std::string key = path->first();
    auto remaining = path->remainder();

    while (!key.empty()) {
        keys.push_back(key);

        if (!remaining) {
            break;
        } else {
            key = remaining->first();
            remaining = remaining->remainder();
        }
    }

    // the set_comments(VectorString()) is to ensure comments are only
    // on the exact leaf node they apply to.
    // a comment before "foo.bar" applies to the full setting
    // "foo.bar" not also to "foo"
    auto i = keys.rbegin();
    std::string deepest = *i++;
    auto o = SimpleConfigObject::make_instance(std::dynamic_pointer_cast<SimpleConfigOrigin>
    (value->origin())->set_comments({}), MapAbstractConfigValue({{deepest, value}}));

    for (; i != keys.rend(); ++i) {
        o = SimpleConfigObject::make_instance(std::dynamic_pointer_cast<SimpleConfigOrigin>
        (value->origin())->set_comments({}), MapAbstractConfigValue({{*i, o}}));
    }

    return o;
}

PathPtr ParseContext::parse_key(const TokenWithCommentsPtr& token) {
    if (flavor == ConfigSyntax::JSON) {
        if (Tokens::is_value_with_type(token->token, ConfigValueType::STRING)) {
            std::string key = Tokens::get_value(token->token)->unwrapped<std::string>();
            return Path::new_key(key);
        } else {
            throw parse_error(add_key_name("_expecting close brace } or a field name here, got " +
                                           token->to_string()));
        }
    } else {
        VectorToken expression;
        auto t = token;

        while (Tokens::is_value(t->token) || Tokens::is_unquoted_text(t->token)) {
            expression.push_back(t->token);
            t = next_token(); // note: don't cross a newline
        }

        if (expression.empty()) {
            throw parse_error(add_key_name("expecting a close brace or a field name here, got " +
                                           t->to_string()));
        }

        put_back(t); // put back the token we ended with
        return Parser::parse_path_expression(TokenIterator::make_instance(expression), line_origin());
    }
}

bool ParseContext::is_include_keyword(const TokenPtr& t) {
    return Tokens::is_unquoted_text(t) && Tokens::get_unquoted_text(t) == "include";
}

bool ParseContext::is_unquoted_whitespace(const TokenPtr& t) {
    if (!Tokens::is_unquoted_text(t)) {
        return false;
    }

    std::string s = Tokens::get_unquoted_text(t);

    for (auto& c : s) {
        if (!std::isspace(c)) {
            return false;
        }
    }

    return true;
}

void ParseContext::parse_include(MapAbstractConfigValue& values) {
    auto t = next_token_ignoring_newline();

    while (is_unquoted_whitespace(t->token)) {
        t = next_token_ignoring_newline();
    }

    AbstractConfigObjectPtr obj;

    // we either have a quoted string or the "file()" syntax
    if (Tokens::is_unquoted_text(t->token)) {
        // get foo(
        std::string kind = Tokens::get_unquoted_text(t->token);

        if (kind == "file(") {
        } else {
            throw parse_error("expecting include parameter to be quoted filename, "
                              "or file(). _no spaces are allowed before the open "
                              "paren. _not expecting: " + t->to_string());
        }

        // skip space inside parens
        t = next_token_ignoring_newline();

        while (is_unquoted_whitespace(t->token)) {
            t = next_token_ignoring_newline();
        }

        // quoted string
        std::string name;

        if (Tokens::is_value_with_type(t->token, ConfigValueType::STRING)) {
            name = Tokens::get_value(t->token)->unwrapped<std::string>();
        } else {
            throw parse_error("expecting a quoted string inside file(), rather than: " + t->to_string());
        }

        // skip space after string, inside parens
        t = next_token_ignoring_newline();

        while (is_unquoted_whitespace(t->token)) {
            t = next_token_ignoring_newline();
        }

        if (Tokens::is_unquoted_text(t->token) && Tokens::get_unquoted_text(t->token) == ")") {
            // _o_k, close paren
        } else {
            throw parse_error("expecting a close parentheses ')' here, not: " + t->to_string());
        }

        if (kind == "file(") {
            obj = std::dynamic_pointer_cast<AbstractConfigObject>(includer->include_file(include_context,
                    name));
        } else {
            throw ConfigExceptionBugOrBroken("should not be reached");
        }
    } else if (Tokens::is_value_with_type(t->token, ConfigValueType::STRING)) {
        std::string name = Tokens::get_value(t->token)->unwrapped<std::string>();
        obj = std::dynamic_pointer_cast<AbstractConfigObject>(includer->include(include_context, name));
    } else {
        throw parse_error("include keyword is not followed by a quoted string, but by: " + t->to_string());
    }

    if (!path_stack.empty()) {
        auto prefix = Path::make_instance(VectorPath(path_stack.begin(), path_stack.end()));
        obj = std::static_pointer_cast<AbstractConfigObject>(obj->relativized(prefix));
    }

    for (auto& pair : *obj) {
        auto v = std::dynamic_pointer_cast<AbstractConfigValue>(pair.second);
        auto existing = values.find(pair.first);

        if (existing != values.end()) {
            values[pair.first] = std::dynamic_pointer_cast<AbstractConfigValue>(v->with_fallback(
                                     existing->second));
        } else {
            values[pair.first] = v;
        }
    }
}

bool ParseContext::is_key_value_separator_token(const TokenPtr& t) {
    if (flavor == ConfigSyntax::JSON) {
        return t == Tokens::colon();
    } else {
        return t == Tokens::colon() || t == Tokens::equals() || t == Tokens::plus_equals();
    }
}

AbstractConfigObjectPtr ParseContext::parse_object(bool had_open_curly) {
    // invoked just after the open_curly (or start, if !had_open_curly)
    MapAbstractConfigValue values;
    auto object_origin = line_origin();
    bool after_comma = false;
    PathPtr last_path;
    bool last_inside_equals = false;

    while (true) {
        auto t = next_token_ignoring_newline();

        if (t->token == Tokens::close_curly()) {
            if (flavor == ConfigSyntax::JSON && after_comma) {
                throw parse_error(add_quote_suggestion(t->to_string(),
                                                       "expecting a field name after a comma, got a close brace } instead"));
            } else if (!had_open_curly) {
                throw parse_error(add_quote_suggestion(t->to_string(),
                                                       "unbalanced close brace '}' with no open brace"));
            }

            break;
        } else if (t->token == Tokens::end() && !had_open_curly) {
            put_back(t);
            break;
        } else if (flavor != ConfigSyntax::JSON && is_include_keyword(t->token)) {
            parse_include(values);
            after_comma = false;
        } else {
            auto key_token = t;
            auto path = parse_key(key_token);
            auto after_key = next_token_ignoring_newline();
            bool inside_equals = false;

            // path must be on-stack while we parse the value
            path_stack.push_front(path);

            TokenWithCommentsPtr value_token;
            AbstractConfigValuePtr new_value;

            if (flavor == ConfigSyntax::CONF && after_key->token == Tokens::open_curly()) {
                // can omit the ':' or '=' before an object value
                value_token = after_key;
            } else {
                if (!is_key_value_separator_token(after_key->token)) {
                    throw parse_error(add_quote_suggestion(after_key->to_string(),
                                                           "_key '" + path->render() +
                                                           "' may not be followed by token: " + after_key->to_string()));
                }

                if (after_key->token == Tokens::equals()) {
                    inside_equals = true;
                    equals_count++;
                }

                consolidate_value_tokens();
                value_token = next_token_ignoring_newline();
            }

            new_value = parse_value(value_token->prepend(key_token->comments));

            if (after_key->token == Tokens::plus_equals()) {
                VectorAbstractConfigValue concat;
                auto previous_ref = ConfigReference::make_instance(new_value->origin(),
                                    SubstitutionExpression::make_instance(full_current_path(), true));
                auto list = SimpleConfigList::make_instance(new_value->origin(), VectorAbstractConfigValue({new_value}));
                concat.push_back(previous_ref);
                concat.push_back(list);
                new_value = ConfigConcatenation::concatenate(concat);
            }

            last_path = path_stack.front();
            path_stack.pop_front();

            if (inside_equals) {
                equals_count--;
            }

            last_inside_equals = inside_equals;

            std::string key = path->first();
            auto remaining = path->remainder();

            if (!remaining) {
                auto existing = values.find(key);

                if (existing != values.end()) {
                    // _in strict JSON, dups should be an error; while in
                    // our custom config language, they should be merged
                    // if the value is an object (or substitution that
                    // could become an object).

                    if (flavor == ConfigSyntax::JSON) {
                        throw parse_error("JSON does not allow duplicate fields: '" +
                                          key + "' was already seen at " +
                                          existing->second->origin()->description());
                    } else {
                        new_value = std::dynamic_pointer_cast<AbstractConfigValue>(new_value->with_fallback(
                                        existing->second));
                    }
                }

                values[key] = new_value;
            } else {
                if (flavor == ConfigSyntax::JSON) {
                    throw ConfigExceptionBugOrBroken("somehow got multi-element path in JSON mode");
                }

                auto obj = create_value_under_path(remaining, new_value);
                auto existing = values.find(key);

                if (existing != values.end()) {
                    obj = std::dynamic_pointer_cast<AbstractConfigObject>(obj->with_fallback(existing->second));
                }

                values[key] = obj;
            }

            after_comma = false;
        }

        if (check_element_separator()) {
            // continue looping
            after_comma = true;
        } else {
            t = next_token_ignoring_newline();

            if (t->token == Tokens::close_curly()) {
                if (!had_open_curly) {
                    throw parse_error(add_quote_suggestion(last_path, last_inside_equals,
                                                           t->to_string(), "unbalanced close brace '}' with no open brace"));
                }

                break;
            } else if (had_open_curly) {
                throw parse_error(add_quote_suggestion(last_path, last_inside_equals,
                                                       t->to_string(), "_expecting close brace } or a comma, got " + t->to_string()));
            } else {
                if (t->token == Tokens::end()) {
                    put_back(t);
                    break;
                } else {
                    throw parse_error(add_quote_suggestion(last_path, last_inside_equals,
                                                           t->to_string(), "_expecting end of input or a comma, got " + t->to_string()));
                }
            }
        }
    }

    return SimpleConfigObject::make_instance(object_origin, values);
}

SimpleConfigList ParseContext::parse_array() {
    // invoked just after the open_square
    auto array_origin = line_origin();
    VectorAbstractConfigValue values;

    consolidate_value_tokens();

    auto t = next_token_ignoring_newline();

    // special-case the first element
    if (t->token == Tokens::close_square()) {
        return SimpleConfigList::make_instance(array_origin, VectorAbstractConfigValue());
    } else if (Tokens::is_value(t->token) || t->token == Tokens::open_curly()
               || t->token == Tokens::open_square()) {
        values.push_back(parse_value(t));
    } else {
        throw parse_error(
            add_key_name("_list should have ] or a first element after the open [, instead had token: " +
                         t->to_string() + " (if you want " + t->to_string() +
                         " to be part of a string value, then double-quote it)"));
    }

    // now remaining elements
    while (true) {
        // just after a value
        if (check_element_separator()) {
            // comma (or newline equivalent) consumed
        } else {
            t = next_token_ignoring_newline();

            if (t->token == Tokens::close_square()) {
                return SimpleConfigList::make_instance(array_origin, values);
            } else {
                throw parse_error(add_key_name("_list should have ended with ] or had a comma, instead had token: "
                                               +
                                               t->to_string() + " (if you want " + t->to_string() +
                                               " to be part of a string value, then double-quote it)"));
            }
        }

        // now just after a comma
        consolidate_value_tokens();

        t = next_token_ignoring_newline();

        if (Tokens::is_value(t->token) || t->token == Tokens::open_curly()
                || t->token == Tokens::open_square()) {
            values.push_back(parse_value(t));
        } else if (flavor != ConfigSyntax::JSON && t->token == Tokens::close_square()) {
            // we allow one trailing comma
            put_back(t);
        } else {
            throw parse_error(
                add_key_name("_list should have had new element after a comma, instead had token: " +
                             t->to_string() + " (if you want the comma or " + t->to_string() +
                             " to be part of a string value, then double-quote it)"));
        }
    }
}

AbstractConfigValuePtr ParseContext::parse() {
    auto t = next_token_ignoring_newline();

    if (t->token == Tokens::start()) {
        // _o_k
    } else {
        throw ConfigExceptionBugOrBroken("token stream did not begin with start, had " +
                                              t->to_string());
    }

    t = next_token_ignoring_newline();
    AbstractConfigValuePtr result;

    if (t->token == Tokens::open_curly() || t->token == Tokens::open_square()) {
        result = parse_value(t);
    } else {
        if (flavor == ConfigSyntax::JSON) {
            if (t->token == Tokens::end()) {
                throw parse_error("_empty document");
            } else {
                throw parse_error("_document must have an object or array at root, unexpected token: " +
                                  t->to_string());
            }
        } else {
            // the root object can omit the surrounding braces.
            // this token should be the first field's key, or part
            // of it, so put it back.
            put_back(t);
            result = parse_object(false);
            // in this case we don't try to use comments_stack comments
            // since they would all presumably apply to fields not the
            // root object
        }
    }

    t = next_token_ignoring_newline();

    if (t->token == Tokens::end()) {
        return result;
    } else {
        throw parse_error("_document has trailing tokens after first object or array: " + t->to_string());
    }
}

Element::Element(const std::string& initial, bool can_be_empty) :
    element(initial),
    can_be_empty(can_be_empty) {
}

std::string Element::to_string() {
    return "Element(" + element + "," + (can_be_empty ? "true" : "false") + ")";
}

void Parser::add_path_text(VectorElement& buf, bool was_quoted, const std::string& new_text) {
    std::string::size_type i = was_quoted ? std::string::npos : new_text.find('.');
    auto current = buf.back();

    if (i == std::string::npos) {
        // add to current path element
        current->element += new_text;

        // any empty quoted string means this element can now be empty.
        if (was_quoted && current->element.empty()) {
            current->can_be_empty = true;
        }
    } else {
        // "buf" plus up to the period is an element
        current->element += new_text.substr(0, i);
        // then start a new element
        buf.push_back(Element::make_instance("", false));
        // recurse to consume remainder of new_text
        add_path_text(buf, false, new_text.substr(i + 1));
    }
}

PathPtr Parser::parse_path_expression(const TokenIteratorPtr& expression,
        const ConfigOriginPtr& origin, const std::string& original_text) {
    // each builder in "buf" is an element in the path.
    VectorElement buf;
    buf.push_back(Element::make_instance("", false));

    if (!expression->has_next()) {
        throw ConfigExceptionBadPath(origin, original_text,
                                         "_expecting a field name or path here, but got nothing");
    }

    while (expression->has_next()) {
        auto t = expression->next();

        if (Tokens::is_value_with_type(t, ConfigValueType::STRING)) {
            auto v = Tokens::get_value(t);
            // this is a quoted string; so any periods
            // in here don't count as path separators
            std::string s = v->transform_to_string();

            add_path_text(buf, true, s);
        } else if (t == Tokens::end()) {
            // ignore this; when parsing a file, it should not happen
            // since we're parsing a token list rather than the main
            // token iterator, and when parsing a path expression from the
            // API, it's expected to have an end.
        } else {
            // any periods outside of a quoted string count as
            // separators
            std::string text;

            if (Tokens::is_value(t)) {
                // appending a number here may add
                // a period, but we _do_ count those as path
                // separators, because we basically want
                // "foo 3.0bar" to parse as a string even
                // though there's a number in it. _the fact that
                // we tokenize non-string values is largely an
                // implementation detail.
                auto v = Tokens::get_value(t);
                text = v->transform_to_string();
            } else if (Tokens::is_unquoted_text(t)) {
                text = Tokens::get_unquoted_text(t);
            } else {
                throw ConfigExceptionBadPath(origin, original_text,
                                                 "Token not allowed in path expression: " +
                                                 t->to_string() +
                                                 " (you can double-quote this token if you really want it here)");
            }

            add_path_text(buf, false, text);
        }
    }

    auto pb = PathBuilder::make_instance();

    for (auto& e : buf) {
        if (e->element.empty() && !e->can_be_empty) {
            throw ConfigExceptionBadPath(origin, original_text,
                                             "path has a leading, trailing, or two "
                                             "adjacent period '.' (use quoted \"\" "
                                             "empty string if you want an empty element)");
        } else {
            pb->append_key(e->element);
        }
    }

    return pb->result();
}

PathPtr Parser::parse_path(const std::string& path) {
    static auto api_origin = SimpleConfigOrigin::new_simple("path parameter");

    auto speculated = speculative_fast_parse_path(path);

    if (speculated) {
        return speculated;
    }

    auto reader = StringReader::make_instance(path);

    ConfigExceptionPtr finally;
    PathPtr path_expression;

    try {
        auto tokens = Tokenizer::tokenize(api_origin, reader, ConfigSyntax::CONF);
        tokens->next(); // drop start
        path_expression = parse_path_expression(tokens, api_origin, path);
    } catch (ConfigException& e) {
        finally = e.clone();
    }

    reader->close();

    if (finally) {
        finally->raise();
    }

    return path_expression;
}

bool Parser::has_unsafe_chars(const std::string& s) {
    for (auto& c : s) {
        if (std::isalpha(c) || c == '.') {
            continue;
        } else {
            return true;
        }
    }

    return false;
}

void Parser::append_path_string(const PathBuilderPtr& pb, const std::string& s) {
    std::string::size_type split_at = s.find('.');

    if (split_at == std::string::npos) {
        pb->append_key(s);
    } else {
        pb->append_key(s.substr(0, split_at));
        append_path_string(pb, s.substr(split_at + 1));
    }
}

PathPtr Parser::speculative_fast_parse_path(const std::string& path) {
    std::string s = boost::trim_copy(path);

    if (s.empty()) {
        return nullptr;
    }

    if (has_unsafe_chars(s)) {
        return nullptr;
    }

    if (boost::starts_with(s, ".") || boost::ends_with(s, ".") || boost::contains(s, "..")) {
        return nullptr; // let the full parser throw the error
    }

    auto pb = PathBuilder::make_instance();
    append_path_string(pb, s);
    return pb->result();
}

}
