#include "include/logimpl.hpp"
#include <scopedlock.hpp>
#ifdef WIN32
#include <atlconv.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

CLogImpl* CLogImpl::_s_instance = 0;

#if defined(__cplusplus)
extern "C" {
#endif

LogLevel set_default_log_level(LogLevel level);

int is_log_valid() {
    return CLogImpl::instance(false) != NULL;
}
int log_init() {
    if (CLogImpl::instance() == NULL) {
        printf("CLogImpl::instance() is NULL!\n");
        return 0;
    }
    return CLogImpl::instance()->start();
}

int log_term() {
    int n_ret = 1;

    if (is_log_valid()) {
        if (CLogImpl::instance() == NULL) {
            printf("CLogImpl::instance() is NULL!\n");
            return 0;
        }
        n_ret = CLogImpl::instance()->stop();
        CLogImpl::release();
    }

    return n_ret;
}

void append_log(LogLevel level, const char* module, const char* log_title,
                 const char* log) {
    if (CLogImpl::instance() == NULL) {
        printf("CLogImpl::instance() is NULL!\n");
        return;
    }
    CLogImpl::instance()->append_log(level, module, log_title, log);
}

#if defined(__cplusplus)
};
#endif

LogItem::LogItem()
    : _n_ref(1) {

}

void LogItem::add_ref() {
    _n_ref++;
}

void LogItem::release() {
    if (--_n_ref == 0) {
        delete this;
    }
}

LogReceiver::LogReceiver()
    : _b_dedicated_thread(0)
    , _str_title("*") {
    memset(&_recv, 0, sizeof(log_receiver_t));
    memset(&_re_title, 0, sizeof(regex_t));
}

LogReceiver::~LogReceiver() {
    regfree(&_re_title);
}

int LogReceiver::match(LogItem* li) {
    return 0 == regexec(&this->_re_title, li->_str_title.c_str(), 0, 0, 0);
}

CLogThread::CLogThread()
#ifdef WIN32
    : _h_thread(0)
    , _ev_quit(0, 1)
#else
    : _ev_quit(0, 1)
#endif
    , _ev_stopped(false, true)
    , _log_sync(0, 0x7fffffff)
    , _b_started(0)
    , _n_thrd_started(0) {

}

CLogThread::~CLogThread() {
    stop();

    CScopedLock lc(&_lock_recvs);

    while (_log_receivers.begin() != _log_receivers.end()) {
        delete *_log_receivers.begin();
        _log_receivers.erase(_log_receivers.begin());
    }
}

int CLogThread::check_recv(LogReceiver* recv) {
    for (std::vector<LogReceiver*>::const_iterator it = _log_receivers.begin();
            it != _log_receivers.end();
            it ++) {
        if (0 == memcmp(&recv->_recv, &(*it)->_recv, sizeof(log_receiver_t))) {
            return 1;
        }
    }

    return 0;
}

int CLogThread::register_receiver(LogReceiver* lr) {
    CScopedLock lc(&_lock_recvs);
    CScopedLock lc2(&_lock_disp);

    if (check_recv(lr)) {
        return 1;
    }

    _log_receivers.insert(_log_receivers.begin(), lr);
    return 0;
}

int CLogThread::unregister_receiver(log_receiver_t* recv) {
    CScopedLock lc(&_lock_recvs);
    CScopedLock lc2(&_lock_disp);

    for (std::vector<LogReceiver*>::iterator it = _log_receivers.begin();
            it != _log_receivers.end();
            it ++) {
        if (0 == memcmp(recv, &(*it)->_recv, sizeof(log_receiver_t))) {
            delete *it;
            _log_receivers.erase(it);
            return 1;
        }
    }

    return 0;
}
#ifdef WIN32
unsigned int __stdcall CLogThread::_log_dispatcher_thrd(void* arg) {
#else
void* CLogThread::log_dispatcher_thrd(void* arg) {
#endif
    reinterpret_cast<CLogThread*>(arg)->dispatch();
#ifdef WIN32
    _endthreadex(0);
#else
    pthread_exit(0);
#endif
    return 0;
}

void CLogThread::dispatch() {
    _b_started = true;
#ifdef WIN32
    HANDLE h_notifies[] = {_log_sync, _ev_quit};
    _interlocked_increment(&_n_thrd_started);
#else
    _n_thrd_started = 1;
#endif

    printf("LOG start do dispatch\n");
#ifdef WIN32

    while (WAIT_OBJECT_0 == ::WaitForMultipleObjects(2, &h_notifies[0], 0, INFINITE)) {
#else

    while (!_ev_quit.wait(0)) {
        if (!_log_sync.wait(1000)) {
            continue;
        }

#endif
        LogItem* li = 0;
        {
            CScopedLock lc(&_lock_logs);

            if (_logs.size() > 0) {
                li = _logs.front();
                _logs.erase(_logs.begin());
            }
        }

        if (li) {
            do_dispatch(li);
            li->release();
        }
    }
    printf("LOG stop dispatching\n");

    _ev_stopped.set_event();
}

void CLogThread::do_dispatch(LogItem* item) {
    CScopedLock lc(&_lock_disp);
    for (std::vector<LogReceiver*>::const_iterator it = _log_receivers.begin();
            it != _log_receivers.end();
            it ++) {
        if (call_receiver(*it, item)) {
            return;
        }
    }
}

int CLogThread::call_receiver(LogReceiver* recv, LogItem* item) {
#ifdef WIN32

    __try {
#endif
        return recv->_recv._receive_log(item->_e_level, item->_str_module.c_str(),
                                        item->_str_title.c_str(), item->_str_log.c_str(), item->_n_thrd_id,
                                        recv->_recv._usr_data);
#ifdef WIN32
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }

#endif

    return 0;
}


int CLogThread::start() {
    CScopedLock lc(&_lock_logs);

    while (_log_sync.wait(0)) {

    }

    _ev_quit.reset_event();
    _ev_stopped.reset_event();
#ifdef WIN32

    DWORD dw_exit = -1;

    if (_h_thread && ::GetExitCodeThread(_h_thread, &dw_exit) && dw_exit == STILL_ACTIVE) {
#else

    if (_p_thread) {
#endif
        return true;
    }

#ifdef WIN32

    if (_h_thread) {
        ::CloseHandle(_h_thread);
        _h_thread = 0;
    }

    InterlockedExchange(&_n_thrd_started, 0);

    _h_thread = (HANDLE) _beginthreadex(NULL, 0 , &CLogImpl::_log_dispatcher_thrd, this, 0 , NULL);

    if (_h_thread) {
        ::SetThreadPriority(_h_thread, THREAD_PRIORITY_ABOVE_NORMAL);
    }

    return !!_h_thread;

#else
    printf("LOG create dispatcher thread\n");
    _n_thrd_started  = 0;

    pthread_create(&_p_thread, NULL, &CLogImpl::log_dispatcher_thrd, this);

    return !!_p_thread;
#endif

}

int CLogThread::stop() {
    CScopedLock lc(&_lock_logs);
    printf("LOG dispatcher thread exit\n");
#ifdef WIN32

    if (_h_thread) {
#else

    if (_p_thread) {
#endif

        if (_b_started) {
            _ev_quit.set_event();
#ifdef WIN32
            HANDLE h_eve[] = {_h_thread, (HANDLE) _ev_stopped};

            _lock_logs.unlock();
            WaitForMultipleObjects(2, h_eve, FALSE, INFINITE);
            ::CloseHandle(_h_thread);
            _h_thread = 0;
            _lock_logs.lock();
#else

            _ev_stopped.wait(-1);
            _p_thread = 0;
#endif


            _ev_quit.reset_event();
        } else {
#ifdef WIN32

            ::TerminateThread(_h_thread, -1);
            ::CloseHandle(_h_thread);
            _h_thread = 0;
#else
            _p_thread = 0;
#endif
        }
    }

    while (_logs.begin() != _logs.end()) {
        (*_logs.begin())->release();
        _logs.erase(_logs.begin());
    }

    while (_log_sync.wait(0)) {

    }

    return 1;
}

void CLogThread::append_log(LogItem* li) {
    unsigned long remains = 0;

    if (_n_thrd_started) {
        CScopedLock lc(&_lock_logs);
        li->add_ref();
        _logs.push_back(li);

        remains = _log_sync.signal();
    } else {
        do_dispatch(li);
    }

    /*
    * avoid invoke blast
    */
    if (remains != 0) {
#ifdef WIN32
        Sleep(remains);
#else
        usleep(remains);
#endif
    }
}

int CLogThread::match(LogItem* li) {
    CScopedLock lc(&_lock_recvs);

    for (std::vector<LogReceiver*>::const_iterator it = _log_receivers.begin();
            it != _log_receivers.end();
            it ++) {
        if ((*it)->match(li)) {
            return 1;
        }
    }

    return 0;
}


CLogImpl::CLogImpl() {

}

CLogImpl::~CLogImpl() {

}

CLogImpl* CLogImpl::instance(int create) {
    if (_s_instance == NULL && create) {
        _s_instance = new CLogImpl();
    }

    return _s_instance;
}

void CLogImpl::release() {
    if (_s_instance /*&& _s_instance == this*/) {
        delete _s_instance;
        _s_instance = 0;
    }
}

void CLogImpl::append_log(LogLevel level, const char* module, const char* log_title,
                              const char* log) {
    LogItem* li = new LogItem();
#ifdef WIN32
    li->_n_thrd_id = GetCurrentThreadId();
#else
    li->_n_thrd_id = (long)pthread_self();
#endif
    li->_e_level = level;
    li->_str_module = module;
    li->_str_title = log_title;
    li->_str_log = log;

    append_log(li);
    li->release();
}

int CLogImpl::register_receiver(log_receiver_t* recv, const char* title,
                                      int b_dedicated_thread) {

    printf("LOG register_receiver %s,%d\n", title, b_dedicated_thread);
    LogReceiver* lr = new LogReceiver();

    lr->_b_dedicated_thread = b_dedicated_thread;
    lr->_recv = *recv;

    if (title && *title != 0) {
        lr->_str_title = title;
    } else {
        lr->_str_title = "*";
    }

    regcomp(&lr->_re_title, lr->_str_title.c_str(), REG_EXTENDED | REG_ICASE);

    if (0 == register_receiver(lr)) {
        return 0;
    } else {
        delete lr;
        return 1;
    }
}

int CLogImpl::register_receiver(LogReceiver* lr) {
    CScopedLock lc(&_lock_thrds);

    printf("LOG register_receiver\n");
    for (std::vector<CLogThread*>::const_iterator it = _thrds.begin();
            it != _thrds.end();
            it ++) {
        CLogThread* th = *it;

        if (th->check_recv(lr)) {
            return 1;
        }
    }

    if (lr->_b_dedicated_thread) {
        CLogThread* th = new CLogThread();
        th->register_receiver(lr);
        th->start();
        _thrds.push_back(th);
        return 0;
    } else {
        return CLogThread::register_receiver(lr);
    }
}

int CLogImpl::unregister_receiver(log_receiver_t* recv) {
    CScopedLock lc(&_lock_thrds);
    printf("LOG unregister_receiver\n");
    for (std::vector<CLogThread*>::iterator it = _thrds.begin();
            it != _thrds.end();
            it ++) {
        do {
            CLogThread* th = *it;

            if (th->unregister_receiver(recv)) {
                th->stop();
                it = _thrds.erase(it);
                delete th;

                if (it != _thrds.end()) {
                    continue;
                }
            }
        } while (0);

        if (it == _thrds.end()) {
            break;
        }
    }

    return CLogThread::unregister_receiver(recv);
}

void CLogImpl::append_log(LogItem* li) {
    CScopedLock lc(&_lock_thrds);

    for (std::vector<CLogThread*>::const_iterator it = _thrds.begin();
            it != _thrds.end();
            it ++) {
        CLogThread* th = *it;

        if (th->match(li)) {
            th->append_log(li);
        }
    }

    CLogThread::append_log(li);
}

int CLogImpl::stop() {
    CScopedLock lc(&_lock_thrds);

    for (std::vector<CLogThread*>::const_iterator it = _thrds.begin();
            it != _thrds.end();
            it ++) {
        CLogThread* th = *it;
        th->stop();
        delete th;
    }

    _thrds.clear();

    return CLogThread::stop();
}
