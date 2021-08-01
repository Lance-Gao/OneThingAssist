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
#include "asr_proxy_impl.h"
#include "asr_service_factory.h"

void module_log_init(void);
void module_log_fini();

Pipeline::Pipeline() {
}

Pipeline::~Pipeline() {
}

int Pipeline::get_asr_service() {
    AsrServiceFactory* factory = AsrServiceFactory::get_instance();
    _asr_service = factory->get_asr_service(AsrServiceFactory::get_asr_sourcetype(
					    _conf.get_asrapi_source()));
    return 0;
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

    get_asr_service();

    _asr_proxy_impl = std::make_shared<AsrProxyImpl>(_asr_service);
    start_brpc_server(_asr_proxy_impl);

    while (!_stop) {
        AIP_LOG_NOTICE("in loop for test modify by lance 2021.07.04");
        sleep(10);
    }

    stop_brpc_server(_conf.get_logoff_ms());
    module_log_fini();

    return 0;
}

int Pipeline::start_brpc_server(std::shared_ptr<AsrProxyImpl>& asr_proxy_impl) {
    if (_brpc_server.AddService(asr_proxy_impl.get(),
                                brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        AIP_LOG_FATAL("Fail to add service");
        return -1;
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = -1/*idle_timeout_s*/;
    options.max_concurrency = _conf.get_concurrent_number();
    if (_brpc_server.Start(_conf.get_server_port(), &options) != 0) {
      LOG(ERROR) << "Fail to start EchoServer";
      return -1;
    }

    return 0;
}

int Pipeline::stop_brpc_server(int log_off_ms) {
    if (log_off_ms > 0) {
        _brpc_server.Stop(log_off_ms);
    } else {
        AIP_LOG_FATAL("log_off_ms is less than 0.");
        return -1;
    }

    return 0;
}
