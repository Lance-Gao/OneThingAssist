
#ifndef CONFIG_CONFIG_FULL_INCLUDER_HPP
#define CONFIG_CONFIG_FULL_INCLUDER_HPP

#include "config_includer.hpp"
#include "config_includer_file.hpp"

namespace config {

class FullIncluder : public virtual ConfigIncluder, public virtual ConfigIncluderFile {
};

}

#endif // CONFIG_CONFIG_FULL_INCLUDER_HPP

