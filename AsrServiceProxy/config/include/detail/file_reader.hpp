
#ifndef CONFIG_CONFIG_FILE_READER_HPP
#define CONFIG_CONFIG_FILE_READER_HPP

#include "detail/reader.hpp"
#include "detail/config_base.hpp"

namespace config {

///
/// _convenience class for reading character files.
///
class FileReader : public virtual Reader, public ConfigBase {
public:
    CONFIG_CLASS(FileReader);

    FileReader(const std::string& file);

    virtual int32_t read() override;
    virtual void close() override;

private:
    bool fill_buffer();

private:
    static const uint32_t BUFFER_SIZE = 8096;

    std::ifstream file;

    char buffer[BUFFER_SIZE];
    uint32_t size;
    uint32_t position;
};

}

#endif // CONFIG_CONFIG_FILE_READER_HPP
