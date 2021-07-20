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
    int print_help_or_version(char** argv);
    int start_brpc_server();
    int stop_brpc_server();

    Config _conf;
    std::shared_ptr<AsrService> _asr_service;
    brpc::Server _brpc_server;
};

#endif // APPLICATION_HPP
