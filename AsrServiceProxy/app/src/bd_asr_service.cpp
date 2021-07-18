#include "bd_asr_service.h"
#include "aip_log.hpp"

BdAsrService::~BdAsrService() {
}

void BdAsrService::call() {
    AIP_LOG_NOTICE("BdAsrService call.");
}

bool BdAsrService::init(const Config& conf) {
    AIP_LOG_NOTICE("BdAsrService init.");
}
