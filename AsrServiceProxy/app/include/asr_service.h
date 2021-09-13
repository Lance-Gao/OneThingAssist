#ifndef _ASR_SERVICE_H_
#define _ASR_SERVICE_H_

#include <string>

typedef enum {
  RETURN_OK = 0, // 返回正常                                                                                                         
  RETURN_ERROR = 1, // 返回错误                                                                                                      
  ERROR_TOKEN_CURL = 13, // TOKEN CURL 调用错误                                                                                      
  ERROR_TOKEN_PARSE_ACCESS_TOKEN = 15,  // access_token字段在返回结果中不存在                                                        
  ERROR_TOKEN_PARSE_SCOPE = 16, // 解析scope字段，或者scope不存在                                                                    
  ERROR_ASR_FILE_NOT_EXIST = 101, // 本地文件不存在                                                                                  
  ERROR_ASR_CURL = 102 // 识别 curl 错误                                                                                             
} ReturnCode;

class Config;

class AsrService {
public:
    virtual ~AsrService();
    virtual int call(const char* audio_data, int audio_data_size, std::string& asr_result) = 0;
    virtual bool init(const Config& conf) = 0;
};

#endif  /*_ASR_SERVICE_H_*/
