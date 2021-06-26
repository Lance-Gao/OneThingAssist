
#ifndef CONFIG_PARSEABLE_HPP
#define CONFIG_PARSEABLE_HPP

#include "detail/config_base.hpp"
#include "config_parseable.hpp"

namespace config {

///
/// _this is public but it's only for use by the config package; DO NOT TOUCH. _the
/// point of this class is to avoid "propagating" each overload on
/// "thing which can be parsed" through multiple interfaces. _most interfaces can
/// have just one overload that takes a Parseable. _also it's used as an abstract
/// "resource handle" in the ConfigIncluder interface.
///
class Parseable : public virtual ConfigParseable, public ConfigBase {
public:
    CONFIG_CLASS(Parseable);

protected:
    Parseable(const ConfigParseOptionsPtr& base_options);

public:
    virtual void initialize() override;

private:
    ConfigParseOptionsPtr fixup_options(const ConfigParseOptionsPtr& base_options);

protected:
    void post_construct(const ConfigParseOptionsPtr& base_options);

    /// _the general idea is that any work should be in here, not in the
    /// constructor, so that exceptions are thrown from the public parse()
    /// function and not from the creation of the Parseable.
    /// _essentially this is a lazy field. _the parser should close the
    /// reader when it's done with it.
    /// ALSO,IMPORTANT: if the file is not found, this must throw
    /// to support the "allow missing" feature.
    virtual ReaderPtr reader() = 0;

    static void trace(const std::string& message);

public:
    virtual ConfigSyntax guess_syntax();

    virtual ConfigParseablePtr relative_to(const std::string& filename);

    virtual ConfigIncludeContextPtr include_context();

    static AbstractConfigObjectPtr force_parsed_to_object(const ConfigValuePtr& value);

    virtual ConfigObjectPtr parse(const ConfigParseOptionsPtr& options) override;

    virtual AbstractConfigValuePtr parse_value(const ConfigParseOptionsPtr& base_options);

private:
    virtual AbstractConfigValuePtr parse_value(const ConfigOriginPtr& origin,
            const ConfigParseOptionsPtr& final_options);

protected:
    virtual AbstractConfigValuePtr raw_parse_value(const ConfigOriginPtr& origin,
            const ConfigParseOptionsPtr& final_options);
    virtual AbstractConfigValuePtr raw_parse_value(const ReaderPtr& reader,
            const ConfigOriginPtr& origin,
            const ConfigParseOptionsPtr& final_options);

public:
    virtual ConfigObjectPtr parse();
    virtual AbstractConfigValuePtr parse_value();

    virtual ConfigOriginPtr origin() override;

protected:
    virtual ConfigOriginPtr create_origin() = 0;

public:
    virtual ConfigParseOptionsPtr options() override;

    virtual std::string to_string() override;

protected:
    static ConfigSyntax syntax_from_extension(const std::string& name);

public:
    static std::string relative_to(const std::string& file,
                                   const std::string& filename);

    static ParseablePtr new_not_found(const std::string& what_not_found,
                                        const std::string& message,
                                        const ConfigParseOptionsPtr& options);
    static ParseablePtr new_reader(const ReaderPtr& reader,
                                     const ConfigParseOptionsPtr& options);
    static ParseablePtr new_string(const std::string& input,
                                     const ConfigParseOptionsPtr& options);
    static ParseablePtr new_file(const std::string& input,
                                   const ConfigParseOptionsPtr& options);

private:
    ConfigParseOptionsPtr base_options;
    ConfigIncludeContextPtr include_context_;
    ConfigParseOptionsPtr initial_options;
    ConfigOriginPtr initial_origin;

    /// _note: this tracks the current parse stack and is NOT threadsafe.
    static StackParseable parse_stack;

    static const uint32_t MAX_INCLUDE_DEPTH = 50;
};

///
/// _this is a parseable that doesn't exist and just throws when you try to
/// parse it.
///
class ParseableNotFound : public Parseable {
public:
    CONFIG_CLASS(ParseableNotFound);

    ParseableNotFound(const std::string& what,
                         const std::string& message,
                         const ConfigParseOptionsPtr& options);

protected:
    virtual ReaderPtr reader() override;
    virtual ConfigOriginPtr create_origin() override;

private:
    std::string what;
    std::string message;
};

class ParseableReader : public Parseable {
public:
    CONFIG_CLASS(ParseableReader);

    ParseableReader(const ReaderPtr& reader,
                      const ConfigParseOptionsPtr& options);

protected:
    virtual ReaderPtr reader() override;
    virtual ConfigOriginPtr create_origin() override;

private:
    ReaderPtr reader_;
};

class ParseableString : public Parseable {
public:
    CONFIG_CLASS(ParseableString);

    ParseableString(const std::string& input,
                      const ConfigParseOptionsPtr& options);

protected:
    virtual ReaderPtr reader() override;
    virtual ConfigOriginPtr create_origin() override;

public:
    virtual std::string to_string() override;

private:
    std::string input;
};

class ParseableFile : public Parseable {
public:
    CONFIG_CLASS(ParseableFile);

    ParseableFile(const std::string& input,
                    const ConfigParseOptionsPtr& options);

protected:
    virtual ReaderPtr reader() override;

public:
    virtual ConfigSyntax guess_syntax() override;

    using Parseable::relative_to;
    virtual ConfigParseablePtr relative_to(const std::string& filename) override;

protected:
    virtual ConfigOriginPtr create_origin() override;

public:
    virtual std::string to_string() override;

private:
    std::string input;
};

}

#endif // CONFIG_PARSEABLE_HPP
