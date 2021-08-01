#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>
#include "config.h"
#include "asr_proxy_impl.h"
#include "asr_service.h"
#include "brpc/server.h"

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

public:
    int run(int argc, char** argv);

private:
    int get_asr_service();
    int print_help_or_version(char** argv);
    int start_brpc_server(std::shared_ptr<AsrProxyImpl>& asr_proxy_impl);
    int stop_brpc_server(int log_off_ms);

    bool _stop = false;
    Config _conf;
    std::shared_ptr<AsrService> _asr_service;
    std::shared_ptr<AsrProxyImpl> _asr_proxy_impl;
    brpc::Server _brpc_server;
};

#endif // APPLICATION_HPP
