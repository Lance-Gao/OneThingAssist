
#include "detail/parseable.hpp"
#include "detail/config_impl.hpp"
#include "detail/simple_includer.hpp"
#include "detail/simple_include_context.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/simple_config_object.hpp"
#include "detail/abstract_config_object.hpp"
#include "detail/tokenizer.hpp"
#include "detail/parser.hpp"
#include "detail/string_reader.hpp"
#include "detail/file_reader.hpp"
#include "config_syntax.hpp"
#include "config_value.hpp"
#include "config_value_type.hpp"
#include "config_parse_options.hpp"

namespace config {

StackParseable Parseable::parse_stack;

Parseable::Parseable(const ConfigParseOptionsPtr& base_options) :
    base_options(base_options) {
}

void Parseable::initialize() {
    post_construct(base_options);
}

ConfigParseOptionsPtr Parseable::fixup_options(const ConfigParseOptionsPtr& base_options) {
    ConfigSyntax syntax = base_options->get_syntax();

    if (syntax == ConfigSyntax::NONE) {
        syntax = guess_syntax();
    }

    if (syntax == ConfigSyntax::NONE) {
        syntax = ConfigSyntax::CONF;
    }

    auto modified = base_options->set_syntax(syntax);

    // make sure the app-provided includer falls back to default
    modified = modified->append_includer(ConfigImpl::default_includer());
    // make sure the app-provided includer is complete
    modified = modified->set_includer(SimpleIncluder::make_full(modified->get_includer()));

    return modified;
}

void Parseable::post_construct(const ConfigParseOptionsPtr& base_options) {
    this->initial_options = fixup_options(base_options);

    this->include_context_ = SimpleIncludeContext::make_instance(shared_from_this());

    std::string origin_description = initial_options->get_origin_description();

    if (!origin_description.empty()) {
        initial_origin = SimpleConfigOrigin::new_simple(origin_description);
    } else {
        initial_origin = create_origin();
    }
}

void Parseable::trace(const std::string& message) {
    if (ConfigImpl::trace_loads_enabled()) {
        ConfigImpl::trace(message);
    }
}

ConfigSyntax Parseable::guess_syntax() {
    return ConfigSyntax::NONE;
}

ConfigParseablePtr Parseable::relative_to(const std::string& filename) {
    return new_not_found(filename, "filename was not found: '" + filename + "'", options());
}

ConfigIncludeContextPtr Parseable::include_context() {
    return include_context_;
}

AbstractConfigObjectPtr Parseable::force_parsed_to_object(const ConfigValuePtr& value) {
    if (instanceof<AbstractConfigObject>(value)) {
        return std::dynamic_pointer_cast<AbstractConfigObject>(value);
    } else {
        throw ConfigExceptionWrongType(value->origin(), "", "object at file root",
                                           ConfigValueTypeEnum::name(value->value_type()));
    }
}

ConfigObjectPtr Parseable::parse(const ConfigParseOptionsPtr& base_options) {
    if (parse_stack.size() >= MAX_INCLUDE_DEPTH) {
        std::ostringstream stream;
        stream << "include statements nested more than " << MAX_INCLUDE_DEPTH;
        stream << " times, you probably have a cycle in your includes. _trace: ";

        for (auto& p : parse_stack) {
            stream << p->to_string() << ",\n";
        }

        std::string message = stream.str();
        throw ConfigExceptionParse(initial_origin, message.substr(0, message.length() - 2));
    }

    parse_stack.push_front(shared_from_this());
    ConfigExceptionPtr finally;
    ConfigObjectPtr parse_object;

    try {
        parse_object = force_parsed_to_object(parse_value(base_options));
    } catch (ConfigException& e) {
        finally = e.clone();
    }

    parse_stack.pop_front();

    if (parse_stack.empty()) {
        parse_stack.clear();
    }

    if (finally) {
        finally->raise();
    }

    return parse_object;
}

AbstractConfigValuePtr Parseable::parse_value(const ConfigParseOptionsPtr& base_options) {
    // note that we are NOT using our "initial_options",
    // but using the ones from the passed-in options. _the idea is that
    // callers can get our original options and then parse with different
    // ones if they want.
    auto options = fixup_options(base_options);

    // passed-in options can override origin
    ConfigOriginPtr origin;

    if (!options->get_origin_description().empty()) {
        origin = SimpleConfigOrigin::new_simple(options->get_origin_description());
    } else {
        origin = initial_origin;
    }

    return parse_value(origin, options);
}

AbstractConfigValuePtr Parseable::parse_value(const ConfigOriginPtr& origin,
        const ConfigParseOptionsPtr& final_options) {
    try {
        return raw_parse_value(origin, final_options);
    } catch (ConfigExceptionIO& e) {
        if (final_options->get_allow_missing()) {
            return SimpleConfigObject::make_empty_missing(origin);
        } else {
            throw ConfigExceptionIO(origin, e.what());
        }
    }
}

AbstractConfigValuePtr Parseable::raw_parse_value(const ConfigOriginPtr& origin,
        const ConfigParseOptionsPtr& final_options) {
    auto reader_ = reader();
    ConfigExceptionPtr finally;
    AbstractConfigValuePtr parse_value;

    try {
        parse_value = raw_parse_value(reader_, origin, final_options);
    } catch (ConfigException& e) {
        finally = e.clone();
    }

    reader_->close();

    if (finally) {
        finally->raise();
    }

    return parse_value;
}

AbstractConfigValuePtr Parseable::raw_parse_value(const ReaderPtr& reader,
        const ConfigOriginPtr& origin, const ConfigParseOptionsPtr& final_options) {
    auto tokens = Tokenizer::tokenize(origin, reader, final_options->get_syntax());
    return Parser::parse(tokens, origin, final_options, include_context());
}

ConfigObjectPtr Parseable::parse() {
    return force_parsed_to_object(parse_value(options()));
}

AbstractConfigValuePtr Parseable::parse_value() {
    return parse_value(options());
}

ConfigOriginPtr Parseable::origin() {
    return initial_origin;
}

ConfigParseOptionsPtr Parseable::options() {
    return initial_options;
}

std::string Parseable::to_string() {
    return get_class_name();
}

ConfigSyntax Parseable::syntax_from_extension(const std::string& name) {
    if (boost::ends_with(name, ".json")) {
        return ConfigSyntax::JSON;
    } else if (boost::ends_with(name, ".conf")) {
        return ConfigSyntax::CONF;
    } else {
        return ConfigSyntax::NONE;
    }
}

std::string Parseable::relative_to(const std::string& file, const std::string& filename) {
    boost::filesystem::path child(filename);

    if (child.has_root_directory()) {
        return "";
    }

    boost::filesystem::path parent = boost::filesystem::path(file).parent_path();

    if (parent.empty()) {
        return "";
    } else {
        parent /= filename;
        return parent.directory_string();
    }
}

ParseableNotFound::ParseableNotFound(const std::string& what, const std::string& message,
        const ConfigParseOptionsPtr& options) :
    Parseable(options),
    what(what),
    message(message) {
}

ReaderPtr ParseableNotFound::reader() {
    throw ConfigExceptionFileNotFound(message);
}

ConfigOriginPtr ParseableNotFound::create_origin() {
    return SimpleConfigOrigin::new_simple(what);
}

ParseablePtr Parseable::new_not_found(const std::string& what_not_found,
        const std::string& message,
        const ConfigParseOptionsPtr& options) {
    return ParseableNotFound::make_instance(what_not_found, message, options);
}

ParseableReader::ParseableReader(const ReaderPtr& reader,
                                     const ConfigParseOptionsPtr& options) :
    Parseable(options),
    reader_(reader) {
}

ReaderPtr ParseableReader::reader() {
    if (ConfigImpl::trace_loads_enabled()) {
        trace("_loading config from reader: " + boost::lexical_cast<std::string>(reader_.get()));
    }

    return reader_;
}

ConfigOriginPtr ParseableReader::create_origin() {
    return SimpleConfigOrigin::new_simple("reader");
}

ParseablePtr Parseable::new_reader(const ReaderPtr& reader,
                                      const ConfigParseOptionsPtr& options) {
    return ParseableReader::make_instance(reader, options);
}

ParseableString::ParseableString(const std::string& input,
                                     const ConfigParseOptionsPtr& options) :
    Parseable(options),
    input(input) {
}

ReaderPtr ParseableString::reader() {
    if (ConfigImpl::trace_loads_enabled()) {
        trace("_loading config from a string: " + input);
    }

    return StringReader::make_instance(input);
}

ConfigOriginPtr ParseableString::create_origin() {
    return SimpleConfigOrigin::new_simple("string");
}

std::string ParseableString::to_string() {
    return get_class_name() + "(" + input + ")";
}

ParseablePtr Parseable::new_string(const std::string& input,
                                      const ConfigParseOptionsPtr& options) {
    return ParseableString::make_instance(input, options);
}

ParseableFile::ParseableFile(const std::string& input,
                                 const ConfigParseOptionsPtr& options) :
    Parseable(options),
    input(input) {
}

ReaderPtr ParseableFile::reader() {
    if (ConfigImpl::trace_loads_enabled()) {
        trace("_loading config from a file: " + input);
    }

    return FileReader::make_instance(input);
}

ConfigSyntax ParseableFile::guess_syntax() {
    return syntax_from_extension(input);
}

ConfigParseablePtr ParseableFile::relative_to(const std::string& filename) {
    boost::filesystem::path sibling;

    if (boost::filesystem::path(filename).has_root_directory()) {
        sibling = boost::filesystem::path(filename);
    } else {
        // this may return empty
        sibling = boost::filesystem::path(relative_to(input, filename));
    }

    if (sibling.empty()) {
        return nullptr;
    }

    if (MiscUtils::file_exists(sibling.directory_string())) {
        return new_file(sibling.directory_string(), options()->set_origin_description(""));
    } else {
        return Parseable::relative_to(filename);
    }
}

ConfigOriginPtr ParseableFile::create_origin() {
    return SimpleConfigOrigin::new_file(input);
}

std::string ParseableFile::to_string() {
    return get_class_name() + "(" + input + ")";
}

ParseablePtr Parseable::new_file(const std::string& input,
                                    const ConfigParseOptionsPtr& options) {
    return ParseableFile::make_instance(input, options);
}

}
