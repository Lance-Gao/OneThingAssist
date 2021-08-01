#ifndef _ASR_PROXY_IMPL_H_
#define _ASR_PROXY_IMPL_H_

#include <brpc/server.h>
#include "asr_service.h"
#include "asr_service_proxy.pb.h"

class AsrProxyImpl : public onething::AsrProxyService {
public:
    AsrProxyImpl(std::shared_ptr<AsrService>& asr_service);
    ~AsrProxyImpl();

    void asr(google::protobuf::RpcController* controller,
             const onething::AsrRequest* request,
             onething::AsrResponse* response,
             google::protobuf::Closure* done);

private:
    std::shared_ptr<AsrService>& _asr_service;
};

#endif  /*_ASR_PROXY_IMPL_H_*/
