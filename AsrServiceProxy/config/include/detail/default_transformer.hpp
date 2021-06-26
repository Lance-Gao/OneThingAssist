
#ifndef CONFIG_CONFIG_DEFAULT_TRANSFORMER_HPP
#define CONFIG_CONFIG_DEFAULT_TRANSFORMER_HPP

#include "config_types.hpp"

namespace config {

///
/// _default automatic type transformations.
///
class DefaultTransformer {
public:
    static AbstractConfigValuePtr transform(const AbstractConfigValuePtr& value,
            ConfigValueType requested);
};

}

#endif // CONFIG_CONFIG_DEFAULT_TRANSFORMER_HPP
