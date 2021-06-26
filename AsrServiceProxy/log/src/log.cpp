#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>

void init_log4cpp();
void term_log4cpp();

int reg_log4cpp();
int unreg_log4cpp();

int log_init();
int log_term();

#if defined(__cplusplus)
};
#endif

static int s_module_log_initialized = 0;

void module_log_init(void) {
 
    if (s_module_log_initialized == 1) {
        return;
    }
    printf("LOG init\n");
    s_module_log_initialized = 1;

    init_log4cpp();
    log_init();
    reg_log4cpp();
}

void module_log_fini(void) {
    if (s_module_log_initialized) {
        unreg_log4cpp();
        log_term();
        term_log4cpp();
        s_module_log_initialized = 0;
        printf("LOG deinit\n");
    }
    
}

