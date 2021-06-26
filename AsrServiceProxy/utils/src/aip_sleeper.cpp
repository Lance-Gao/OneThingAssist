#include "include/aip_sleeper.hpp"
#include "include/aip_log.hpp"

Sleeper::Sleeper() {
}

Sleeper::~Sleeper() {
}

int Sleeper::sleep_s(int64_t seconds) {
    int ret = 0;

    if (seconds < 0) {
        ret = -1;
        AIP_LOG_WARNING("parameter is invalid, n_seconds must be greater than 0.");
    } else {
        _sleep_time.tv_sec = seconds;
        _sleep_time.tv_nsec = 0;
        ret = nanosleep(&_sleep_time, NULL);
    }

    return ret;
}

int Sleeper::sleep_ms(int64_t m_seconds) {
    return _sleep(m_seconds * 1000 * 1000);
}

int Sleeper::sleep_us(int64_t u_seconds) {
    return _sleep(u_seconds * 1000);
}

int Sleeper::sleep_ns(int64_t n_seconds) {
    return _sleep(n_seconds);
}

int Sleeper::_sleep(int64_t n_seconds) {
    int ret = 0;
    if (n_seconds < 0) {
        ret = -1;
        AIP_LOG_WARNING("parameter is invalid, n_seconds must be greater than 0.");
    } else {
        _sleep_time.tv_sec = n_seconds / 1000000000;
        _sleep_time.tv_nsec = n_seconds % 1000000000;
        ret = nanosleep(&_sleep_time, NULL);
    }

    return ret;
}
