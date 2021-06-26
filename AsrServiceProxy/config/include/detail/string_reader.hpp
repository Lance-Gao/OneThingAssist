
#ifndef CONFIG_STRING_READER_HPP
#define CONFIG_STRING_READER_HPP

#include "detail/reader.hpp"
#include "detail/config_base.hpp"

namespace config {

///
/// _a character stream whose source is a string.
///
class StringReader : public virtual Reader, public ConfigBase {
public:
    CONFIG_CLASS(StringReader);

    StringReader(const std::string& str);

    virtual int32_t read() override;
    virtual void close() override;

private:
    std::string str;
    uint32_t position;
};

}

#endif // CONFIG_STRING_READER_HPP
