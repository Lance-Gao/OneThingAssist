#include "asr_service_factory.h"
#include "bd_asr_service.h"

AsrServiceFactory* AsrServiceFactory::s_asr_service_factory = nullptr;
//AsrServiceFactory* AsrServiceFactory::_asr_service_factory = nullptr;

AsrServiceFactory::AsrServiceFactory() {
}

AsrServiceFactory::~AsrServiceFactory() {
}

std::shared_ptr<AsrService> AsrServiceFactory::get_asr_service(ASR_SOURCE_TYPE asr_source_type) {
    switch(asr_source_type) {
        case ASR_SOURCE_TYPE::BAIDU_ASR:
            _asr_service = std::make_shared<BdAsrService>();
            break;
        default:
            break;
    }

    return _asr_service;
}

ASR_SOURCE_TYPE AsrServiceFactory::get_asr_sourcetype(const std::string& asr_source) {
    if (!asr_source.compare("baidu")) {
        return ASR_SOURCE_TYPE::BAIDU_ASR;
    }

    return ASR_SOURCE_TYPE::NONE;
}

AsrServiceFactory* AsrServiceFactory::get_instance() {
    if (s_asr_service_factory == nullptr) {
        s_asr_service_factory = new AsrServiceFactory();
    }

    return s_asr_service_factory;
}
