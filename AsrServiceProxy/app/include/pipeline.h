#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <memory>
#include "config.h"
#include "asr_service.h"

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

public:
    int run(int argc, char** argv);

private:
    int print_help_or_version(char** argv);

    Config _conf;
    std::shared_ptr<AsrService> _asr_service;
};

#endif // APPLICATION_HPP
