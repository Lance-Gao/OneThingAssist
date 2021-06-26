
#ifndef CONFIG_SIMPLE_INCLUDE_CONTEXT_HPP
#define CONFIG_SIMPLE_INCLUDE_CONTEXT_HPP

#include "detail/config_base.hpp"
#include "config_include_context.hpp"

namespace config {

class SimpleIncludeContext : public virtual ConfigIncludeContext, public ConfigBase {
public:
    CONFIG_CLASS(SimpleIncludeContext);

    SimpleIncludeContext(const ParseablePtr& parseable);

    SimpleIncluderContextPtr with_parseable(const ParseablePtr& parseable);

    virtual ConfigParseablePtr relative_to(const std::string& filename) override;
    virtual ConfigParseOptionsPtr parse_options() override;

private:
    ParseablePtr parseable;
};

}

#endif // CONFIG_SIMPLE_INCLUDE_CONTEXT_HPP
