
#include "detail/simple_config_origin.hpp"
#include "detail/origin_type.hpp"
#include "detail/abstract_config_value.hpp"
#include "config_exception.hpp"

namespace config {

SimpleConfigOrigin::SimpleConfigOrigin(const std::string& description,
        int32_t line_number,
        int32_t end_line_number,
        OriginType origin_type,
        const VectorString& comments_or_null) :
    description_(description),
    line_number_(line_number),
    end_line_number(end_line_number),
    origin_type(origin_type),
    comments_or_null(comments_or_null) {
    if (description.empty()) {
        throw ConfigExceptionBugOrBroken("description may not be empty");
    }
}

SimpleConfigOriginPtr SimpleConfigOrigin::new_simple(const std::string& description) {
    return SimpleConfigOrigin::make_instance(description, -1, -1, OriginType::GENERIC,
            VectorString());
}

SimpleConfigOriginPtr SimpleConfigOrigin::new_file(const std::string& filename) {
    return SimpleConfigOrigin::make_instance(filename, -1, -1, OriginType::FILE, VectorString());
}

SimpleConfigOriginPtr SimpleConfigOrigin::set_line_number(int32_t line_number) {
    if (line_number == this->line_number_ && line_number == this->end_line_number) {
        return shared_from_this();
    } else {
        return SimpleConfigOrigin::make_instance(this->description_, line_number, line_number,
                this->origin_type, this->comments_or_null);
    }
}

SimpleConfigOriginPtr SimpleConfigOrigin::set_comments(const VectorString& comments) {
    if (comments.size() == comments_or_null.size()
            && std::equal(comments.begin(), comments.end(), comments_or_null.begin())) {
        return shared_from_this();
    } else {
        return SimpleConfigOrigin::make_instance(description_, line_number_, end_line_number,
                origin_type,
                comments);
    }
}

std::string SimpleConfigOrigin::description() {
    if (line_number_ < 0) {
        return description_;
    } else if (end_line_number == line_number_) {
        return description_ + ": " + boost::lexical_cast<std::string>(line_number_);
    } else {
        return description_ + ": " + boost::lexical_cast<std::string>(line_number_) + "-" +
               boost::lexical_cast<std::string>(end_line_number);
    }
}

bool SimpleConfigOrigin::equals(const ConfigVariant& other) {
    if (instanceof<SimpleConfigOrigin>(other)) {
        auto other_origin = dynamic_get<SimpleConfigOrigin>(other);
        return description_ == other_origin->description_ &&
               line_number_ == other_origin->line_number_ &&
               end_line_number == other_origin->end_line_number &&
               origin_type == other_origin->origin_type;
    } else {
        return false;
    }
}

uint32_t SimpleConfigOrigin::hash_code() {
    uint32_t hash = 41 * (41 + std::hash<std::string>()(description_));
    hash = 41 * (hash + line_number_);
    hash = 41 * (hash + end_line_number);
    hash = 41 * (hash + std::hash<uint32_t>()(static_cast<uint32_t>(origin_type)));
    return hash;
}

std::string SimpleConfigOrigin::to_string() {
    return "ConfigOrigin(" + description_ + ")";
}

std::string SimpleConfigOrigin::filename() {
    if (origin_type == OriginType::FILE) {
        return description_;
    } else {
        return "";
    }
}

int32_t SimpleConfigOrigin::line_number() {
    return line_number_;
}

VectorString SimpleConfigOrigin::comments() {
    return comments_or_null;
}

SimpleConfigOriginPtr SimpleConfigOrigin::merge_two(const SimpleConfigOriginPtr& a,
        const SimpleConfigOriginPtr& b) {
    static const std::string merge_of_prefix = "merge of ";

    std::string merged_desc;
    int32_t merged_start_line = 0;
    int32_t merged_end_line = 0;
    VectorString merged_comments;

    OriginType merged_type;

    if (a->origin_type == b->origin_type) {
        merged_type = a->origin_type;
    } else {
        merged_type = OriginType::GENERIC;
    }

    // first use the "description" field which has no line numbers
    // cluttering it.
    std::string a_desc = a->description_;
    std::string b_desc = b->description_;

    if (boost::starts_with(a_desc, merge_of_prefix)) {
        a_desc = a_desc.substr(merge_of_prefix.length());
    }

    if (boost::starts_with(b_desc, merge_of_prefix)) {
        b_desc = b_desc.substr(merge_of_prefix.length());
    }

    if (a_desc == b_desc) {
        merged_desc = a_desc;

        if (a->line_number_ < 0) {
            merged_start_line = b->line_number_;
        } else if (b->line_number_ < 0) {
            merged_start_line = a->line_number_;
        } else {
            merged_start_line = std::min(a->line_number_, b->line_number_);
        }

        merged_end_line = std::max(a->end_line_number, b->end_line_number);
    } else {
        // this whole merge song-and-dance was intended to avoid this case
        // whenever possible, but we've lost. _now we have to lose some
        // structured information and cram into a string.

        // description() method includes line numbers, so use it instead
        // of description field.
        std::string a_full = a->description();
        std::string b_full = b->description();

        if (boost::starts_with(a_full, merge_of_prefix)) {
            a_full = a_full.substr(merge_of_prefix.length());
        }

        if (boost::starts_with(b_full, merge_of_prefix)) {
            b_full = b_full.substr(merge_of_prefix.length());
        }

        merged_desc = merge_of_prefix + a_full + "," + b_full;

        merged_start_line = -1;
        merged_end_line = -1;
    }

    if (a->comments_or_null.size() == b->comments_or_null.size()
            && std::equal(a->comments_or_null.begin(), a->comments_or_null.end(),
                          b->comments_or_null.begin())) {
        merged_comments = a->comments_or_null;
    } else {
        merged_comments.insert(merged_comments.end(), a->comments_or_null.begin(),
                               a->comments_or_null.end());
        merged_comments.insert(merged_comments.end(), b->comments_or_null.begin(),
                               b->comments_or_null.end());
    }

    return SimpleConfigOrigin::make_instance(merged_desc, merged_start_line, merged_end_line,
            merged_type,
            merged_comments);
}

uint32_t SimpleConfigOrigin::similarity(const SimpleConfigOriginPtr& a,
        const SimpleConfigOriginPtr& b) {
    uint32_t count = 0;

    if (a->origin_type == b->origin_type) {
        count += 1;
    }

    if (a->description_ == b->description_) {
        count += 1;

        // only count these if the description field (which is the file
        // or resource name) also matches.
        if (a->line_number_ == b->line_number_) {
            count += 1;
        }

        if (a->end_line_number == b->end_line_number) {
            count += 1;
        }
    }

    return count;
}

SimpleConfigOriginPtr SimpleConfigOrigin::merge_three(const SimpleConfigOriginPtr& a,
        const SimpleConfigOriginPtr& b, const SimpleConfigOriginPtr& c) {
    if (similarity(a, b) >= similarity(b, c)) {
        return merge_two(merge_two(a, b), c);
    } else {
        return merge_two(a, merge_two(b, c));
    }
}

ConfigOriginPtr SimpleConfigOrigin::merge_origins(const ConfigOriginPtr& a,
        const ConfigOriginPtr& b) {
    return merge_two(std::dynamic_pointer_cast<SimpleConfigOrigin>(a),
                     std::dynamic_pointer_cast<SimpleConfigOrigin>(b));
}

ConfigOriginPtr SimpleConfigOrigin::merge_origins(const VectorAbstractConfigValue&
        stack) {
    VectorConfigOrigin origins;
    origins.reserve(stack.size());

    for (auto& v : stack) {
        origins.push_back(v->origin());
    }

    return merge_origins(origins);
}

ConfigOriginPtr SimpleConfigOrigin::merge_origins(const VectorConfigOrigin& stack) {
    if (stack.empty()) {
        throw ConfigExceptionBugOrBroken("can't merge empty list of origins");
    } else if (stack.size() == 1) {
        return stack.front();
    } else if (stack.size() == 2) {
        return merge_two(std::dynamic_pointer_cast<SimpleConfigOrigin>(stack[0]),
                         std::dynamic_pointer_cast<SimpleConfigOrigin>(stack[1]));
    } else {
        VectorConfigOrigin remaining(stack);

        while (remaining.size() > 2) {
            auto c = std::dynamic_pointer_cast<SimpleConfigOrigin>(remaining.back());
            remaining.pop_back();
            auto b = std::dynamic_pointer_cast<SimpleConfigOrigin>(remaining.back());
            remaining.pop_back();
            auto a = std::dynamic_pointer_cast<SimpleConfigOrigin>(remaining.back());
            remaining.pop_back();

            auto merged = merge_three(a, b, c);
            remaining.push_back(merged);
        }

        // should be down to either 1 or 2
        return merge_origins(remaining);
    }
}

}
