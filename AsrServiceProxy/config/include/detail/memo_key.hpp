
#ifndef CONFIG_MEMO_KEY_HPP
#define CONFIG_MEMO_KEY_HPP

#include "detail/config_base.hpp"

namespace config {

///
/// _the key used to memoize already-traversed nodes when resolving substitutions.
///
class MemoKey : public ConfigBase {
public:
    CONFIG_CLASS(MemoKey);

    MemoKey(const AbstractConfigValuePtr& value, const PathPtr& restrict_to_child_or_null);

    virtual uint32_t hash_code() override;
    virtual bool equals(const ConfigVariant& other) override;

private:
    AbstractConfigValuePtr value;
    PathPtr restrict_to_child_or_null;
};

}

#endif // CONFIG_MEMO_KEY_HPP

