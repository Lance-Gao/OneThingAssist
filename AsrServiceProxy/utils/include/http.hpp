#ifndef UTILS_HTTP_HPP
#define UTILS_HTTP_HPP

#include "aip_common.hpp"
#include "utils.hpp"

class HttpRequest;
class HttpResponse;
typedef std::map<std::string, std::string> HttpHeaders;
typedef std::map<std::string, std::string> HttpParams;

// IMPORTANT: call global_init/global_clean at startup/exit is required
//            otherwise there will be thread-safe risks
// example usage:
//     HttpRequest request = build_your_request();
//     HttpResponst response;
//     int ret = HttpClient::talk(request, response);
//     if (ret != 200) { // suppose you expect "200 OK" status
//         handle_failure();
//     } else {
//         process_response(response);
//     }

class HttpClient {
public:
    const static int GENERAL_ERROR = -1;
    const static int COULDNT_RESOLVE_HOST = -2;
    const static int COULDNT_CONNECT = -3;
    const static int SEND_ERROR = -4;
    const static int RECV_ERROR = -5;

    static void global_init();
    static void global_cleanup();
    // return HTTP status code if success; <0 if fail
    static int talk(const HttpRequest& request, HttpResponse* response);
    static std::string join_params(const HttpParams& params);
    static HttpHeaders build_headers_from_str(const std::string& str);
    static std::string get_statusline_from_str(const std::string& str);
    static size_t read_body(void* buffer, size_t size, size_t nmemb, void* resp);
    static size_t write_body(void* buffer, size_t size, size_t nmemb, void* req);
    static size_t read_headers(void* buffer, size_t size, size_t nmemb, void* resp);
    static int on_debug(CURL*, curl_infotype type, char* data, size_t size, void*);
private:
    static int _s_curl_init;
};

class HttpsClient : public HttpClient {
public:
    static int talk(const HttpRequest& request, HttpResponse* response);
};

class HttpMessage {
public:
    HttpMessage() {}
    virtual ~HttpMessage() {}

    const HttpHeaders& headers() const {
        return _headers;
    }
    void set_headers(const HttpHeaders& headers) {
        _headers = headers;
    }
    void add_header(const std::string& name, const std::string& value) {
        _headers[name] = value;
    }

    std::string* mutable_body() {
        return &_body;
    }
    const std::string& body() const {
        return _body;
    }
    void set_body(const std::string& body) {
        _body = body;
    }

private:
    HttpHeaders _headers;
    std::string _body;
};

enum HttpMethod {
    METHOD_GET = 0,
    METHOD_POST,
    METHOD_PUT,
    METHOD_DELETE,
    METHOD_PATCH,
    METHOD_HEAD,
    UNKNOWN
};

class HttpRequest : public HttpMessage {
public:
    HttpRequest() : _host("localhost"), _port(80), _uri("/"), _method(METHOD_GET) {}
    HttpRequest(const std::string& host, int port, 
                const std::string uri, HttpMethod method = METHOD_GET) :
        _host(host), _port(port), _uri(uri), _method(method) {}
    virtual ~HttpRequest() {}

    const std::string& host() const {
        return _host;
    }
    void set_host(const std::string& host) {
        _host = host;
    }

    int port() const {
        return _port;
    }
    void set_port(int port) {
        _port = port;
    }

    HttpMethod method() const {
        return _method;
    }
    void set_method(HttpMethod method) {
        _method = method;
    }
    std::string method_str() const;
    static HttpMethod str_to_method(const std::string& str);

    const std::string& uri() const {
        return _uri;
    }
    void set_uri(const std::string& uri) {
        _uri = uri;
    }

    const HttpParams& params() const {
        return _params;
    }
    void add_param(const std::string& name, const std::string& value) {
        _params[name] = value;
    }
    void set_params(const HttpParams& params) {
        _params = params;
    }
    void set_content_type_json() {
        add_header("Content-Type", "application/json; charset=utf-8");
    }
    void set_accept_json() {
        add_header("Accept", "application/json");
    }
    void set_accept_encoding() {
        add_header("Accept-Encoding", "gzip,deflate");
    }
    void set_content_length(int size) {
        add_header("Content-Length", to_string(size));
    }
    void set_body(const std::string& body) {
        HttpMessage::set_body(body);
        set_content_length(body.size());
    }

private:
    std::string _host;
    int _port;
    std::string _uri;
    HttpMethod _method;
    HttpParams _params;
};

class HttpResponse : public HttpMessage {
public:
    HttpResponse() : _code(0) {}
    virtual ~HttpResponse() {}

    int code() const {
        return _code;
    }
    void set_code(int code) {
        _code = code;
    }

    const std::string& status() const {
        return _status;
    }
    void set_status(const std::string& status) {
        _status = status;
    }

    const std::string& str_headers() const {
        return _str_headers;
    }
    void set_str_headers(const std::string& str_headers) {
        _str_headers = str_headers;
    }
    std::string* mutable_str_headers() {
        return &_str_headers;
    }

private:
    int _code;
    std::string _status;
    std::string _str_headers;
};

#endif  // UTILS_HTTP_HPP