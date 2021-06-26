
#ifndef CONFIG_CONFIG_FROM_MAP_MODE_HPP
#define CONFIG_CONFIG_FROM_MAP_MODE_HPP

#include "config_types.hpp"

namespace config {

enum class FromMapMode : public uint32_t {
    KEYS_ARE_PATHS, KEYS_ARE_KEYS
};

}

#endif // CONFIG_CONFIG_FROM_MAP_MODE_HPP

