
#include "detail/path_builder.hpp"
#include "detail/path.hpp"
#include "config_exception.hpp"

namespace config {

void PathBuilder::check_can_append() {
    if (_result) {
        throw ConfigExceptionBugOrBroken("_adding to PathBuilder after getting result");
    }
}

void PathBuilder::append_key(const std::string& key) {
    check_can_append();

    _keys.push_front(key);
}

void PathBuilder::append_path(const PathPtr& path) {
    check_can_append();

    std::string first = path->first();
    auto remainder = path->remainder();

    while (true) {
        _keys.push_front(first);

        if (remainder) {
            first = remainder->first();
            remainder = remainder->remainder();
        } else {
            break;
        }
    }
}

PathPtr PathBuilder::result() {
    // note: if keys is empty, we want to return null, which is a valid empty path
    if (!_result) {
        PathPtr remainder;

        while (!_keys.empty()) {
            std::string key = _keys.front();
            _keys.pop_front();
            remainder = _path::make_instance(key, remainder);
        }

        _result = remainder;
    }

    return _result;
}

}
