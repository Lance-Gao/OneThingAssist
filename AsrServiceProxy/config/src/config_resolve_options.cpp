
#include "config_resolve_options.hpp"

namespace config {

ConfigResolveOptions::ConfigResolveOptions(bool use_system_environment) :
    use_system_environment(use_system_environment) {
}

ConfigResolveOptionsPtr ConfigResolveOptions::defaults() {
    return make_instance(true);
}

ConfigResolveOptionsPtr ConfigResolveOptions::no_system() {
    return make_instance(false);
}

ConfigResolveOptionsPtr ConfigResolveOptions::set_use_system_environment(bool value) {
    return make_instance(value);
}

bool ConfigResolveOptions::get_use_system_environment() {
    return use_system_environment;
}

}
