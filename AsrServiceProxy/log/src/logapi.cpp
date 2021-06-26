#include "logapi.hpp"
#include <stdio.h>
#include <stdarg.h>
#ifdef WIN32
#include <vadefs.h>
#include <crtdefs.h>
#include <atlconv.h>

#pragma warning(push)
#pragma warning(disable:4482)
#endif

#if defined(__cplusplus)
extern "C" {
#endif
unsigned long check_level();

static LogLevel s_log_level = INFO;
static unsigned long s_log_flags = check_level();
static LogLevel s_default_log_level = INFO;

#ifdef WIN32
volatile log_callback_t __declspec(align(4)) g_callback = NULL;
#else
volatile log_callback_t g_callback __attribute__((aligned(4)));
#endif

log_callback_t LOGAPI log_set_callback(log_callback_t callback) {
#ifdef WIN32
    return (log_callback_t) InterlockedExchange((volatile long*) &g_callback,
            (long) callback);
#else
    log_callback_t ret;
    ret = g_callback;
    g_callback = callback;
    return ret;
#endif
}

unsigned long check_level() {
    unsigned long chk = 0xffff;
    int i = 0;

    while ((i < 16) && 0 == ((s_log_level) & (1 << (i++)))) {
        chk &= ~((1 << i) - 1);
    }

    s_log_flags = chk;

    return s_log_flags;
}

void append_log(LogLevel level, const char* module, const char* log_title,
                 const char* log);

LogLevel set_default_log_level(LogLevel level) {
    LogLevel old_level = s_default_log_level;
    s_default_log_level = level;
    return old_level;
}

LogLevel LOGAPI set_log_level(LogLevel level) {
    LogLevel old_level = s_log_level;
    s_log_level = level;
    check_level();
    return old_level;
}

LogLevel LOGAPI get_log_level() {
    return s_log_level;
}

LogLevel LOGAPI reset_log_level() {
    s_log_level = s_default_log_level;
    check_level();
    return get_log_level();
}

int LOGAPI is_log_level_enabled(LogLevel level) {
    return ((level == NOTIFY) || (s_log_flags & level)) ? 1 : 0;
}

void LOGAPI report_log(LogLevel level, const char* module, const char* log_title,
                        const char* log) {
    if (is_log_level_enabled(level)) {
        append_log(level, module, log_title, log);
    }
}

void LOGAPI v_log_log(LogLevel level, const char* module, const char* log_title,
                      const char* log, va_list lst) {
    if (is_log_level_enabled(level)) {
        char* buf = (char*) calloc(4096, sizeof(char));

        if (buf) {
            vsnprintf(&buf[0], 4096, log, lst);
            report_log(level, module, log_title, &buf[0]);
            free(buf);
        }
    }
}
void LOGAPI log_log(LogLevel level, const char* module, const char* log_title,
                     const char* log, ...) {
    if (is_log_level_enabled(level)) {
        va_list args;
        va_start(args, log);
        v_log_log(level, module, log_title, log, args);
        va_end(args);
    }
}

void LOGAPI log_debug(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(DEBUG)) {
        va_list args;
        va_start(args, log);
        v_log_log(DEBUG, module, log_title, log, args);
        va_end(args);
    }
}

void LOGAPI log_info(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(INFO)) {
        va_list args;
        va_start(args, log);
        v_log_log(INFO, module, log_title, log, args);
        va_end(args);
    }
}

void LOGAPI log_notice(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(NOTICE)) {
        va_list args;
        va_start(args, log);
        v_log_log(NOTICE, module, log_title, log, args);
        va_end(args);
    }
}

void LOGAPI log_warning(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(WARNING)) {
        va_list args;
        va_start(args, log);
        v_log_log(WARNING, module, log_title, log, args);
        va_end(args);
    }
}
void LOGAPI log_error(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(ERROR)) {
        va_list args;
        va_start(args, log);
        v_log_log(ERROR, module, log_title, log, args);
        va_end(args);
    }
}

void LOGAPI log_fatal(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(FATAL)) {
        va_list args;
        va_start(args, log);
        v_log_log(FATAL, module, log_title, log, args);
        va_end(args);
    }
}



void LOGAPI log_notify(const char* module, const char* log_title, const char* log, ...) {
    if (is_log_level_enabled(NOTIFY)) {
        va_list args;
        va_start(args, log);
        v_log_log(NOTIFY, module, log_title, log, args);
        va_end(args);
    }
}

#if defined(__cplusplus)
};
#endif
#ifdef WIN32
#pragma warning(pop)
#endif

