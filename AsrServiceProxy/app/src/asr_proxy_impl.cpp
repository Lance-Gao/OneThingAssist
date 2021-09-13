#include "asr_proxy_impl.h"
#include <aip_log.hpp>

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
    brpc::Controller* cntl = static_cast<brpc::Controller*>(cntl_base);

    if (_asr_service == nullptr) {
        AIP_LOG_FATAL("asr_service is nullptr!");
	response->set_code(-1);
        response->set_msg("asr service is nullptr!");
	return;
    }

    std::string asr_result;
    ReturnCode ret = _asr_service->call(request->audio().data(), request->audio().size(), asr_result);
    if (ret != RETURN_OK) {
        response->set_code(-1);
        response->set_msg("asr call failed!");
    } else {
        response->set_code(0);
        response->set_msg(asr_result.c_str());
        AIP_LOG_NOTICE("asr result is %s", asr_result.c_str());
    }
}
