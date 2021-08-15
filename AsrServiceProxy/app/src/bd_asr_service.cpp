#include <chrono>
#include "bd_asr_service.h"
#include "aip_log.hpp"

BdAsrService::~BdAsrService() {
    deinit();
}

void BdAsrService::call() {
    AIP_LOG_NOTICE("BdAsrService call.");
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
	}/* else {
	  res = parse_token(response, scope, token); // 解析token，结果保存在token里
	  if (res == RETURN_OK) {
            printf("token: %s of %s\n", token, response);
	  }
	}*/
        if (response != NULL) {
	    free(response);
	    response = NULL;
	}
	curl_easy_cleanup(curl);

	if (res != RETURN_OK) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
	}
	std::this_thread::sleep_for(std::chrono::seconds(sleep_seconds));
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
