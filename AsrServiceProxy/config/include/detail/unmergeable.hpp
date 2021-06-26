
#ifndef CONFIG_UNMERGEABLE_HPP
#define CONFIG_UNMERGEABLE_HPP

#include "config_types.hpp"

namespace config {

///
/// _interface that tags aConfigValue that is not mergeable until after
/// substitutions are resolved. _basically these are specialConfigValue that
/// never appear in a resolved tree, like {@link _config_substitution} and
/// {@link ConfigDelayedMerge}.
///
class Unmergeable {
public:
    virtual VectorAbstractConfigValue unmerged_values() = 0;
};

}

#endif // CONFIG_UNMERGEABLE_HPP
