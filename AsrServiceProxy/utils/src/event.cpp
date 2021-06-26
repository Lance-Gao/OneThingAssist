#include "include/event.hpp"

CEvent::CEvent(int b_initially_own, int b_manual_reset)
    : _setted(b_initially_own), _manual(b_manual_reset) {

    pthread_mutex_init(&_lock, NULL);
    pthread_cond_init(&_cond, NULL);
}

CEvent::~CEvent(void) {

    pthread_mutex_destroy(&_lock);
    pthread_cond_destroy(&_cond);
}

int CEvent::set_event() {
    pthread_mutex_lock(&_lock);
    _setted = true;
    pthread_cond_broadcast(&_cond);
    pthread_mutex_unlock(&_lock);
}

int CEvent::reset_event() {
    pthread_mutex_lock(&_lock);
    _setted = false;
    pthread_mutex_unlock(&_lock);
}

int CEvent::wait(long time_out) {
    int ret = 0;
    struct timespec wait_tm = {0, 0};

    wait_tm.tv_sec = time(NULL);
    wait_tm.tv_sec += time_out / 1000;
    wait_tm.tv_nsec += (time_out % 1000) * 1000000;

    pthread_mutex_lock(&_lock);

    if (_setted) {
        if (!_manual) {
            _setted = false;
        }

        pthread_mutex_unlock(&_lock);
        ret++;
        return ret;
    } else {
        if (time_out >= 0) {
            pthread_cond_timedwait(&_cond, &_lock, &wait_tm);
        } else {
            pthread_cond_wait(&_cond, &_lock);
        }

        if (_setted) {
            if (!_manual) {
                _setted = false;
            }

            ret++;
        }

        pthread_mutex_unlock(&_lock);

        return ret;
    }
}
