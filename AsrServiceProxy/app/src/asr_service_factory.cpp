#include "asr_service_factory.h"
#include "bd_asr_service.h"

AsrServiceFactory::AsrServiceFactory(char c) {
    switch(c) {
        case 'BD_ASR':
            _asr_service = std::make_shared<BdAsrService>();
            break;
        default:
            break;
    }
}
