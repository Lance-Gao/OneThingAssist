#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/HierarchyMaintainer.hh>
#include <fstream>
#include "logapi.hpp"

#ifdef WIN32
#include <io.h>
#define snprintf _snprintf
#endif

#ifdef __cplusplus
extern "C" {
#endif
extern log_callback_t g_callback;
#ifdef __cplusplus
}
#endif

void align_string(std::string& msg, int align) {
    if (align <= 0) {
        return;
    }

    if (align > 8) {
        align = 8;
    }

    int len = msg.length();
    int append = align - (len % align);

    while (append --) {
        msg += " ";
    }
}

class CLogReceiver_log4cpp {
    class CReceiver : public CLogReceiver {
    public:
        CReceiver(CLogReceiver_log4cpp* host)
            : _host(host) {
        }
        virtual ~CReceiver() {            
        }
    private:
        virtual int LOGAPI on_receive_log(LogLevel level, const char* module,
                                           const char* log_title,
                                           const char* log, unsigned long thrd_id) {
            return _host->on_receive_log(level, module, log_title, log, thrd_id);
        }
    private:
        CLogReceiver_log4cpp* _host;
    };
protected:
    CLogReceiver_log4cpp() : _receiver(0) {}
public:
    virtual ~CLogReceiver_log4cpp() {}
private:
    static CLogReceiver_log4cpp* _instance;
public:
    static CLogReceiver_log4cpp* instance(int create = 1);
    static void LOGAPI release() {
        if (_instance) {
            delete _instance;
        }

        _instance = 0;
    }
private:
    CReceiver* _receiver;
public:
    int attach_to_log() {
        if (!_receiver) {
            _receiver = new CReceiver(this);
        }

        return !!_receiver;
    }
    int detach_from_log() {
        if (_receiver) {
            delete _receiver;
            _receiver = 0;
        }

        return !_receiver;
    }
public:
    virtual int LOGAPI  on_receive_log(LogLevel level, const char* module,
                                        const char* log_title,
                                        const char* log, unsigned long thrd_id) {
        log_callback_t callback = g_callback;

        char lbuf[32];
        std::string msg;

        snprintf(lbuf, 32, "%ld", thrd_id);
        msg += lbuf;

        msg += " ";
        align_string(msg, 4);
        msg += module;

        msg += " ";
        align_string(msg, 4);
        msg += log_title;

        msg += " ";
        align_string(msg, 4);
        msg += log;

        if (callback) {
            callback(level, msg.c_str());
        } else {
            log4cpp::Category* cate =
                log4cpp::HierarchyMaintainer::getDefaultMaintainer().getExistingInstance(log_title);

            if (!cate) {
                cate = &log4cpp::Category::getRoot();
            }

            cate->log(from_log_level(level), msg.c_str());
        }

        return 0;
    }

private:
    log4cpp::Priority::Value from_log_level(LogLevel ll) {
        switch (ll) {
        case NOT_SET:
            return log4cpp::Priority::NOTSET;

        case DEBUG:
            return log4cpp::Priority::DEBUG;

        case INFO:
            return log4cpp::Priority::INFO;

        case NOTICE:
            return log4cpp::Priority::NOTICE;

        case WARNING:
            return log4cpp::Priority::WARN;

        case ERROR:
            return log4cpp::Priority::ERROR;

        case FATAL:
            return log4cpp::Priority::FATAL;

        case NOTIFY:
            return log4cpp::Priority::CRIT;

        default:
            return log4cpp::Priority::NOTSET;
        }
    }
};

CLogReceiver_log4cpp* CLogReceiver_log4cpp::_instance = 0;

CLogReceiver_log4cpp* CLogReceiver_log4cpp::instance(int create) {
    if (!_instance && create) {
        _instance = new CLogReceiver_log4cpp();
        ::atexit(&CLogReceiver_log4cpp::release);
    }

    return _instance;
}

#ifdef WIN32
std::string get_config_file() {
    char path[1025] = {0};
    int path_len = 0;

    if ((path_len = GetModuleFileNameA((HMODULE)(g_instance), &path[0], 1024)) > 0
            && path_len < 1000) {
        path[path_len] = 0;
        char* file_name = 0;

        if ((file_name = strrchr(path, '\\'))) {
            char* ext = 0;

            if ((ext = strrchr(file_name, '.'))) {
                strcpy_s(ext, &path[1024] - ext , "._properties");
                return &path[0];
            }
        }
    }

    return "";
}
#else

std::string get_config_file() {
    std::string ret;
    char resolved_path[40960];
    realpath(".", resolved_path);
    ret = std::string(resolved_path);
    ret += "/config.properties";
    return ret;
}

#endif

void save_default_log4cpp_cfg(const char* path) {
    char resolved_path[40960] = {0};
    static const char* default_cfg[] = {
        "rootCategory=DEBUG, rootAppender",
        "appender.rootAppender=RollingFileAppender",
        "appender.rootAppender.maxFileSize=10485760",
        "appender.rootAppender.append=true",
        "appender.rootAppender.dailyRolling=true",
        "appender.rootAppender.maxBackupDays=180",
        "appender.rootAppender.maxBackupIndex=100",
        "appender.rootAppender.layout=PatternLayout",
        "appender.rootAppender.layout.ConversionPattern=%d	[%t]	%p	%m%n",
        0
    };
#ifdef WIN32
    std::locale loc1 = std::locale::global(std::locale(".936"));
#endif
    std::fstream save_f(path, std::ios::in | std::ios::out | std::ios::trunc);
#ifdef WIN32
    std::locale::global(std::locale(loc1));
#endif

    const char* const* item = &default_cfg[0];

    while (*item) {
        save_f << *item << std::endl;
        item++;
    }
    std::string file_path;
    std::string log_path;
    file_path = "appender.rootAppender.fileName=";
    realpath(".", resolved_path);
    
    log_path = std::string(resolved_path);
    log_path += "/log";
    
    mkdir(log_path.c_str(),  S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
    
    log_path += "/log.log";
    file_path += log_path;
    
    save_f << file_path.c_str() << std::endl;

    save_f.close();
}

#if defined(__cplusplus)
extern "C" {
#endif

void init_log4cpp() {
    struct stat status;
    std::string cfg_path = get_config_file();

    if (0 != stat(cfg_path.c_str(), &status)) {
        save_default_log4cpp_cfg(cfg_path.c_str());
    }

#ifdef WIN32
    ::SetFileAttributes(cfg_path.c_str(), FILE_ATTRIBUTE_NORMAL);
#endif

    try {
        log4cpp::PropertyConfigurator::configure(cfg_path);
    } catch (log4cpp::ConfigureFailure e) {
#ifdef WIN32
        OutputDebugString(e.what());
#else
        printf("%s", e.what());
#endif
    }
}

void term_log4cpp() {
    //    log4cpp::_category::shutdown();
    log4cpp::Appender::closeAll();
    CLogReceiver_log4cpp::release();
}

int reg_log4cpp() {
    if (CLogReceiver_log4cpp::instance() == NULL) {
        printf("reg_log4cpp error!\n");
        return -1;
    }
    return CLogReceiver_log4cpp::instance()->attach_to_log();
}

int unreg_log4cpp() {

    CLogReceiver_log4cpp *inst = CLogReceiver_log4cpp::instance(0);

    if (inst != NULL) {
        return inst->detach_from_log();
    }

    return 1;
}

#if defined(__cplusplus)
};
#endif
