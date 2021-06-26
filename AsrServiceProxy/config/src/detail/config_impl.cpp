
#include "detail/config_impl.hpp"
#include "detail/parseable.hpp"
#include "detail/simple_includer.hpp"
#include "detail/simple_config_origin.hpp"
#include "detail/simple_config_object.hpp"
#include "detail/simple_config_list.hpp"
#include "detail/abstract_config_object.hpp"
#include "detail/from_map_mode.hpp"
#include "detail/config_boolean.hpp"
#include "detail/config_null.hpp"
#include "detail/config_string.hpp"
#include "detail/config_int.hpp"
#include "detail/config_int64.hpp"
#include "detail/config_double.hpp"
#include "detail/path.hpp"

#ifndef WIN32
#if !HAVE_DECL_ENVIRON
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*__n_s_get_environ())
#else
extern char** environ;
#endif
#endif
#endif

namespace config {

ConfigParseablePtr FileNameSource::name_to_parseable(const std::string& name,
        const ConfigParseOptionsPtr& parse_options) {
    return Parseable::new_file(name, parse_options);
}

ConfigObjectPtr ConfigImpl::parse_file_any_syntax(const std::string& basename,
        const ConfigParseOptionsPtr& base_options) {
    auto source = FileNameSource::make_instance();
    return SimpleIncluder::from_basename(source, basename, base_options);
}

AbstractConfigObjectPtr ConfigImpl::empty_object(const std::string& origin_description) {
    auto origin = !origin_description.empty() ? SimpleConfigOrigin::new_simple(
                      origin_description) : nullptr;
    return empty_object(origin);
}

ConfigPtr ConfigImpl::empty_config(const std::string& origin_description) {
    return empty_object(origin_description)->to_config();
}

ConfigOriginPtr ConfigImpl::default_value_origin() {
    static auto default_value_origin_ = SimpleConfigOrigin::new_simple("hardcoded value");
    return default_value_origin_;
}

AbstractConfigObjectPtr ConfigImpl::empty_object(const ConfigOriginPtr& origin) {
    static auto default_empty_object = SimpleConfigObject::make_empty(default_value_origin());

    // we want null origin to go to SimpleConfigObject::make_empty() to get the
    // origin "empty config" rather than "hardcoded value"
    if (origin == default_value_origin()) {
        return default_empty_object;
    } else {
        return SimpleConfigObject::make_empty(origin);
    }
}

SimpleConfigList ConfigImpl::empty_list(const ConfigOriginPtr& origin) {
    static auto default_empty_list = SimpleConfigList::make_instance(default_value_origin(),
                                     VectorAbstractConfigValue());

    if (!origin || origin == default_value_origin()) {
        return default_empty_list;
    } else {
        return SimpleConfigList::make_instance(origin, VectorAbstractConfigValue());
    }
}

ConfigOriginPtr ConfigImpl::value_origin(const std::string& origin_description) {
    if (origin_description.empty()) {
        return default_value_origin();
    } else {
        return SimpleConfigOrigin::new_simple(origin_description);
    }
}

ConfigValuePtr ConfigImpl::from_any_ref(const ConfigVariant& object,
        const std::string& origin_description) {
    auto origin = value_origin(origin_description);
    return std::dynamic_pointer_cast<ConfigValue>(from_any_ref(object, origin,
            FromMapMode::KEYS_ARE_KEYS));
}

ConfigObjectPtr ConfigImpl::from_path_map(const MapVariant& path_map,
        const std::string& origin_description) {
    auto origin = value_origin(origin_description);
    return std::dynamic_pointer_cast<ConfigObject>(from_any_ref(path_map, origin,
            FromMapMode::KEYS_ARE_PATHS));
}

AbstractConfigValuePtr ConfigImpl::from_any_ref(const ConfigVariant& object,
        const ConfigOriginPtr& origin, FromMapMode map_mode) {
    static auto default_true_value = ConfigBoolean::make_instance(default_value_origin(), true);
    static auto default_false_value = ConfigBoolean::make_instance(default_value_origin(), false);
    static auto default_null_value = ConfigNull::make_instance(default_value_origin());

    if (!origin) {
        throw ConfigExceptionBugOrBroken("origin not supposed to be null");
    }

    if (instanceof<null>(object)) {
        if (origin != default_value_origin()) {
            return ConfigNull::make_instance(origin);
        } else {
            return default_null_value;
        }
    }

    if (instanceof<bool>(object)) {
        if (origin != default_value_origin()) {
            return ConfigBoolean::make_instance(origin, variant_get<bool>(object));
        } else if (variant_get<bool>(object)) {
            return default_true_value;
        } else {
            return default_false_value;
        }
    }

    if (instanceof<std::string>(object)) {
        return ConfigString::make_instance(origin, variant_get<std::string>(object));
    } else if (instanceof<int32_t>(object)) {
        return ConfigInt::make_instance(origin, variant_get<int32_t>(object), "");
    } else if (instanceof<int64_t>(object)) {
        return ConfigInt64::make_instance(origin, variant_get<int64_t>(object), "");
    } else if (instanceof<double>(object)) {
        return ConfigDouble::make_instance(origin, variant_get<double>(object), "");
    } else if (instanceof<MapVariant>(object)) {
        if (variant_get<MapVariant>(object).empty()) {
            return empty_object(origin);
        }

        if (map_mode == FromMapMode::KEYS_ARE_KEYS) {
            MapAbstractConfigValue values;

            for (auto& entry : variant_get<MapVariant>(object)) {
                auto value = from_any_ref(entry.second, origin, map_mode);
                values[entry.first] = value;
            }

            return SimpleConfigObject::make_instance(origin, values);
        } else {
            return from_path_map(origin, variant_get<MapVariant>(object));
        }
    } else if (instanceof<VectorVariant>(object)) {
        if (variant_get<VectorVariant>(object).empty()) {
            return empty_list(origin);
        }

        VectorAbstractConfigValue values;

        for (auto& i : variant_get<VectorVariant>(object)) {
            auto v = from_any_ref(i, origin, map_mode);
            values.push_back(v);
        }

        return SimpleConfigList::make_instance(origin, values);
    } else {
        std::ostringstream stream;
        stream << "bug in method caller: not valid to createConfigValue from: ";
        stream << boost::apply_visitor(_variant_string(), object);
        throw ConfigExceptionBugOrBroken(stream.str());
    }
}

AbstractConfigObjectPtr ConfigImpl::from_path_map(const ConfigOriginPtr& origin,
        const MapVariant& path_expression_map) {
    MapPathVariant path_map;

    for (auto& entry : path_expression_map) {
        auto path = Path::new_path(entry.first);
        path_map[path] = entry.second;
    }

    // _first, build a list of paths that will have values, either string or
    // object values.
    SetPath scope_paths;
    SetPath value_paths;

    for (auto& path : path_map) {
        // add value's path
        value_paths.insert(path.first);

        // all parent paths are objects
        auto next = path.first->parent();

        while (next) {
            scope_paths.insert(next);
            next = next->parent();
        }
    }

    for (auto& path : value_paths) {
        if (scope_paths.find(path) != scope_paths.end()) {
            throw ConfigExceptionBugOrBroken("_in the map, path '" + path->render() +
                                                  "' occurs as both the parent object of a value and as a value. "
                                                  "_because _map has no defined ordering, this is a broken situation.");
        }
    }

    // _create maps for the object-valued values.
    MapAbstractConfigValue root;
    MapPathMapAbstractConfigValue scopes;

    for (auto& path : scope_paths) {
        scopes[path] = MapAbstractConfigValue();
    }

    // _store string values in the associated scope maps
    for (auto& path : value_paths) {
        auto parent_path = path->parent();
        MapAbstractConfigValue& parent = parent_path ? scopes[parent_path] : root;
        std::string last = path->last();
        ConfigVariant raw_value = path_map[path];
        auto value = ConfigImpl::from_any_ref(path_map[path], origin, FromMapMode::KEYS_ARE_PATHS);
        parent[last] = value;
    }

    // _make a list of scope paths from longest to shortest, so children go
    // before parents.
    VectorPath sorted_scope_paths(scope_paths.begin(), scope_paths.end());
    // sort descending by length
    std::sort(sorted_scope_paths.begin(), sorted_scope_paths.end(),
    [](const PathPtr & first, const PathPtr & second) {
        return second->length() < first->length();
    }
             );

    /// _create ConfigObject for each scope map, working from children to
    /// parents to avoid modifying any already-created ConfigObject. _this is
    /// where we need the sorted list.

    for (auto& scope_path : sorted_scope_paths) {
        auto parent_path = scope_path->parent();
        auto& parent = parent_path ? scopes[parent_path] : root;
        auto o = SimpleConfigObject::make_instance(origin, scopes[scope_path],
                 ResolveStatus::RESOLVED, false);
        parent[scope_path->last()] = o;
    }

    // return root config object
    return SimpleConfigObject::make_instance(origin, root, ResolveStatus::RESOLVED, false);
}

ConfigIncluderPtr ConfigImpl::default_includer() {
    static auto _default_includer = SimpleIncluder::make_instance(nullptr);
    return _default_includer;
}

MapString ConfigImpl::load_env_variables() {
    static MapString env;

    if (env.empty()) {
        VectorString env_list;
#ifdef WIN32
        LPCCH env_strings = _get_environment_strings();
        LPCSTR var = (LPTSTR)env_strings;

        while (*var) {
            env_list.push_back(var);
            var += strlen(var) + 1;
        }

        _free_environment_strings(env_strings);
#else

        for (uint32_t n = 0; environ[n]; ++n) {
            env_list.push_back(environ[n]);
        }

#endif
        VectorString kv;

        for (auto& env_pair : env_list) {
            boost::split(kv, env_pair, boost::is_any_of("="));

            if (kv.size() == 2) {
                env[kv[0]] = kv[1];
            }
        }
    }

    return env;
}

ConfigPtr ConfigImpl::env_variables_as_config() {
    return env_variables_as_config_object()->to_config();
}

AbstractConfigObjectPtr ConfigImpl::env_variables_as_config_object() {
    MapString env = load_env_variables();
    MapAbstractConfigValue m;

    for (auto& entry : env) {
        m[entry.first] = ConfigString::make_instance(SimpleConfigOrigin::new_simple("env var " +
                         entry.first), entry.second);
    }

    return SimpleConfigObject::make_instance(SimpleConfigOrigin::new_simple("env variables"), m,
            ResolveStatus::RESOLVED, false);
}

bool ConfigImpl::trace_loads_enabled() {
    MapString env = load_env_variables();
    return env.find("config.trace") != env.end();
}

void ConfigImpl::trace(const std::string& message) {
    std::cerr << message << std::endl;
}

ConfigExceptionNotResolved ConfigImpl::improve_not_resolved(const PathPtr& what,
        const ConfigExceptionNotResolved& original) {
    std::string new_message = what->render() +
                              " has not been resolved, you need to call Config::resolve()," +
                              " see API docs for Config#resolve()";

    if (new_message == original.what()) {
        return original;
    } else {
        return ConfigExceptionNotResolved(new_message);
    }
}

}
