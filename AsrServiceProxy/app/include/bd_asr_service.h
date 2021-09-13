#ifndef _BD_ASR_SERVICE_H_
#define _BD_ASR_SERVICE_H_

#include <mutex>
#include <thread>
#include "asr_service.h"
#include "config.h"

class BdAsrService : public AsrService {
public:
    virtual ~BdAsrService();

    virtual int call(const char* audio_data, int audio_data_size, std::string& asr_result);
    virtual bool init(const Config& conf);

private:
    void deinit();
    void get_token();
    ReturnCode handle_asr_result(const char* response,
                                 std::string& asr_result);
    ReturnCode handle_response(const char* response,
                               std::string& token,
                               std::string& scopes);
    bool speech_get_token(const char *api_key, const char *secret_key, const char *scope, char *token);
    bool start_gettoken_thread();

    std::thread _get_token_thrd;
    std::string _asr_token;
    Config _conf;
    std::mutex _token_mutex;
    static const char* api_token_url = "http://openapi.baidu.com/oauth/2.0/token";
    static const int max_token_size = 100;
};

#endif  /*_BD_ASR_SERVICE_H_*/
