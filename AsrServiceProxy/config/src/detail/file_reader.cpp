
#include "detail/file_reader.hpp"
#include "config_exception.hpp"

namespace config {

FileReader::FileReader(const std::string& file) :
    file(file.c_str()),
    size(0),
    position(BUFFER_SIZE) {
    if (!this->file.is_open()) {
        throw ConfigExceptionFileNotFound(file);
    }
}

int32_t FileReader::read() {
    if (position >= BUFFER_SIZE) {
        if (!fill_buffer()) {
            return READER_EOF;
        }
    }

    if (position >= size) {
        return READER_EOF;
    }

    return static_cast<int32_t>(buffer[position++]);
}

bool FileReader::fill_buffer() {
    try {
        if (file.eof()) {
            return READER_EOF;
        }

        file.read(buffer, BUFFER_SIZE);
        size = file.gcount();
        position = 0;
        return size > 0;
    } catch (...) {
        return false;
    }
}

void FileReader::close() {
    position = BUFFER_SIZE;
    file.close();
}

}
