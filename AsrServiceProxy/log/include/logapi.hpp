#ifndef LOG_LOGAPI_HPP
#define LOG_LOGAPI_HPP

#include <stdlib.h>
#include <string.h>

#if defined(__cplusplus)
extern "C" {
#endif

/************************************************************************/
/* _data _type _definition                                                 */
/************************************************************************/

#ifndef LOGAPI
#ifdef WIN32
#define LOGAPI __cdecl
#else
#define LOGAPI
#endif
#endif

typedef enum __LogLevel {
    NOT_SET    =    0x00,
    DEBUG     =    0x01,
    INFO      =    0x02,
    NOTICE    =    0x04,
    WARNING   =    0x08,
    ERROR    =    0x10,
    FATAL     =    0x20,
    NOTIFY    =    0x40,
} LogLevel;

typedef struct {
    typedef int (LOGAPI* receive_log_callback_t)(LogLevel level, const char* module,
                                         const char* log_title,
                                         const char* log, unsigned long thrd_id, void* usr_data);
    receive_log_callback_t _receive_log;

    void* _usr_data;

} log_receiver_t;

typedef void (*log_callback_t)(LogLevel, const char*);

/************************************************************************/
/* API _declaration                                                      */
/************************************************************************/

LogLevel LOGAPI set_log_level(LogLevel level);

LogLevel LOGAPI get_log_level();

LogLevel LOGAPI reset_log_level();

int LOGAPI is_log_level_enabled(LogLevel level);

log_callback_t LOGAPI log_set_callback(log_callback_t);

void LOGAPI log_log(LogLevel level, const char* module, const char* log_title,
                     const char* log, ...);

/************************************************************************/
/* _wrapper for _log_log                                                   */
/************************************************************************/
void LOGAPI log_debug(const char* module, const char* log_title, const char* log, ...);

void LOGAPI log_info(const char* module, const char* log_title, const char* log, ...);

void LOGAPI log_notice(const char* module, const char* log_title, const char* log, ...);

void LOGAPI log_warning(const char* module, const char* log_title, const char* log, ...);

void LOGAPI log_error(const char* module, const char* log_title, const char* log, ...);

void LOGAPI log_fatal(const char* module, const char* log_title, const char* log, ...);

void LOGAPI log_notify(const char* module, const char* log_title, const char* log, ...);

int LOGAPI register_log_receiver(log_receiver_t* receiver, const char* title,
                                  int dedicated_thread);

int LOGAPI unregister_log_receiver(log_receiver_t* receiver);


/************************************************************************/
/* _reg_match                                                             */
/************************************************************************/

int LOGAPI reg_match(const char* str, const char* rule);

#if defined(__cplusplus)
};
#endif

class CLogReceiver {
public:
    CLogReceiver()
        : _n_dedicated_thread(0) {
        set_filter("*");
        init();
    }

    CLogReceiver(int dedicated_thread)
        : _n_dedicated_thread(dedicated_thread) {
        set_filter("*");
        init();
    }
    CLogReceiver(int dedicated_thread, const char* filter)
        : _n_dedicated_thread(dedicated_thread) {
        set_filter(filter);
        init();
    }

    virtual ~CLogReceiver() {
        term();
        free(_str_title);
    }
    void set_filter(const char* title) {
        if (title && *title) {
           _str_title = strdup(title);
        } else {
            set_filter("*");
        }
    }
protected:
    virtual void init() {

        memset(&_receiver, 0, sizeof(log_receiver_t));

        _receiver._usr_data = this;

        _receiver._receive_log = (log_receiver_t::receive_log_callback_t)receive_log_thunk;

        ::register_log_receiver(&_receiver,_str_title, _n_dedicated_thread);
    }

    virtual void term() {
        ::unregister_log_receiver(&_receiver);
    }

public:
    virtual int LOGAPI  on_receive_log(LogLevel level, const char* module,
                                        const char* log_title,
                                        const char* log, unsigned long thrd_id) {
        return 0;
    }
private:
    log_receiver_t _receiver;
    int _n_dedicated_thread;
    char*_str_title;
private:
    static int LOGAPI receive_log_thunk(LogLevel level, const char* module,
                                         const char* log_title,
                                         const char* log, unsigned long thrd_id, void* usr_data) {
        return reinterpret_cast<CLogReceiver*>(usr_data)->on_receive_log(level, module,
                log_title, log, thrd_id);
    }
};

class CLogNotify : public CLogReceiver {
public:
    CLogNotify(const char* lp_title)
        : CLogReceiver(1, lp_title) {

    }

    virtual ~CLogNotify() {
    }

public:
    virtual int LOGAPI on_receive_log(LogLevel level, const char* module,
                                       const char* log_title,
                                       const char* log, unsigned long thrd_id) {
        if (level == NOTIFY) {
            return on_notify(module, log);
        } else {
            return 0;
        }
    }

    virtual int LOGAPI on_notify(const char* module, const char* msg) = 0;
};

#endif // LOG_LOGAPI_HPP

