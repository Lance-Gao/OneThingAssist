#ifndef UTILS_AIP_THREAD_HPP
#define UTILS_AIP_THREAD_HPP

#include <unistd.h>

class IThread {
public:
    IThread();
    virtual ~IThread();
    virtual int run() = 0;
    int start();
    int detach();
    int join();
    int exit(void *param);
    int set_thread_name(const char *thread_name);
    const char *get_thread_name() const {
        return _thread_name;
    }
    pthread_t get_tid() const {
        while (0 == _tid) {
            usleep(1000);
        }

        return _tid;
    }

private:
    static void *_process(void *param);

private:
    static const int MAX_NAME = 256;

    char _thread_name[MAX_NAME];
    pthread_t _tid;
};

#endif  // UTILS_AIP_THREAD_HPP
