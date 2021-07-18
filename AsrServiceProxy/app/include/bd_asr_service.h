#ifndef _BD_ASR_SERVICE_H_
#define _BD_ASR_SERVICE_H_

#include "asr_service.h"

class BdAsrService : public AsrService {
public:
    virtual ~BdAsrService();

    virtual void call();
    virtual bool init(const Config& conf);
};

#endif  /*_BD_ASR_SERVICE_H_*/
