
#include "detail/memo_key.hpp"
#include "detail/abstract_config_value.hpp"
#include "detail/path.hpp"

namespace config {

MemoKey::MemoKey(const AbstractConfigValuePtr& value,
                     const PathPtr& restrict_to_child_or_null) :
    value(value),
    restrict_to_child_or_null(restrict_to_child_or_null) {
}

uint32_t MemoKey::hash_code() {
    uint32_t hash = value->ConfigBase::hash_code();

    if (restrict_to_child_or_null) {
        return hash + 41 * (41 + restrict_to_child_or_null->hash_code());
    } else {
        return hash;
    }
}

bool MemoKey::equals(const ConfigVariant& other) {
    if (instanceof<MemoKey>(other)) {
        auto o = static_get<MemoKey>(other);

        if (o->value != this->value) {
            return false;
        } else if (o->restrict_to_child_or_null == this->restrict_to_child_or_null) {
            return true;
        } else if (!o->restrict_to_child_or_null || !this->restrict_to_child_or_null) {
            return false;
        } else {
            return o->restrict_to_child_or_null->equals(this->restrict_to_child_or_null);
        }
    } else {
        return false;
    }
}

}

