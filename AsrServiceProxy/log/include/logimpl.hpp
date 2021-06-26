#ifndef LOG_LOGIMPL_HPP
#define LOG_LOGIMPL_HPP

#include "logapi.hpp"
#include <semaphore.hpp>
#include <event.hpp>
#include <critsec.hpp>
#ifdef WIN32
#include "process.h"
#endif
#include <regex.h>
#include <string>
#include <vector>
#include <atomic>

class LogItem {
public:
    LogItem();
public:
    LogLevel _e_level;
    std::string _str_module;
    std::string _str_title;
    std::string _str_log;
    long _n_thrd_id;
private:
    std::atomic_int _n_ref;
public:
    void add_ref();
    void release();
};

class LogReceiver {
public:
    LogReceiver();
    ~LogReceiver();
public:
    log_receiver_t _recv;

    int _b_dedicated_thread;

    std::string _str_title;
    regex_t _re_title;
public:
    int match(LogItem* lit);
};

class CLogThread {
public:
    CLogThread();
    virtual ~CLogThread();

public:
    int start();
    virtual int stop();

    int check_recv(LogReceiver* recv);
    virtual int register_receiver(LogReceiver* lr);
    virtual int unregister_receiver(log_receiver_t* recv);
    virtual void append_log(LogItem* li);

    int match(LogItem* li);
private:
    CCritSec _lock_logs;
    CCritSec _lock_recvs;
    CCritSec _lock_disp;
    CSemaphore _log_sync;
    CEvent _ev_quit;
    CEvent _ev_stopped;
#ifdef WIN32
    HANDLE _h_thread;
#else
    pthread_t _p_thread;
#endif
    int _b_started;

protected:
#ifdef WIN32
    static unsigned int __stdcall log_dispatcher_thrd(void* arg);
#else
    static void* log_dispatcher_thrd(void* arg);
#endif
    void dispatch();

    void do_dispatch(LogItem* item);

    int call_receiver(LogReceiver* recv, LogItem* item);

protected:
    std::vector<LogItem*> _logs;
    std::vector<LogReceiver*> _log_receivers;
    std::atomic_int _n_thrd_started;
};

class CLogImpl : public CLogThread {
protected:
    CLogImpl();
public:
    virtual ~CLogImpl();
private:
    static CLogImpl* _s_instance;
public:
    static CLogImpl* instance(int create = 1);
    static void release();

protected:
    CCritSec _lock_thrds;
    std::vector<CLogThread*> _thrds;

public:
    void append_log(LogLevel level, const char* module, const char* log_title,
                     const char* log);

    int register_receiver(log_receiver_t* recv, const char* title, int b_dedicated_thread);
    virtual int unregister_receiver(log_receiver_t* recv);
    
    virtual int stop();
protected:
    virtual int register_receiver(LogReceiver* lr);
    virtual void append_log(LogItem* li);
};

#endif // LOG_LOGIMPL_HPP

