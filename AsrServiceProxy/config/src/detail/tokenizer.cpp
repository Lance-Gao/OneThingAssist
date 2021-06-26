
#include "detail/tokenizer.hpp"
#include "detail/token.hpp"
#include "detail/tokens.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/reader.hpp"
#include "config_syntax.hpp"

namespace config {

std::string Tokenizer::as_string(int32_t codepoint) {
    if (static_cast<char>(codepoint) == '\n') {
        return "newline";
    } else if (static_cast<char>(codepoint) == '\t') {
        return "tab";
    } else if (codepoint == -1) {
        return "end of file";
    } else if (std::iscntrl(static_cast<char>(codepoint))) {
        std::ostringstream stream;
        stream << "control character 0x" << std::hex << std::nouppercase;
        stream << std::setfill('0') << std::setw(2) << codepoint;
        return stream.str();
    } else {
        return std::string(1, static_cast<char>(codepoint));
    }
}

TokenIteratorPtr Tokenizer::tokenize(const ConfigOriginPtr& origin, const ReaderPtr& input,
        ConfigSyntax flavor) {
    return TokenStream::make_instance(origin, input, flavor != ConfigSyntax::JSON);
}

WhitespaceSaver::WhitespaceSaver() :
    last_token_was_simple_value(false) {
}

void WhitespaceSaver::add(int32_t c) {
    if (last_token_was_simple_value) {
        whitespace << static_cast<char>(c);
    }
}

TokenPtr WhitespaceSaver::check(const TokenPtr& t, const ConfigOriginPtr& base_origin,
                                    int32_t line_number) {
    if (TokenStream::is_simple_value(t)) {
        return next_is_a_simple_value(base_origin, line_number);
    } else {
        next_is_not_a_simple_value();
        return nullptr;
    }
}

void WhitespaceSaver::next_is_not_a_simple_value() {
    last_token_was_simple_value = false;
    whitespace.str("");
}

TokenPtr WhitespaceSaver::next_is_a_simple_value(const ConfigOriginPtr& base_origin,
        int32_t line_number) {
    if (last_token_was_simple_value) {
        // need to save whitespace between the two so
        // the parser has the option to concatenate it.
        if (!whitespace.str().empty()) {
            auto t = Tokens::new_unquoted_text(TokenStream::line_origin(base_origin, line_number),
                                                whitespace.str());
            whitespace.str(""); // reset
            return t;
        } else {
            // last_token_was_simple_value = true still
            return nullptr;
        }
    } else {
        last_token_was_simple_value = true;
        whitespace.str("");
        return nullptr;
    }
}

TokenIterator::TokenIterator(const VectorToken& tokens) :
    begin(tokens.begin()),
    end(tokens.end()) {
}

bool TokenIterator::has_next() {
    return begin != end;
}

TokenPtr TokenIterator::next() {
    return *begin++;
}

const std::string TokenStream::first_number_chars = "0123456789-";
const std::string TokenStream::number_chars = "0123456789e_e+-.";
const std::string TokenStream::not_in_unquoted_text = "$\"{}[]:=,+#`^?!@*&\\";

TokenStream::TokenStream(const ConfigOriginPtr& origin, const ReaderPtr& input,
                             bool allow_comments) :
    origin_(std::dynamic_pointer_cast<SimpleConfigOrigin>(origin)),
    input(input),
    allow_comments(allow_comments),
    line_number(1),
    line_origin_(origin_->set_line_number(line_number)),
    tokens(1, Tokens::start()),
    whitespace_saver(WhitespaceSaver::make_instance()) {
}

int32_t TokenStream::next_char_raw() {
    if (buffer.empty()) {
        try {
            return input->read();
        } catch (ConfigExceptionIO& e) {
            throw ConfigExceptionIO(origin_, std::string("read error: ") + e.what());
        }
    } else {
        int32_t c = buffer.front();
        buffer.pop_front();
        return c;
    }
}

void TokenStream::put_back(int32_t c) {
    if (buffer.size() > 2) {
        throw ConfigExceptionBugOrBroken("bug: put_back() three times, undesirable look-ahead");
    }

    buffer.push_front(c);
}

bool TokenStream::is_whitespace(int32_t c) {
    return std::isspace(c);
}

bool TokenStream::is_whitespace_not_newline(int32_t c) {
    return c != '\n' && std::isspace(c);
}

bool TokenStream::start_of_comment(int32_t c) {
    if (c == -1) {
        return false;
    } else {
        if (allow_comments) {
            if (c == '#') {
                return true;
            } else if (c == '/') {
                int32_t maybe_second_slash = next_char_raw();
                // we want to predictably NOT consume any chars
                put_back(maybe_second_slash);

                if (maybe_second_slash == '/') {
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}

int32_t TokenStream::next_char_after_whitespace(const WhitespaceSaverPtr& saver) {
    while (true) {
        int32_t c = next_char_raw();

        if (c == -1) {
            return -1;
        } else {
            if (is_whitespace_not_newline(c)) {
                saver->add(c);
                continue;
            } else {
                return c;
            }
        }
    }
}

ConfigExceptionTokenizerProblem TokenStream::problem(const std::string& message) {
    return problem(line_origin_, "", message, false);
}

ConfigExceptionTokenizerProblem TokenStream::problem(const std::string& what,
        const std::string& message, bool suggest_quotes) {
    return problem(line_origin_, what, message, suggest_quotes);
}

ConfigExceptionTokenizerProblem TokenStream::problem(const ConfigOriginPtr& origin,
        const std::string& what, const std::string& message, bool suggest_quotes) {
    return ConfigExceptionTokenizerProblem(Tokens::new_problem(origin, what, message,
            suggest_quotes));
}

ConfigExceptionTokenizerProblem TokenStream::problem(const ConfigOriginPtr& origin,
        const std::string& message) {
    return problem(origin, "", message, false);
}

ConfigOriginPtr TokenStream::line_origin(const ConfigOriginPtr& base_origin,
        int32_t line_number) {
    return std::dynamic_pointer_cast<SimpleConfigOrigin>(base_origin)->set_line_number(line_number);
}

TokenPtr TokenStream::pull_comment(int32_t first_char) {
    if (first_char == '/') {
        int32_t discard = next_char_raw();

        if (discard != '/') {
            throw ConfigExceptionBugOrBroken("called pull_comment but // not seen");
        }
    }

    std::string s;

    while (true) {
        int32_t c = next_char_raw();

        if (c == -1 || c == '\n') {
            put_back(c);
            return Tokens::new_comment(line_origin_, s);
        } else {
            s += static_cast<char>(c);
        }
    }
}

TokenPtr TokenStream::pull_unquoted_text() {
    auto origin = line_origin_;
    std::string sb;
    int32_t c = next_char_raw();

    while (true) {
        if (c == -1) {
            break;
        } else if (not_in_unquoted_text.find(c) != std::string::npos) {
            break;
        } else if (is_whitespace(c)) {
            break;
        } else if (start_of_comment(c)) {
            break;
        } else {
            sb += static_cast<char>(c);
        }

        // we parse true/false/null tokens as such no matter
        // what is after them, as long as they are at the
        // start of the unquoted token.
        if (sb.length() == 4) {
            if (sb == "true") {
                return Tokens::new_boolean(origin, true);
            } else if (sb == "null") {
                return Tokens::new_null(origin);
            }
        } else if (sb.length() == 5) {
            if (sb == "false") {
                return Tokens::new_boolean(origin, false);
            }
        }

        c = next_char_raw();
    }

    // put back the char that ended the unquoted text
    put_back(c);

    return Tokens::new_unquoted_text(origin, sb);
}

TokenPtr TokenStream::pull_number(int32_t first_char) {
    std::string s;
    s += static_cast<char>(first_char);
    bool contained_decimal_or_e = false;
    int32_t c = next_char_raw();

    while (c != -1 && number_chars.find(c) != std::string::npos) {
        if (c == '.' || c == 'e' || c == '_e') {
            contained_decimal_or_e = true;
        }

        s += static_cast<char>(c);
        c = next_char_raw();
    }

    // the last character we looked at wasn't part of the number, put it back
    put_back(c);

    try {
        if (contained_decimal_or_e) {
            // force floating point representation
            return Tokens::new_double(line_origin_, boost::lexical_cast<double>(s), s);
        } else {
            // this should throw if the integer is too large for int64_t
            return Tokens::new_int64(line_origin_, boost::lexical_cast<int64_t>(s), s);
        }
    } catch (boost::bad_lexical_cast&) {
        throw problem(s, "_invalid number: '" + s + "'", true);
    }
}

void TokenStream::pull_escape_sequence(std::string& s) {
    int32_t escaped = next_char_raw();

    if (escaped == -1) {
        throw problem("_end of input but backslash in string had nothing after it");
    }

    switch (escaped) {
    case '"':
        s += '"';
        break;

    case '\\':
        s += '\\';
        break;

    case '/':
        s += '/';
        break;

    case 'b':
        s += '\b';
        break;

    case 'f':
        s += '\f';
        break;

    case 'n':
        s += '\n';
        break;

    case 'r':
        s += '\r';
        break;

    case 't':
        s += '\t';
        break;

    case 'u': {
        // kind of absurdly slow, but screw it for now
        std::string digits;

        for (int32_t i = 0; i < 4; ++i) {
            int32_t c = next_char_raw();

            if (c == -1) {
                throw problem("_end of input but expecting 4 hex digits for \\u_x_x_x_x escape");
            }

            digits.push_back(static_cast<char>(c));
        }

        s += strtol(digits.c_str(), 0, 16);
    }
    break;

    default:
        throw problem(Tokenizer::as_string(escaped),
                      "backslash followed by '" + Tokenizer::as_string(escaped) +
                      "', this is not a valid escape sequence (quoted"
                      " strings use JSON escaping, so use"
                      " double-backslash \\\\ for literal backslash)");
    }
}

void TokenStream::append_triple_quoted_string(std::string& s) {
    // we are after the opening triple quote and need to consume the
    // close triple
    uint32_t consecutive_quotes = 0;

    while (true) {
        int32_t c = next_char_raw();

        if (c == '\"') {
            consecutive_quotes += 1;
        } else if (consecutive_quotes >= 3) {
            // the last three quotes end the string and the others are kept.
            s.resize(s.length() - 3);
            put_back(c);
            break;
        } else {
            consecutive_quotes = 0;

            if (c == -1) {
                throw problem("_end of input but triple-quoted string was still open");
            }
        }

        s += static_cast<char>(c);
    }
}

TokenPtr TokenStream::pull_quoted_string() {
    // the open quote has already been consumed
    std::string s;
    int32_t c = static_cast<int32_t>('\0'); // value doesn't get used

    do {
        c = next_char_raw();

        if (c == -1) {
            throw problem("_end of input but string quote was still open");
        }

        if (c == '\\') {
            pull_escape_sequence(s);
        } else if (c == '"') {
            // end the loop, done!
        } else if (std::iscntrl(static_cast<char>(c))) {
            throw problem(Tokenizer::as_string(c), "JSON does not allow unescaped " +
                          Tokenizer::as_string(c) + " in quoted strings, use a backslash escape");
        } else {
            s += static_cast<char>(c);
        }
    } while (c != '"');

    // maybe switch to triple-quoted string, sort of hacky...
    if (s.empty()) {
        int32_t third = next_char_raw();

        if (third == '\"') {
            append_triple_quoted_string(s);
        } else {
            put_back(third);
        }
    }

    return Tokens::new_string(line_origin_, s);
}

TokenPtr TokenStream::pull_plus_equals() {
    // the initial '+' has already been consumed
    int32_t c = next_char_raw();

    if (c != '=') {
        throw problem(Tokenizer::as_string(c), "'+' not followed by =, '" +
                      Tokenizer::as_string(c) + "' not allowed after '+'", true);
    }

    return Tokens::plus_equals();
}

TokenPtr TokenStream::pull_substitution() {
    // the initial '$' has already been consumed
    auto origin = line_origin_;
    int32_t c = next_char_raw();

    if (c != '{') {
        throw problem(Tokenizer::as_string(c), "'$' not followed by {, '" +
                      Tokenizer::as_string(c) + "' not allowed after '$'", true);
    }

    bool optional = false;
    c = next_char_raw();

    if (c == '?') {
        optional = true;
    } else {
        put_back(c);
    }

    auto saver = WhitespaceSaver::make_instance();
    VectorToken expression;

    TokenPtr t;

    do {
        t = pull_next_token(saver);

        // note that we avoid validating the allowed tokens inside
        // the substitution here; we even allow nested substitutions
        // in the tokenizer. _the parser sorts it out.
        if (t == Tokens::close_curly()) {
            // end the loop, done!
            break;
        } else if (t == Tokens::end()) {
            throw problem(origin, "_substitution ${ was not closed with a }");
        } else {
            auto whitespace = saver->check(t, origin, line_number);

            if (whitespace) {
                expression.push_back(whitespace);
            }

            expression.push_back(t);
        }
    } while (true);

    return Tokens::new_substitution(origin, optional, expression);
}

TokenPtr TokenStream::pull_next_token(const WhitespaceSaverPtr& saver) {
    int32_t c = next_char_after_whitespace(saver);

    if (c == -1) {
        return Tokens::end();
    } else if (c == '\n') {
        // newline tokens have the just-ended line number
        auto line = Tokens::new_line(line_origin_);
        line_number += 1;
        line_origin_ = origin_->set_line_number(line_number);
        return line;
    } else {
        TokenPtr t;

        if (start_of_comment(c)) {
            t = pull_comment(c);
        } else {
            switch (c) {
            case '"':
                t = pull_quoted_string();
                break;

            case '$':
                t = pull_substitution();
                break;

            case ':':
                t = Tokens::colon();
                break;

            case ',':
                t = Tokens::comma();
                break;

            case '=':
                t = Tokens::equals();
                break;

            case '{':
                t = Tokens::open_curly();
                break;

            case '}':
                t = Tokens::close_curly();
                break;

            case '[':
                t = Tokens::open_square();
                break;

            case ']':
                t = Tokens::close_square();
                break;

            case '+':
                t = pull_plus_equals();
                break;

            default:
                t = nullptr;
                break;
            }

            if (!t) {
                if (first_number_chars.find(c) != std::string::npos) {
                    t = pull_number(c);
                } else if (not_in_unquoted_text.find(c) != std::string::npos) {
                    throw problem(Tokenizer::as_string(c), "_reserved character '" +
                                  Tokenizer::as_string(c) +
                                  "' is not allowed outside quotes", true);
                } else {
                    put_back(c);
                    t = pull_unquoted_text();
                }
            }
        }

        if (!t) {
            throw ConfigExceptionBugOrBroken("bug: failed to generate next token");
        }

        return t;
    }
}

bool TokenStream::is_simple_value(const TokenPtr& t) {
    return Tokens::is_substitution(t) || Tokens::is_unquoted_text(t) || Tokens::is_value(t);
}

void TokenStream::queue_next_token() {
    auto t = pull_next_token(whitespace_saver);
    auto whitespace = whitespace_saver->check(t, origin_, line_number);

    if (whitespace) {
        tokens.push_back(whitespace);
    }

    tokens.push_back(t);
}

bool TokenStream::has_next() {
    return !tokens.empty();
}

TokenPtr TokenStream::next() {
    auto t = tokens.front();
    tokens.pop_front();

    if (tokens.empty() && t != Tokens::end()) {
        try {
            queue_next_token();
        } catch (ConfigExceptionTokenizerProblem& e) {
            tokens.push_back(e.problem());
        }

        if (tokens.empty()) {
            throw ConfigExceptionBugOrBroken("bug: tokens queue should not be empty here");
        }
    }

    return t;
}

}
