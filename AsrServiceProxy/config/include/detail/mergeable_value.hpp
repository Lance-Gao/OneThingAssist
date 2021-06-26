
#ifndef CONFIG_MERGEABLE_VALUE_HPP
#define CONFIG_MERGEABLE_VALUE_HPP

#include "config_mergeable.hpp"

namespace config {

class MergeableValue : public virtual ConfigMergeable {
public:
    /// _converts a Config to its root object and aConfigValue to itself
    virtual ConfigValuePtr to_fallback_value() = 0;
};

}

#endif // CONFIG_MERGEABLE_VALUE_HPP

