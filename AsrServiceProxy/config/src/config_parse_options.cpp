
#include "config_parse_options.hpp"
#include "config_syntax.hpp"
#include "config_includer.hpp"

namespace config {

ConfigParseOptions::ConfigParseOptions(ConfigSyntax syntax,
        const std::string& origin_description,
        bool allow_missing,
        const ConfigIncluderPtr& includer) :
    _syntax(syntax),
    _origin_description(origin_description),
    _allow_missing(allow_missing),
    _includer(includer) {
}

ConfigParseOptionsPtr ConfigParseOptions::defaults() {
    return make_instance(ConfigSyntax::NONE, "", true, nullptr);
}

ConfigParseOptionsPtr ConfigParseOptions::set_syntax(ConfigSyntax syntax) {
    if (this->_syntax == syntax) {
        return shared_from_this();
    } else {
        return make_instance(syntax, _origin_description, this->_allow_missing, this->_includer);
    }
}

ConfigSyntax ConfigParseOptions::get_syntax() {
    return _syntax;
}

ConfigParseOptionsPtr ConfigParseOptions::set_origin_description(const std::string&
        origin_description) {
    if (this->_origin_description == origin_description) {
        return shared_from_this();
    } else {
        return make_instance(this->_syntax, origin_description,
            this->_allow_missing, this->_includer);
    }
}

std::string ConfigParseOptions::get_origin_description() {
    return _origin_description;
}

ConfigParseOptionsPtr ConfigParseOptions::with_fallback_origin_description(
    const std::string& origin_description) {
    if (this->_origin_description.empty()) {
        return set_origin_description(origin_description);
    } else {
        return shared_from_this();
    }
}

ConfigParseOptionsPtr ConfigParseOptions::set_allow_missing(bool allow_missing) {
    if (this->_allow_missing == allow_missing) {
        return shared_from_this();
    } else {
        return make_instance(this->_syntax, this->_origin_description,
            allow_missing, this->_includer);
    }
}

bool ConfigParseOptions::get_allow_missing() {
    return _allow_missing;
}

ConfigParseOptionsPtr ConfigParseOptions::set_includer(const ConfigIncluderPtr&
        includer) {
    if (this->_includer == includer) {
        return shared_from_this();
    } else {
        return make_instance(this->_syntax, this->_origin_description,
            this->_allow_missing, includer);
    }
}

ConfigParseOptionsPtr ConfigParseOptions::prepend_includer(const ConfigIncluderPtr&
        includer) {
    if (this->_includer == includer) {
        return shared_from_this();
    } else if (this->_includer) {
        return set_includer(includer->with_fallback(this->_includer));
    } else {
        return set_includer(includer);
    }
}

ConfigParseOptionsPtr ConfigParseOptions::append_includer(const ConfigIncluderPtr&
        includer) {
    if (this->_includer == includer) {
        return shared_from_this();
    } else if (this->_includer) {
        return set_includer(this->_includer->with_fallback(includer));
    } else {
        return set_includer(includer);
    }
}

ConfigIncluderPtr ConfigParseOptions::get_includer() {
    return _includer;
}

}
