#include <stdio.h>
#include <string.h>
#include "include/http.hpp"
#include "include/utils.hpp"
#include "include/aip_log.hpp"

int HttpClient::_s_curl_init = 0;

std::string HttpRequest::method_str() const {
    std::string str;

    switch (_method) {
    case METHOD_GET:
        str = "GET";
        break;

    case METHOD_POST:
        str = "POST";
        break;

    case METHOD_PUT:
        str = "PUT";
        break;

    case METHOD_PATCH:
        str = "PATCH";
        break;

    case METHOD_DELETE:
        str = "DELETE";
        break;

    case METHOD_HEAD:
        str = "HEAD";
        break;

    default:
        str = "UNKNOWN";
    }

    return str;
}

HttpMethod HttpRequest::str_to_method(const std::string& str) {
    if (str == "GET") {
        return METHOD_GET;
    } else if (str == "POST") {
        return METHOD_POST;
    } else if (str == "PUT") {
        return METHOD_PUT;
    } else if (str == "PATCH") {
        return METHOD_PATCH;
    } else if (str == "DELETE") {
        return METHOD_DELETE;
    } else if (str == "HEAD") {
        return METHOD_HEAD;
    } else {
        return UNKNOWN;
    }
}

void HttpClient::global_init() {
    if (_s_curl_init == 0) {
        curl_global_init(CURL_GLOBAL_ALL);
        _s_curl_init = 1;
    }
}

void HttpClient::global_cleanup() {
    if (_s_curl_init == 1) {
        curl_global_cleanup();
        _s_curl_init = 0;
    }
}

int HttpClient::talk(const HttpRequest& request, HttpResponse* response) {
    std::string url = request.host() + ":" + to_string(request.port());
    url.append(request.uri());

    if (!request.params().empty()) {
        url.append("?");
        url.append(join_params(request.params()));
    }

    CURL* curl = curl_easy_init();

    if (NULL == curl) {
        AIP_LOG_WARNING("curl init failed");
        return GENERAL_ERROR;
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, on_debug);

    char curl_errbuf[CURL_ERROR_SIZE];
    memset(curl_errbuf, 0, CURL_ERROR_SIZE);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);

    switch (request.method()) {
    case METHOD_POST:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        break;

    case METHOD_PUT:
        curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        break;

    case METHOD_PATCH:
        // TODO: complete it
        AIP_LOG_FATAL("PATCH not supported");
        return GENERAL_ERROR;

    case METHOD_DELETE:
        // TODO: complete it
        curl_easy_setopt(curl, CURLOPT_PUT, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;

    case METHOD_HEAD:
        // TODO: complete it
        AIP_LOG_FATAL("HEAD not supported");
        return GENERAL_ERROR;

    case METHOD_GET:
        break;

    default:
        // TODO: complete it
        AIP_LOG_FATAL("method not supported");
        return GENERAL_ERROR;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // build a stream for curl to read requst body
    // 'cause maybe body cannot be sent in one write_body callback
    std::istringstream body_stream(request.body());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, write_body);
    curl_easy_setopt(curl, CURLOPT_READDATA, static_cast<void*>(&body_stream));

    curl_slist* headers = NULL;

    for (HttpHeaders::const_iterator it = request.headers().begin();
            it != request.headers().end(); ++it) {
        std::string str_header = it->first + ":" + it->second;
        headers = curl_slist_append(headers, str_header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, read_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(response->mutable_body()));
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, read_headers);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, static_cast<void*>(response->mutable_str_headers()));
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);

    CURLcode curl_ret = CURLE_OK;

    for (int i = 0; i < 3; ++i) { // perform 3 retries
        curl_ret = curl_easy_perform(curl);

        if (curl_ret == CURLE_OK) {
            response->set_headers(build_headers_from_str(response->str_headers()));
            response->set_status(get_statusline_from_str(response->str_headers()));
            break;
        } else {
            AIP_LOG_WARNING("curl talk failed: %s, retry", curl_errbuf);
        }
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (curl_ret == CURLE_OK) {
        return http_code;
    } else {
        AIP_LOG_WARNING("http talk failed: %s", curl_errbuf);
        if (curl_ret == CURLE_COULDNT_RESOLVE_HOST) {
            return COULDNT_RESOLVE_HOST;
        } else if (curl_ret == CURLE_COULDNT_CONNECT) {
            return COULDNT_CONNECT;
        } else if (curl_ret == CURLE_SEND_ERROR) {
            return SEND_ERROR;
        } else if (curl_ret == CURLE_RECV_ERROR) {
            return RECV_ERROR;
        } else {
            return GENERAL_ERROR;
        }
    }

    // should not reach here
    return GENERAL_ERROR;
}

size_t HttpClient::write_body(void* buffer, size_t size, size_t nmemb, void* istream) {
    std::istringstream* req_stream = reinterpret_cast<std::istringstream*>(istream);
    req_stream->read(reinterpret_cast<char*>(buffer), size * nmemb);
    return req_stream->gcount();
}

size_t HttpClient::read_body(void* buffer, size_t size, size_t nmemb, void* resp) {
    std::string* response = reinterpret_cast<std::string*>(resp);
    response->append(reinterpret_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

size_t HttpClient::read_headers(void* buffer, size_t size, size_t nmemb, void* head) {
    std::string* headers = reinterpret_cast<std::string*>(head);
    headers->append(reinterpret_cast<char*>(buffer), size * nmemb);
    return size * nmemb;
}

int HttpClient::on_debug(CURL*, curl_infotype type, char* data, size_t size, void*) {
    if (type == CURLINFO_TEXT) {
        //        AIP_LOG_DEBUG("[TEXT]%s", data);
    } else if (type == CURLINFO_HEADER_IN) {
        //        AIP_LOG_DEBUG("[HEADER_IN]%s", data);
    } else if (type == CURLINFO_HEADER_OUT) {
        //        AIP_LOG_DEBUG("[HEADER_OUT]%s", data);
    } else if (type == CURLINFO_DATA_IN) {
        //        AIP_LOG_DEBUG("[DATA_IN]%s", data);
    } else if (type == CURLINFO_DATA_OUT) {
        //        AIP_LOG_DEBUG("[DATA_OUT]%s", data);
    }

    return 0;
}

std::string HttpClient::join_params(const HttpParams& params) {
    std::string joined;

    for (HttpParams::const_iterator it = params.begin(); it != params.end(); ++it) {
        if (!joined.empty()) {
            joined.append("&");
        }

        joined.append(it->first);

        if (!StringUtil::is_empty(it->second)) {
            joined.append("=");
            joined.append(StringUtil::uri_encode(it->second));
        }
    }

    return joined;
}

HttpHeaders HttpClient::build_headers_from_str(const std::string& str) {
    HttpHeaders headers;
    int ret = StringUtil::split(StringUtil::space_trim(str), '\n', ':', &headers, true);

    if (ret < 0) {
        AIP_LOG_WARNING("invalid headers string: %s", str.c_str());
        return HttpHeaders();
    }

    // TODO: don't know why HTTP/1.1 is read in headers, so delete it
    HttpHeaders::iterator it = headers.begin();

    while (it != headers.end()) {
        std::string name = it->first;

        if (name.find("HTTP/1.1") != std::string::npos) {
            HttpHeaders::iterator to_erase = it;
            ++it;
            headers.erase(to_erase);
        } else {
            ++it;
        }
    }

    return headers;
}


// TODO: HTTP/1.1 is in first line of headers str, extract it
std::string HttpClient::get_statusline_from_str(const std::string& str) {
    size_t startpos = str.find("HTTP");
    size_t endpos = str.find("\r\n");
    return StringUtil::space_trim(str.substr(startpos, endpos));
}

int HttpsClient::talk(const HttpRequest& request, HttpResponse* response) {
    std::string url = "https://" + request.host() + ":" + to_string(request.port());
    url.append(request.uri());

    if (!request.params().empty()) {
        url.append("?");
        url.append(join_params(request.params()));
    }

    CURL* curl = curl_easy_init();

    if (NULL == curl) {
        AIP_LOG_WARNING("curl init failed");
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, on_debug);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    char curl_errbuf[CURL_ERROR_SIZE];
    memset(curl_errbuf, 0, CURL_ERROR_SIZE);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);

    switch (request.method()) {
    case METHOD_POST:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        break;

    case METHOD_PUT:
        curl_easy_setopt(curl, CURLOPT_PUT, 1L);
        break;

    case METHOD_PATCH:
        // TODO: complete it
        AIP_LOG_FATAL("PATCH not supported");
        return -1;

    case METHOD_DELETE:
        // TODO: complete it
        curl_easy_setopt(curl, CURLOPT_PUT, 0L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;

    case METHOD_HEAD:
        // TODO: complete it
        AIP_LOG_FATAL("HEAD not supported");
        return -1;

    case METHOD_GET:
        break;

    default:
        // TODO: complete it
        AIP_LOG_FATAL("method not supported");
        return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // build a stream for curl to read requst body
    // 'cause maybe body cannot be sent in one write_body callback
    std::istringstream body_stream(request.body());
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, write_body);
    curl_easy_setopt(curl, CURLOPT_READDATA, static_cast<void*>(&body_stream));

    curl_slist* headers = NULL;

    for (HttpHeaders::const_iterator it = request.headers().begin();
            it != request.headers().end(); ++it) {
        std::string str_header = it->first + ":" + it->second;
        headers = curl_slist_append(headers, str_header.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, read_body);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(response->mutable_body()));
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, read_headers);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, static_cast<void*>(response->mutable_str_headers()));
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);

    CURLcode curl_ret = curl_easy_perform(curl);

    if (curl_ret == CURLE_OK) {
        response->set_headers(build_headers_from_str(response->str_headers()));
        response->set_status(get_statusline_from_str(response->str_headers()));
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (curl_ret == CURLE_OK) {
        return http_code;
    } else {
        AIP_LOG_WARNING("curl talk failed: %s", curl_errbuf);
        return -1;
    }
}
