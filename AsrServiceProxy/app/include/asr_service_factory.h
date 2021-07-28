#ifndef _ASR_SERVICE_FACTORY_H_
#define _ASR_SERVICE_FACTORY_H_

#include <memory>
#include "asr_service.h"

typedef enum {
    BAIDU_ASR,
    NONE
} ASR_SOURCE_TYPE;

class AsrServiceFactory {
public:
    ~AsrServiceFactory();
    static AsrServiceFactory* get_instance();
    std::shared_ptr<AsrService> get_asr_service(ASR_SOURCE_TYPE asr_source_type);

private:
    AsrServiceFactory();
    ASR_SOURCE_TYPE get_asr_sourcetype(const std::string& asr_source);

    static AsrServiceFactory* s_asr_service_factory;
    std::shared_ptr<AsrService> _asr_service;
};

#endif  /*_ASR_SERVICE_FACTORY_H_*/
