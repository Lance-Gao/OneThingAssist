#include "asr_proxy_impl.h"

AsrProxyImpl::AsrProxyImpl(std::shared_ptr<AsrService>& asr_service) :
    _asr_service(asr_service) {
}

AsrProxyImpl::~AsrProxyImpl() {
}

void AsrProxyImpl::asr(google::protobuf::RpcController* cntl_base,
                        const onething::AsrRequest* request,
                        onething::AsrResponse* response,
                        google::protobuf::Closure* done) {
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl =
    static_cast<brpc::Controller*>(cntl_base);

    // Echo request and its attachment                                                                                             
    response->set_value(request->value());
    if (true) {
        cntl->response_attachment().append(cntl->request_attachment());
    }
}
