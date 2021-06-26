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

void module_log_init(void);
void module_log_fini();

Pipeline::Pipeline() {
}

Pipeline::~Pipeline() {
}

int Application::print_help_or_version(char** argv) {
    if (strncmp(argv[1], "-help", strlen("-help")) == 0 ||
        strncmp(argv[1], "--help", strlen("--help")) == 0) {
        printf("%s\n", _conf.get_command_line_help());
    } else if (strncmp(argv[1], "-version", strlen("-version")) == 0 ||
              strncmp(argv[1], "--version", strlen("--version")) == 0) {
        printf("%s\n", _conf.get_app_version().c_str());
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
    path << _conf.get_working_dir() << "/" << _conf.get_id();
    mkdir(path.str().c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);

    // check if instance id is running
    long running_pid = 0;
    if (already_running(path.str().c_str(), &running_pid) != 0) {
        if (running_pid != 0) {
            printf("%s: %d\n", "[PID]", running_pid);
        }
        printf("camera id is already running\n");
        return -1;
    }

    printf("%s: %d\n", "[PID]", getpid());
    std::cout << "arg count:" << argc << std::endl;
    for (int i = 0; i < argc; i++) {
        cout << i << ":" << argv[i] << std::endl;
    }

    cout << "local time: " <<  TimeUtil::now_local_time() << std::endl;
    cout << "utc time: " << TimeUtil::now_utc_time() << std::endl;
    chdir(path.str().c_str());

    module_log_init();

    // back to original dir
    chdir(_conf.get_working_dir().c_str());

    AIP_LOG_NOTICE(_conf.to_string().c_str());

    // avoid the crash for SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    setpriority(PRIO_PROCESS, getpid(), -8);

    _exit = 0;
    std::thread mon(&Application::report, this);
    int ret = _pipeline.run(&_conf);
    _exit = 1;

    if (mon.joinable()) {
        mon.join();
    }

    module_log_fini();

    return ret;
}
