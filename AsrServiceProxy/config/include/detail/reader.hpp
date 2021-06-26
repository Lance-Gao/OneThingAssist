
#ifndef CONFIG_READER_HPP
#define CONFIG_READER_HPP

#include "config_types.hpp"

namespace config {

///
/// _abstract class for reading character streams.
///
class Reader {
public:
    static const int32_t READER_EOF = -1;

    /// _read a single character.
    virtual int32_t read() = 0;

    /// _close the stream.
    virtual void close() = 0;
};

}

#endif // CONFIG_READER_HPP
