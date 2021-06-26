
#include "config_value.hpp"
#include "config_object.hpp"
#include "config_list.hpp"
#include "detail/config_impl.hpp"

namespace config {

ConfigValuePtrConfigValue::from_any_ref(const ConfigVariant& object,
        const std::string& origin_description) {
    return ConfigImpl::from_any_ref(object, origin_description);
}

ConfigObjectPtrConfigValue::from_map(const MapVariant& values,
        const std::string& origin_description) {
    return std::dynamic_pointer_cast<ConfigObject>(from_any_ref(values, origin_description));
}

ConfigListPtrConfigValue::from_vector(const VectorVariant& values,
        const std::string& origin_description) {
    return std::dynamic_pointer_cast< ConfigList>(from_any_ref(values, origin_description));
}

}
