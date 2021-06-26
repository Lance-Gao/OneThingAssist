
#ifndef CONFIG_REPLACEABLE_MERGE_STACK_HPP
#define CONFIG_REPLACEABLE_MERGE_STACK_HPP

#include "config_types.hpp"

namespace config {

///
/// _implemented by a merge stack (ConfigDelayedMerge, ConfigDelayedMergeObject)
/// that replaces itself during substitution resolution in order to implement
/// "look backwards only" semantics.
///
class ReplaceableMergeStack {
public:
    /// _make a replacer for this object, skipping the given number of items in
    /// the stack.
    virtual ResolveReplacerPtr make_replacer(uint32_t skipping) = 0;
};

}

#endif // CONFIG_REPLACEABLE_MERGE_STACK_HPP

