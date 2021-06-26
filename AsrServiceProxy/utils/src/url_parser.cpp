#include "include/url_parser.hpp"
#include "include/utils.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UrlParser::UrlParser(const std::string& url)
: _proto("http"), _host(""), _uri("/"), _port(80) {
    parse(url);
}
    
std::string& UrlParser::get_proto() {
    return this->_proto;
}

std::string& UrlParser::get_host() {
    return this->_host;
}

std::string& UrlParser::get_uri() {
    return this->_uri;
}

int UrlParser::get_port() {
    return this->_port;
}

void UrlParser::parse(const std::string& url) {
    static const char* s_pro_spl = ":/\\";
    
    std::string clean_url = StringUtil::trim(url);
    const char* c_str = clean_url.c_str();

    int start = 0;
    
    // find protocol
    if (strncasecmp(c_str, "https", 5) == 0) {
        _proto = "https";
        c_str += 5;
    } else if (strncasecmp(c_str, "http", 4) == 0) {
        _proto = "http";
        c_str += 4;
    }
    
    while (*c_str && strchr(s_pro_spl, *c_str)) {
        c_str++;
    }
    
    _host.clear();
    
    while (*c_str && !strchr(s_pro_spl, *c_str)) {
        _host += *c_str;
        c_str++;
    }
    _host = StringUtil::trim(_host);
    
    if (*c_str == ':') {
        c_str++;
        std::string str_port;
                
        while (*c_str && !strchr(s_pro_spl, *c_str)) {
            if (isdigit(*c_str)) {
                str_port += *c_str;
            }
            c_str++;
        }
        
        if (str_port.size() > 0) {
            _port = atoi(str_port.c_str());
        }
    }
    
    _uri = c_str;
    _uri = StringUtil::trim(_uri);
}