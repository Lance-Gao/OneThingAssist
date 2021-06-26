
#include "detail/path.hpp"
#include "detail/path_builder.hpp"
#include "detail/config_impl_util.hpp"
#include "detail/parser.hpp"
#include "detail/variant_utils.hpp"
#include "config_exception.hpp"

namespace config {

Path::Path(const std::string& first, const PathPtr& remainder) :
    first_(first),
    remainder_(remainder) {
}

Path::Path(const VectorString& elements) {
    if (elements.empty()) {
        throw ConfigExceptionBugOrBroken("empty path");
    }

    first_ = elements[0];

    if (elements.size() > 1) {
        auto pb = PathBuilder::make_instance();

        for (uint32_t i = 1; i < elements.size(); ++i) {
            pb->append_key(elements[i]);
        }

        remainder_ = pb->result();
    }
}

Path::Path(const VectorPath& paths_to_concat) {
    if (paths_to_concat.empty()) {
        throw ConfigExceptionBugOrBroken("empty path");
    }

    auto i = paths_to_concat.begin();
    auto first_path = *(i++);
    first_ = first_path->first_;

    auto pb = PathBuilder::make_instance();

    if (first_path->remainder_) {
        pb->append_path(first_path->remainder_);
    }

    while (i != paths_to_concat.end()) {
        pb->append_path(*(i++));
    }

    remainder_ = pb->result();
}

std::string Path::first() {
    return first_;
}

PathPtr Path::remainder() {
    return remainder_;
}

PathPtr Path::parent() {
    if (!remainder_) {
        return nullptr;
    }

    auto pb = PathBuilder::make_instance();
    auto p = shared_from_this();

    while (p->remainder_) {
        pb->append_key(p->first_);
        p = p->remainder_;
    }

    return pb->result();
}

std::string Path::last() {
    auto p = shared_from_this();

    while (p->remainder_) {
        p = p->remainder_;
    }

    return p->first_;
}

PathPtr Path::prepend(const PathPtr& to_prepend) {
    auto pb = PathBuilder::make_instance();
    pb->append_path(to_prepend);
    pb->append_path(shared_from_this());
    return pb->result();
}

uint32_t Path::length() {
    uint32_t count = 1;
    auto p = remainder_;

    while (p) {
        count += 1;
        p = p->remainder_;
    }

    return count;
}

PathPtr Path::sub_path(uint32_t remove_from_front) {
    int32_t count = remove_from_front;
    auto p = shared_from_this();

    while (p && count > 0) {
        count -= 1;
        p = p->remainder_;
    }

    return p;
}

PathPtr Path::sub_path(uint32_t first_index, uint32_t last_index) {
    if (last_index < first_index) {
        throw ConfigExceptionBugOrBroken("bad call to sub_path");
    }

    auto from = sub_path(first_index);
    auto pb = PathBuilder::make_instance();
    uint32_t count = last_index - first_index;

    while (count > 0) {
        count -= 1;
        pb->append_key(from->first());
        from = from->remainder();

        if (!from) {
            throw ConfigExceptionBugOrBroken("sub_path last_index out of range " +
                                                  boost::lexical_cast<std::string>(last_index));
        }
    }

    return pb->result();
}

bool Path::equals(const ConfigVariant& other) {
    if (instanceof<Path>(other)) {
        auto that = static_get<Path>(other);
        return this->first_ == that->first_
               && ConfigImplUtil::equals_handling_null(this->remainder_, that->remainder_);
    } else {
        return false;
    }
}

uint32_t Path::hash_code() {
    return 41 * (41 + std::hash<std::string>()(first_)) + (!remainder_ ? 0 : remainder_->hash_code());
}

bool Path::has_funky_chars(const std::string& s) {
    uint32_t length = s.length();

    if (length == 0) {
        return false;
    }

    // if the path starts with something that could be a number,
    // we need to quote it because the number could be invalid,
    // for example it could be a hyphen with no digit afterward
    // or the exponent "e" notation could be mangled.
    char first = s[0];

    if (!std::isalpha(first)) {
        return true;
    }

    for (uint32_t i = 1; i < length; ++i) {
        char c = s[i];

        if (std::isalnum(c) || c == '-' || c == '_') {
            continue;
        } else {
            return true;
        }
    }

    return false;
}

void Path::append_to_stream(std::string& s) {
    if (has_funky_chars(first_) || first_.empty()) {
        s += ConfigImplUtil::render_json_string(first_);
    } else {
        s += first_;
    }

    if (remainder_) {
        s += ".";
        remainder_->append_to_stream(s);
    }
}

std::string Path::to_string() {
    std::string s = "Path(";
    append_to_stream(s);
    s += ")";
    return s;
}

std::string Path::render() {
    std::string s;
    append_to_stream(s);
    return s;
}

PathPtr Path::new_key(const std::string& key) {
    return make_instance(key, nullptr);
}

PathPtr Path::new_path(const std::string& path) {
    return Parser::parse_path(path);
}

}
