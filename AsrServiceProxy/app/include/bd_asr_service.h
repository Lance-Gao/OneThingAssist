#ifndef _BD_ASR_SERVICE_H_
#define _BD_ASR_SERVICE_H_

#include <mutex>
#include <thread>
#include "asr_service.h"
#include "config.h"

typedef enum {
  RETURN_OK = 0, // 返回正常                                                                                                         
  RETURN_ERROR = 1, // 返回错误                                                                                                      
  ERROR_TOKEN_CURL = 13, // TOKEN CURL 调用错误                                                                                      
  ERROR_TOKEN_PARSE_ACCESS_TOKEN = 15,  // access_token字段在返回结果中不存在                                                        
  ERROR_TOKEN_PARSE_SCOPE = 16, // 解析scope字段，或者scope不存在                                                                    
  ERROR_ASR_FILE_NOT_EXIST = 101, // 本地文件不存在                                                                                  
  ERROR_ASR_CURL = 102 // 识别 curl 错误                                                                                             
} ReturnCode;

class BdAsrService : public AsrService {
public:
    virtual ~BdAsrService();

    virtual int call(const std::string& audio_data);
    virtual bool init(const Config& conf);

private:
    void deinit();
    void get_token();
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
