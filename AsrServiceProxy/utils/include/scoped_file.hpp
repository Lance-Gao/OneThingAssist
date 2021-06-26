#ifndef UTILS_SCOPED_HPP
#define UTILS_SCOPED_HPP

#include "aip_common.h"

class ScopedFILE {
public:
    ScopedFILE() : _fp(NULL) {}

    // Open file at |path| with |mode|.
    // If fopen failed, operator FILE* returns NULL and errno is set.
    ScopedFILE(const char *path, const char *mode) { _fp = fopen(path, mode); }
    
    ~ScopedFILE() { 
        if (_fp) {
            fclose(_fp);
        }
        _fp = NULL;
    }

    // Close current opened file and open another file
    void reset(const char* path, const char* mode) {
        if (_fp) {
            fclose(_fp);
        }
        _fp = fopen(path, mode);
    }

    // Return internal FILE* and set this object empty
    FILE* release() {
        FILE* const tmp = _fp;
        _fp = NULL;
        return tmp;
    }
    
    operator FILE*() const { return _fp; }
    
private:
    // Copying this is wrong.
    ScopedFILE(const ScopedFILE &);
    void operator=(const ScopedFILE &);

    FILE* _fp;
};

#endif  // UTILS_SCOPED_HPP
