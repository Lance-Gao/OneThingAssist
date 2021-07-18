#ifndef _ASR_SERVICE_H_
#define _ASR_SERVICE_H_

class Config;

class AsrService {
public:
    virtual ~AsrService();
    virtual void call() = 0;
    virtual bool init(const Config& conf) = 0;
};

#endif  /*_ASR_SERVICE_H_*/
