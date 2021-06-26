
#include "detail/string_reader.hpp"

namespace config {

StringReader::StringReader(const std::string& str) :
    str(str),
    position(0) {
}

int32_t StringReader::read() {
    if (position >= str.length()) {
        return READER_EOF;
    }

    return static_cast<int32_t>(str[position++]);
}

void StringReader::close() {
    str.clear();
    position = 0;
}

}
