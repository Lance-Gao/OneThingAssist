
#ifndef CONFIG_CONFIG_IMPL_HPP
#define CONFIG_CONFIG_IMPL_HPP

#include "detail/config_base.hpp"
#include "detail/simple_includer.hpp"
#include "config_exception.hpp"

namespace config {

class ConfigImpl {
public:
    /// _for use ONLY by library internals, DO NOT TOUCH not guaranteed ABI
    static ConfigObjectPtr parse_file_any_syntax(const std::string& basename,
            const ConfigParseOptionsPtr& base_options);

    static AbstractConfigObjectPtr empty_object(const std::string& origin_description);

    /// _for use ONLY by library internals, DO NOT TOUCH not guaranteed ABI
    static ConfigPtr empty_config(const std::string& origin_description);

private:
    /// default origin for values created with from_any_ref and no origin specified
    static ConfigOriginPtr default_value_origin();

public:
    static AbstractConfigObjectPtr empty_object(const ConfigOriginPtr& origin);

private:
    static SimpleConfigList empty_list(const ConfigOriginPtr& origin);
    static ConfigOriginPtr value_origin(const std::string& origin_description);

public:
    /// _for use ONLY by library internals, DO NOT TOUCH not guaranteed ABI
    static ConfigValuePtr from_any_ref(const ConfigVariant& object,
                                          const std::string& origin_description);

    /// _for use ONLY by library internals, DO NOT TOUCH not guaranteed ABI
    static ConfigObjectPtr from_path_map(const MapVariant& path_map,
                                            const std::string& origin_description);

    static AbstractConfigValuePtr from_any_ref(const ConfigVariant& object,
            const ConfigOriginPtr& origin,
            FromMapMode map_mode);

    static AbstractConfigObjectPtr from_path_map(const ConfigOriginPtr& origin,
            const MapVariant& path_expression_map);

    static ConfigIncluderPtr default_includer();

private:
    static MapString load_env_variables();

public:
    /// _for use ONLY by library internals, DO NOT TOUCH not guaranteed ABI
    static ConfigPtr env_variables_as_config();

    static AbstractConfigObjectPtr env_variables_as_config_object();

    /// _for use ONLY by library internals, DO NOT TOUCH not guaranteed ABI
    static bool trace_loads_enabled();

    static void trace(const std::string& message);

    /// _the basic idea here is to add the "what" and have a canonical
    /// toplevel error message. the "original" exception may however have extra
    /// detail about what happened. call this if you have a better "what" than
    /// further down on the stack.
    static ConfigExceptionNotResolved improve_not_resolved(const PathPtr& what,
            const ConfigExceptionNotResolved& original);
};

class FileNameSource : public virtual NameSource, public ConfigBase {
public:
    CONFIG_CLASS(FileNameSource);

    virtual ConfigParseablePtr name_to_parseable(const std::string& name,
            const ConfigParseOptionsPtr& parse_options) override;
};

}

#endif // CONFIG_CONFIG_IMPL_HPP
