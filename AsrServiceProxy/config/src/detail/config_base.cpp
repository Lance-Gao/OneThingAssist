
#include "detail/config_base.hpp"
#include "detail/variant_utils.hpp"

namespace config {

ConfigBase::~ConfigBase() {
}

void ConfigBase::initialize() {
    // override
}

uint32_t ConfigBase::hash_code() {
    return (uint32_t)(uint64_t)this;
}

bool ConfigBase::equals(const ConfigVariant& other) {
    if (!instanceof<ConfigBasePtr>(other)) {
        return false;
    }

    return this == variant_get<ConfigBasePtr>(other).get();
}

std::string ConfigBase::to_string() {
    return get_class_name() + " " + boost::lexical_cast<std::string>(this);
}

std::string ConfigBase::get_class_name() {
    return "ConfigBase";
}

}
