#include "include/logapi.hpp"
#include "include/logimpl.hpp"

#if defined(__cplusplus)
extern "C" {
#endif

int LOGAPI register_log_receiver(log_receiver_t* receiver, const char* title,
                                    int dedicated_thread) {
    if (CLogImpl::instance() == NULL) {
        printf("CLogImpl::instance() is NULL!\n");
        return 1;
    }
    return CLogImpl::instance()->register_receiver(receiver, title, dedicated_thread);
}
int LOGAPI unregister_log_receiver(log_receiver_t* receiver) {
    if (CLogImpl::instance() == NULL) {
        printf("CLogImpl::instance() is NULL!\n");
        return 1;
    }
    return CLogImpl::instance()->unregister_receiver(receiver);
}

#if defined(__cplusplus)
};
#endif
