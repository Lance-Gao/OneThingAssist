#ifndef UTILS_EVENT_HPP
#define UTILS_EVENT_HPP

#include <pthread.h>

class CEvent {
public:
    CEvent(int b_initially_own, int b_manual_reset);
    virtual ~CEvent(void);
public:
    int set_event();
    int reset_event();
    int wait(long time_out);
private:
    pthread_mutex_t _lock;
    pthread_cond_t _cond;
    int _setted;
    int _manual;
};

#endif // UTILS_EVENT_HPP
