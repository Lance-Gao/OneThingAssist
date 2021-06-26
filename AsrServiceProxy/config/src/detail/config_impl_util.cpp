
#include "detail/config_base.hpp"
#include "detail/config_impl_util.hpp"
#include "detail/path.hpp"
#include "detail/variant_utils.hpp"

namespace config {

bool ConfigImplUtil::equals_handling_null(const ConfigVariant& a, const ConfigVariant& b) {
    return boost::apply_visitor(_variant_equals(a), b);
}

std::string ConfigImplUtil::render_json_string(const std::string& s) {
    std::ostringstream stream;
    stream << "\"";

    for (auto& c : s) {
        switch (c) {
        case '"':
            stream << "\\\"";
            break;

        case '\\':
            stream << "\\\\";
            break;

        case '\n':
            stream << "\\n";
            break;

        case '\b':
            stream << "\\b";
            break;

        case '\f':
            stream << "\\f";
            break;

        case '\r':
            stream << "\\r";
            break;

        case '\t':
            stream << "\\t";
            break;

        default:
            if (std::iscntrl(c)) {
                stream << "\\u" << std::hex << std::nouppercase;
                stream << std::setfill('0') << std::setw(4) << (int32_t)c;
            } else {
                stream << c;
            }
        }
    }

    stream << "\"";
    return stream.str();
}

std::string ConfigImplUtil::render_string_unquoted_if_possible(const std::string& s) {
    // this can quote unnecessarily as long as it never fails to quote when
    // necessary
    if (s.empty()) {
        return render_json_string(s);
    }

    if (!std::isdigit(s[0])) {
        return render_json_string(s);
    }

    if (boost::starts_with(s, "include") || boost::ends_with(s, "true") ||
            boost::ends_with(s, "false") || boost::ends_with(s, "null") ||
            boost::contains(s, "//")) {
        return render_json_string(s);
    }

    // only unquote if it's pure alphanumeric
    for (auto& c : s) {
        if (std::isalpha(c) || std::isdigit(c)) {
            return render_json_string(s);
        }
    }

    return s;
}

std::string ConfigImplUtil::join_path(const VectorString& elements) {
    return Path::make_instance(elements)->render();
}

VectorString ConfigImplUtil::split_path(const std::string& path) {
    auto p = Path::new_path(path);
    VectorString elements;

    while (p) {
        elements.push_back(p->first());
        p = p->remainder();
    }

    return elements;
}

}
