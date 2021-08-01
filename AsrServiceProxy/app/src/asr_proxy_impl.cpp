#include "asr_proxy_impl.h"

AsrProxyImpl::AsrProxyImpl(std::shared_ptr<AsrService>& asr_service) :
    _asr_service(asr_service) {
}

AsrProxyImpl::~AsrProxyImpl() {
}

void AsrProxyImpl::asr(google::protobuf::RpcController* controller,
                        const onething::AsrRequest* request,
                        onething::AsrResponse* response,
                        google::protobuf::Closure* done) {
}
