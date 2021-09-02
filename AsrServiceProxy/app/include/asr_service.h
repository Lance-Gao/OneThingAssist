#ifndef _ASR_SERVICE_H_
#define _ASR_SERVICE_H_

#include <string>

class Config;

class AsrService {
public:
    virtual ~AsrService();
    virtual int call(const std::string& audio_data) = 0;
    virtual bool init(const Config& conf) = 0;
};

#endif  /*_ASR_SERVICE_H_*/
