#include <stdio.h>
#include <aip_log.hpp>
#include <sys/stat.h>
#include <sys/types.h> /* about files */
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <pthread.h>
#include <utils.hpp>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include "pipeline.h"

void module_log_init(void);
void module_log_fini();

Pipeline::Pipeline() {
}

Pipeline::~Pipeline() {
}

int Pipeline::print_help_or_version(char** argv) {
    if (strncmp(argv[1], "-help", strlen("-help")) == 0 ||
        strncmp(argv[1], "--help", strlen("--help")) == 0) {
        printf("%s\n", _conf.get_command_line_help());
    } else {
        printf("Unknown option!\n");
    }

    return 0;
}

int Pipeline::run(int argc, char** argv) {
    if (_conf.load_command_line(argv, argc) == 1) {
        print_help_or_version(argv);
        return 1;
    }

    if (_conf.check_server_uri() != 0) {
        printf("server uri is not satisfiable, facetracer quit!\n");
        return 1;
    }

    // change work dir and init log module
    std::stringstream path;
    path << _conf.get_working_dir() << "/log";
    mkdir(path.str().c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);

    printf("%s: %d\n", "[PID]", getpid());
    std::cout << "arg count:" << argc << std::endl;
    for (int i = 0; i < argc; i++) {
        std::cout << i << ":" << argv[i] << std::endl;
    }

    std::cout << "local time: " <<  TimeUtil::now_local_time() << std::endl;
    std::cout << "utc time: " << TimeUtil::now_utc_time() << std::endl;
    chdir(path.str().c_str());

    module_log_init();

    // back to original dir
    chdir(_conf.get_working_dir().c_str());

    AIP_LOG_NOTICE(_conf.to_string().c_str());

    // avoid the crash for SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    setpriority(PRIO_PROCESS, getpid(), -8);

    // todo loop
    while (true) {
        AIP_LOG_NOTICE("in loop for test modify by lance 2021.07.04");
        sleep(10);
    }

    module_log_fini();

    return 0;
}
