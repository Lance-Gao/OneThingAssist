
#ifndef CONFIG_RESOLVE_MEMOS_HPP
#define CONFIG_RESOLVE_MEMOS_HPP

#include "detail/config_base.hpp"

namespace config {

///
/// _this exists because we have to memoize resolved substitutions as we go
/// through the config tree; otherwise we could end up creating multiple copies
/// of values or whole trees of values as we follow chains of substitutions.
///
class ResolveMemos : public ConfigBase {
public:
    CONFIG_CLASS(ResolveMemos);

    AbstractConfigValuePtr get(const MemoKeyPtr& key);
    void put(const MemoKeyPtr& key, const AbstractConfigValuePtr& value);

private:
    // note that we can resolve things to undefined (represented as null,
    // rather than ConfigNull) so this map can have null values.
    MapMemoKeyAbstractConfigValue _memos;
};

}

#endif // CONFIG_RESOLVE_MEMOS_HPP
