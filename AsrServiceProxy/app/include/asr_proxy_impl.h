#ifndef _ASR_PROXY_IMPL_H_
#define _ASR_PROXY_IMPL_H_

#include <brpc/server.h>
#include "asr_service_proxy.pb.h"

class AsrProxyImpl : public onething::AsrProxyService {
public:
    AsrProxyImpl();
    ~AsrProxyImpl();

    void asr(google::protobuf::RpcController* controller,
             const onething::AsrRequest* request,
             onething::AsrResponse* response,
             google::protobuf::Closure* done);
};

#endif  /*_ASR_PROXY_IMPL_H_*/
