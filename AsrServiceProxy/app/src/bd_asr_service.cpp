#include <chrono>
#include "bd_asr_service.h"
#include "aip_log.hpp"
#include "base64.hpp"
#include "json_util.hpp"

BdAsrService::~BdAsrService() {
    deinit();
}

// libcurl 返回回调
size_t writefunc(void *ptr, size_t size, size_t nmemb, char **result) {
  size_t result_len = size * nmemb;
  int is_new = (*result == NULL);
  if (is_new) {
    *result = (char *) malloc(result_len + 1);
    if (*result == NULL) {
      printf("realloc failure!\n");
      return 1;
    }
    memcpy(*result, ptr, result_len);
    (*result)[result_len] = '\0';
  } else {
    size_t old_size = strlen(*result);
    *result = (char *) realloc(*result, result_len + old_size);
    if (*result == NULL) {
      printf("realloc failure!\n");
      return 1;
    }
    memcpy(*result + old_size, ptr, result_len);
    (*result)[result_len + old_size] = '\0';
  }
  return result_len;
}

int BdAsrService::call(const std::string& audio_data_base64) {
    AIP_LOG_NOTICE("BdAsrService call.");

    char url[300];
    CURL *curl = curl_easy_init(); // 需要释放
    char *cuid = curl_easy_escape(curl, "1234567C"/*config->cuid*/, strlen("1234567C"/*config->cuid*/)); // 需要释放

    {
        std::lock_guard<std::mutex> lc(_token_mutex);
	if (!_asr_token.empty()) {
            snprintf(url, sizeof(url), "%s?cuid=%s&token=%s&dev_pid=%d",
                     _conf.get_asr_server().c_str(), cuid, _asr_token.c_str(), _conf.get_audio_type());
	} else {
	    AIP_LOG_FATAL("asr token is empty.");
	    return -1;
	}
    }

    //测试自训练平台需要打开以下信息
    /*snprintf(url, sizeof(url), "%s?cuid=%s&token=%s&dev_pid=%d&lm_id=%d",
             config->url, cuid, token, config->dev_pid, config->lm_id);*/
    curl_free(cuid);


    struct curl_slist *headerlist = NULL;
    char header[50];
    snprintf(header, sizeof(header), "Content-Type: audio/%s; rate=%d", _conf.get_audio_format().c_str(),
             16000);
    headerlist = curl_slist_append(headerlist, header); // 需要释放

    int content_len = 0;
    char *result = NULL;
    std::string audio_data_binary = base64_decode(audio_data_base64);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5); // 连接5s超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60); // 整体请求60s超时
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); // 添加http header Content-Type
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audio_data_binary.c_str()); // 音频数据
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, audio_data_binary.size()); // 音频数据长度
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);  // 需要释放

    CURLcode res_curl = curl_easy_perform(curl);


    printf("request url :%s\n", url);
    printf("header is: %s\n", header);

    ReturnCode res = RETURN_OK;
    if (res_curl != CURLE_OK) {
        // curl 失败
        AIP_LOG_FATAL("perform curl error:%d, %s.\n", res, curl_easy_strerror(res_curl));
        res = ERROR_ASR_CURL;
    } else {
        printf("YOUR FINAL RESULT: %s\n", result);
    }

    curl_slist_free_all(headerlist);
    free(result);
    curl_easy_cleanup(curl);
    return res;
}

bool BdAsrService::init(const Config& conf) {
    AIP_LOG_NOTICE("BdAsrService init.");
    bool ret = true;
    _conf = conf;
    curl_global_init(CURL_GLOBAL_ALL);

    // start the thread of getting token
    ret = start_gettoken_thread();

    return true;
}

void BdAsrService::deinit() {
    AIP_LOG_NOTICE("BdAsrService deinit.");
    curl_global_cleanup();
}

ReturnCode BdAsrService::handle_response(const char* response,
                                          std::string& token,
                                          std::string& scopes) {
    Json::Value root(Json::objectValue);
    std::string msg;
    if (!JsonUtils::load_json(response, root, msg)) {
        AIP_LOG_FATAL("Parse json failed!");
	return -1;
    }

    token = root["access_token"].asString();
    if (token.empty()) {
      AIP_LOG_FATAL("parse token error: %s\n", response);
      return ERROR_TOKEN_PARSE_ACCESS_TOKEN;
    }

    scopes = root["scope"].asString();
    if (scopes.empty()) {
        AIP_LOG_FATAL("parse scope error: %s\n", response);
	return ERROR_TOKEN_PARSE_ACCESS_TOKEN;
    }

    return RETURN_OK;
}

void BdAsrService::get_token() {
    int sleep_seconds = 15 * 3600 * 24;
    while (true) {
        char token[max_token_size];
	char url_pattern[] = "%s?grant_type=client_credentials&client_id=%s&client_secret=%s";
	char url[200];
	char *response = NULL;
	char api_token_url[] = "http://openapi.baidu.com/oauth/2.0/token";

	snprintf(url, 200, url_pattern, api_token_url, _conf.get_app_key().c_str(), _conf.get_appsecret_key().c_str());
        AIP_LOG_NOTICE("url is: %s", url);

	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url); // 注意返回值判读
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60); // 60s超时
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	CURLcode res_curl = curl_easy_perform(curl);
	ReturnCode res = RETURN_OK;
	if (res_curl != CURLE_OK) {
	  AIP_LOG_FATAL("perform curl error:%d, %s.\n", res, curl_easy_strerror(res_curl));
	  res = ERROR_TOKEN_CURL;
	} else {
	    std::string token;
	    std::string scope;
	    res = handle_response(response, token, scope); // 解析token，结果保存在token里
	    if (res == RETURN_OK) {
	        AIP_LOG_NOTICE("token: %s", token.c_str());
	    }
	}
        if (response != NULL) {
	    free(response);
	    response = NULL;
	}
	curl_easy_cleanup(curl);

	if (res != RETURN_OK) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
	} else {
	    std::lock_guard<std::mutex> lc(_token_mutex);
	    _asr_token.assign(token);
	}
	std::this_thread::sleep_for(std::chrono::seconds(sleep_seconds));
        {
	    std::lock_guard<std::mutex> lc(_token_mutex);
	    _asr_token.clear();
	}
    }
}

bool BdAsrService::start_gettoken_thread() {
    try {
      _get_token_thrd = std::thread(
				std::bind(&BdAsrService::get_token, this));
    } catch (std::runtime_error& err) {
      std::cerr << "BdAsrService::get_token failed:"
		<< err.what() << std::endl;
      AIP_LOG_FATAL("BdAsrService::get_token failed: %s", err.what());
      return false;
    }

    return true;
}
