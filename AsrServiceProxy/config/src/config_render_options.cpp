
#include "config_render_options.hpp"

namespace config {

ConfigRenderOptions::ConfigRenderOptions(bool origin_comments, bool comments,
        bool formatted, bool json) :
    origin_comments(origin_comments),
    comments(comments),
    formatted(formatted),
    json(json) {
}

ConfigRenderOptionsPtr ConfigRenderOptions::defaults() {
    return make_instance(true, true, true, true);
}

ConfigRenderOptionsPtr ConfigRenderOptions::concise() {
    return make_instance(false, false, false, true);
}

ConfigRenderOptionsPtr ConfigRenderOptions::set_comments(bool value) {
    if (value == comments) {
        return shared_from_this();
    } else {
        return make_instance(origin_comments, value, formatted, json);
    }
}

bool ConfigRenderOptions::get_comments() {
    return comments;
}

ConfigRenderOptionsPtr ConfigRenderOptions::set_origin_comments(bool value) {
    if (value == origin_comments) {
        return shared_from_this();
    } else {
        return make_instance(value, comments, formatted, json);
    }
}

bool ConfigRenderOptions::get_origin_comments() {
    return origin_comments;
}

ConfigRenderOptionsPtr ConfigRenderOptions::set_formatted(bool value) {
    if (value == formatted) {
        return shared_from_this();
    } else {
        return make_instance(origin_comments, comments, value, json);
    }
}

bool ConfigRenderOptions::get_formatted() {
    return formatted;
}

ConfigRenderOptionsPtr ConfigRenderOptions::set_json(bool value) {
    if (value == json) {
        return shared_from_this();
    } else {
        return make_instance(origin_comments, comments, formatted, value);
    }
}

bool ConfigRenderOptions::get_json() {
    return json;
}

std::string ConfigRenderOptions::to_string() {
    std::string s = get_class_name() + "(";

    if (origin_comments) {
        s += "origin_comments,";
    }

    if (comments) {
        s += "comments,";
    }

    if (formatted) {
        s += "formatted,";
    }

    if (json) {
        s += "json,";
    }

    if (boost::ends_with(s, ",")) {
        s.resize(s.length() - 1);
    }

    s += ")";
    return s;
}

}
