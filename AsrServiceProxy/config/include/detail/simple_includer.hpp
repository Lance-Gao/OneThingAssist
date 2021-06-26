
#ifndef CONFIG_SIMPLE_INCLUDER_HPP
#define CONFIG_SIMPLE_INCLUDER_HPP

#include "detail/config_base.hpp"
#include "detail/full_includer.hpp"

namespace config {

class SimpleIncluder : public virtual FullIncluder, public ConfigBase {
public:
    CONFIG_CLASS(SimpleIncluder);

    SimpleIncluder(const ConfigIncluderPtr& fallback);

    /// ConfigIncludeContext does this for us on its options
    static ConfigParseOptionsPtr clear_for_include(const ConfigParseOptionsPtr& options);

    /// _this is the heuristic includer
    virtual ConfigObjectPtr include(const ConfigIncludeContextPtr& context,
                                       const std::string& what) override;

    /// _the heuristic includer in static form
    static ConfigObjectPtr include_without_fallback(const ConfigIncludeContextPtr& context,
            const std::string& name);

    virtual ConfigObjectPtr include_file(const ConfigIncludeContextPtr& context,
                                            const std::string& file) override;

    static ConfigObjectPtr include_file_without_fallback(const ConfigIncludeContextPtr& context,
            const std::string& file);

    virtual ConfigIncluderPtr with_fallback(const ConfigIncluderPtr& fallback) override;

    /// _this function is a little tricky because there are two places we're
    /// trying to use it; for 'include "basename"' in a .conf file, for
    /// loading app.{conf,json} from the filesystem.
    static ConfigObjectPtr from_basename(const NameSourcePtr& source,
                                            const std::string& name,
                                            const ConfigParseOptionsPtr& options);

    static FullIncluderPtr make_full(const ConfigIncluderPtr& includer);

private:
    ConfigIncluderPtr fallback;
};

class NameSource {
public:
    virtual ConfigParseablePtr name_to_parseable(const std::string& name,
            const ConfigParseOptionsPtr& parse_options) = 0;
};

class RelativeNameSource : public virtual NameSource, public ConfigBase {
public:
    CONFIG_CLASS(RelativeNameSource);

    RelativeNameSource(const ConfigIncludeContextPtr& context);

    virtual ConfigParseablePtr name_to_parseable(const std::string& name,
            const ConfigParseOptionsPtr& parse_options) override;

private:
    ConfigIncludeContextPtr context;
};

///
/// _the _proxy is a proxy for an application-provided includer that uses our
/// default implementations when the application-provided includer doesn't
/// have an implementation.
///
class FullIncluderProxy : public virtual FullIncluder, public ConfigBase {
public:
    CONFIG_CLASS(FullIncluderProxy);

    FullIncluderProxy(const ConfigIncluderPtr& delegate);

    virtual ConfigIncluderPtr with_fallback(const ConfigIncluderPtr& fallback) override;
    virtual ConfigObjectPtr include(const ConfigIncludeContextPtr& context,
                                       const std::string& what) override;
    virtual ConfigObjectPtr include_file(const ConfigIncludeContextPtr& context,
                                            const std::string& file) override;

public:
    ConfigIncluderPtr delegate;
};

}

#endif // CONFIG_SIMPLE_INCLUDER_HPP
