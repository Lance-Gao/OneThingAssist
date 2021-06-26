#include "logapi.hpp"
#include <regex.h>

#if defined(__cplusplus)
extern "C" {
#endif

int LOGAPI reg_match(const char* str, const char* rule) {
    regmatch_t pm[10];
    const size_t nmatch = 10;
    regex_t reg = {0};

    if (0 != regcomp(&reg, rule, REG_EXTENDED | REG_ICASE)) {
        return 0;
    }

    if (0 == regexec(&reg, str, nmatch, pm, 0)) {
        regfree(&reg);
        return 1;
    } else {
        regfree(&reg);
        return 0;
    }
}

#if defined(__cplusplus)
};
#endif

