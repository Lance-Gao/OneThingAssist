
#include "detail/config_impl.hpp"
#include "detail/parseable.hpp"
#include "detail/abstract_config_object.hpp"
#include "config.hpp"
#include "config_object.hpp"
#include "config_parse_options.hpp"
#include "config_resolve_options.hpp"

namespace config {

ConfigPtr Config::load(const std::string& file_basename) {
    return load(file_basename, ConfigParseOptions::defaults(), ConfigResolveOptions::defaults());
}

ConfigPtr Config::load(const std::string& file_basename,
                          const ConfigParseOptionsPtr& parse_options,
                          const ConfigResolveOptionsPtr& resolve_options) {
    auto app_config = parse_file_any_syntax(file_basename, parse_options);
    return load(app_config, resolve_options);
}

ConfigPtr Config::load(const ConfigPtr& config,
                          const ConfigResolveOptionsPtr& resolve_options) {
    return std::dynamic_pointer_cast<Config>(default_overrides()->with_fallback(config))->resolve(
               resolve_options);
}

ConfigPtr Config::default_overrides() {
    return ConfigImpl::env_variables_as_config();
}

ConfigPtr Config::empty_config(const std::string& origin_description) {
    return ConfigImpl::empty_config(origin_description);
}

ConfigPtr Config::parse_reader(const ReaderPtr& reader,
                                  const ConfigParseOptionsPtr& options) {
    if (options) {
        return Parseable::new_reader(reader, options)->parse()->to_config();
    } else {
        return Parseable::new_reader(reader, ConfigParseOptions::defaults())->parse()->to_config();
    }
}

ConfigPtr Config::parse_file(const std::string& file, const ConfigParseOptionsPtr& options) {
    if (options) {
        return Parseable::new_file(file, options)->parse()->to_config();
    } else {
        return Parseable::new_file(file, ConfigParseOptions::defaults())->parse()->to_config();
    }
}

ConfigPtr Config::parse_file_any_syntax(const std::string& file_basename,
        const ConfigParseOptionsPtr& options) {
    if (options) {
        return ConfigImpl::parse_file_any_syntax(file_basename, options)->to_config();
    } else {
        return ConfigImpl::parse_file_any_syntax(file_basename,
                ConfigParseOptions::defaults())->to_config();
    }
}

ConfigPtr Config::parse_string(const std::string& s, const ConfigParseOptionsPtr& options) {
    if (options) {
        return Parseable::new_string(s, options)->parse()->to_config();
    } else {
        return Parseable::new_string(s, ConfigParseOptions::defaults())->parse()->to_config();
    }
}

ConfigPtr Config::parse_map(const MapVariant& values, const std::string& origin_description) {
    return ConfigImpl::from_path_map(values, origin_description)->to_config();
}

}
