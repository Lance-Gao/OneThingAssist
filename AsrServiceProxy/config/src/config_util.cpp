
#include "config_util.hpp"
#include "detail/config_impl_util.hpp"

namespace config {

ConfigUtil::ConfigUtil() {
    // private
}

std::string ConfigUtil::quote_string(const std::string& s) {
    return ConfigImplUtil::render_json_string(s);
}

std::string ConfigUtil::join_path(const VectorString& elements) {
    return ConfigImplUtil::join_path(elements);
}

VectorString ConfigUtil::split_path(const std::string& path) {
    return ConfigImplUtil::split_path(path);
}

}
