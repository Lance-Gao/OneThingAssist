
#include "detail/simple_includer.hpp"
#include "detail/parseable.hpp"
#include "detail/simple_config_object.hpp"
#include "detail/simple_config_origin.hpp"
#include "config_includer_file.hpp"
#include "config.hpp"
#include "config_syntax.hpp"
#include "config_parseable.hpp"
#include "config_include_context.hpp"
#include "config_object.hpp"
#include "config_parse_options.hpp"
#include "config_exception.hpp"

namespace config {

SimpleIncluder::SimpleIncluder(const ConfigIncluderPtr& fallback) :
    fallback(fallback) {
}

ConfigParseOptionsPtr SimpleIncluder::clear_for_include(const ConfigParseOptionsPtr&
        options) {
    // the class loader and includer are inherited, but not this other stuff.
    return options->set_syntax(ConfigSyntax::NONE)->set_origin_description("")->set_allow_missing(
               true);
}

ConfigObjectPtr SimpleIncluder::include(const ConfigIncludeContextPtr& context,
        const std::string& name) {
    auto obj = include_without_fallback(context, name);

    // now use the fallback includer if any and merge its result.
    if (fallback) {
        return std::dynamic_pointer_cast<ConfigObject>(obj->with_fallback(fallback->include(context,
                name)));
    } else {
        return obj;
    }
}

ConfigObjectPtr SimpleIncluder::include_without_fallback(const ConfigIncludeContextPtr&
        context,
        const std::string& name) {
    auto source = RelativeNameSource::make_instance(context);
    return from_basename(source, name, context->parse_options());
}

ConfigObjectPtr SimpleIncluder::include_file(const ConfigIncludeContextPtr& context,
        const std::string& file) {
    auto obj = include_file_without_fallback(context, file);

    // now use the fallback includer if any and merge its result.
    if (fallback && instanceof<ConfigIncluderFile>(fallback)) {
        return std::dynamic_pointer_cast<ConfigObject>(obj->with_fallback(
                    std::dynamic_pointer_cast<ConfigIncluderFile>(fallback)->include_file(context, file)));
    } else {
        return obj;
    }
}

ConfigObjectPtr SimpleIncluder::include_file_without_fallback(const
        ConfigIncludeContextPtr& context,
        const std::string& file) {
    return Config::parse_file_any_syntax(file, context->parse_options())->root();
}

ConfigIncluderPtr SimpleIncluder::with_fallback(const ConfigIncluderPtr& fallback) {
    if (shared_from_this() == fallback) {
        throw ConfigExceptionBugOrBroken("trying to create includer cycle");
    } else if (this->fallback == fallback) {
        return shared_from_this();
    } else if (this->fallback) {
        return make_instance(this->fallback->with_fallback(fallback));
    } else {
        return make_instance(fallback);
    }
}

ConfigObjectPtr SimpleIncluder::from_basename(const NameSourcePtr& source,
        const std::string& name,
        const ConfigParseOptionsPtr& options) {
    ConfigObjectPtr obj;

    if (boost::ends_with(name, ".conf") || boost::ends_with(name, ".json")) {
        auto p = source->name_to_parseable(name, options);
        obj = p->parse(p->options()->set_allow_missing(options->get_allow_missing()));
    } else {
        auto conf_handle = source->name_to_parseable(name + ".conf", options);
        auto json_handle = source->name_to_parseable(name + ".json", options);
        bool got_something = false;
        VectorString fail_messages;

        ConfigSyntax syntax = options->get_syntax();

        obj = SimpleConfigObject::make_empty(SimpleConfigOrigin::new_simple(name));

        if (syntax == ConfigSyntax::NONE || syntax == ConfigSyntax::CONF) {
            try {
                obj = conf_handle->parse(conf_handle->options()->set_allow_missing(false)->set_syntax(
                                             ConfigSyntax::CONF));
                got_something = true;
            } catch (ConfigExceptionIO& e) {
                fail_messages.push_back(e.what());
            }
        }

        if (syntax == ConfigSyntax::NONE || syntax == ConfigSyntax::JSON) {
            try {
                auto parsed = json_handle->parse(json_handle->options()->set_allow_missing(false)->set_syntax(
                                                     ConfigSyntax::JSON));
                obj = std::dynamic_pointer_cast<ConfigObject>(obj->with_fallback(parsed));
                got_something = true;
            } catch (ConfigExceptionIO& e) {
                fail_messages.push_back(e.what());
            }
        }

        if (!options->get_allow_missing() && !got_something) {
            std::string fail_message;

            if (fail_messages.empty()) {
                // this should not happen
                throw ConfigExceptionBugOrBroken("should not be reached: nothing found but no exceptions thrown");
            } else {
                std::ostringstream stream;

                for (std::string msg : fail_messages) {
                    stream << msg << ", ";
                }

                fail_message = stream.str();
                fail_message = fail_message.substr(0, fail_message.length() - 2);
            }

            throw ConfigExceptionIO(SimpleConfigOrigin::new_simple(name), fail_message);
        }
    }

    return obj;
}

FullIncluderPtr SimpleIncluder::make_full(const ConfigIncluderPtr& includer) {
    if (instanceof<FullIncluder>(includer)) {
        return std::dynamic_pointer_cast<FullIncluder>(includer);
    } else {
        return FullIncluderProxy::make_instance(includer);
    }
}

RelativeNameSource::RelativeNameSource(const ConfigIncludeContextPtr& context) :
    context(context) {
}

ConfigParseablePtr RelativeNameSource::name_to_parseable(const std::string& name,
        const ConfigParseOptionsPtr& options) {
    auto p = context->relative_to(name);

    if (!p) {
        // avoid returning null
        return Parseable::new_not_found(name, "include was not found: '" + name + "'", options);
    } else {
        return p;
    }
}

FullIncluderProxy::FullIncluderProxy(const ConfigIncluderPtr& delegate) :
    delegate(delegate) {
}

ConfigIncluderPtr FullIncluderProxy::with_fallback(const ConfigIncluderPtr& fallback) {
    // we never fall back
    return shared_from_this();
}

ConfigObjectPtr FullIncluderProxy::include(const ConfigIncludeContextPtr& context,
        const std::string& what) {
    return delegate->include(context, what);
}

ConfigObjectPtr FullIncluderProxy::include_file(const ConfigIncludeContextPtr& context,
        const std::string& file) {
    if (instanceof<ConfigIncluderFile>(delegate)) {
        return std::dynamic_pointer_cast<ConfigIncluderFile>(delegate)->include_file(context, file);
    } else {
        return SimpleIncluder::include_file_without_fallback(context, file);
    }
}

}
