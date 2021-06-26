
#include "detail/config_string.hpp"
#include "detail/config_impl_util.hpp"
#include "config_value_type.hpp"
#include "config_render_options.hpp"

namespace config {

ConfigString::ConfigString(const ConfigOriginPtr& origin, const std::string& value) :
    AbstractConfigValue(origin),
    value(value) {
}

ConfigValueType ConfigString::value_type() {
    return ConfigValueType::STRING;
}

ConfigVariant ConfigString::unwrapped() {
    return value;
}

std::string ConfigString::transform_to_string() {
    return value;
}

void ConfigString::render(std::string& s, uint32_t indent,
                            const ConfigRenderOptionsPtr& options) {
    std::string rendered;

    if (options->get_json()) {
        rendered = ConfigImplUtil::render_json_string(value);
    } else {
        rendered = ConfigImplUtil::render_string_unquoted_if_possible(value);
    }

    s += rendered;
}

AbstractConfigValuePtr ConfigString::new_copy(const ConfigOriginPtr& origin) {
    return make_instance(origin, value);
}

}
