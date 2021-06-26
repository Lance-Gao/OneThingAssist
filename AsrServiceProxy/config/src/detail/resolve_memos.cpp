
#include "detail/resolve_memos.hpp"
#include "detail/memo_key.hpp"

namespace config {

AbstractConfigValuePtr ResolveMemos::get(const MemoKeyPtr& key) {
    auto value = _memos.find(key);
    return value == _memos.end() ? nullptr : value->second;
}

void ResolveMemos::put(const MemoKeyPtr& key, const AbstractConfigValuePtr& value) {
    _memos[key] = value;
}

}
