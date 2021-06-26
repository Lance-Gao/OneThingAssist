#ifndef _ASR_SERVICE_FACTORY_H_
#define _ASR_SERVICE_FACTORY_H_

class AsrServiceFactory {
public:
    AsrServiceFactory(char c);

private:
    std::shared_ptr<AsrService> _asr_service;
};

#endif  /*_ASR_SERVICE_FACTORY_H_*/
