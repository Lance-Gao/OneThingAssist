#ifndef _ASR_SERVICE_H_
#define _ASR_SERVICE_H_

class AsrService {
public:
    virtual ~AsrService();
    virtual void Call() = 0;
};

#endif  /*_ASR_SERVICE_H_*/
