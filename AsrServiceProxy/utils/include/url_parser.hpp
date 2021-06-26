#ifndef UTILS_URL_PARSER_HPP
#define UTILS_URL_PARSER_HPP

#include <string>

class UrlParser {
public:
    UrlParser(const std::string& url);
    
public:
    std::string& get_proto();
    std::string& get_host();
    std::string& get_uri();
    int get_port();
    
private:
    void parse(const std::string& url);
private:
    std::string _proto;
    std::string _host;
    int _port;
    std::string _uri;
};

#endif // UTILS_URL_PARSER_HPP