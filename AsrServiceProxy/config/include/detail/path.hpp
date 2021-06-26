
#ifndef CONFIG_PATH_HPP
#define CONFIG_PATH_HPP

#include "detail/config_base.hpp"

namespace config {

class Path : public ConfigBase {
public:
    CONFIG_CLASS(Path);

    Path(const std::string& first, const PathPtr& remainder);
    Path(const VectorString& elements = VectorString());
    Path(const VectorPath& paths_to_concat);

    std::string first();

    /// @return path minus the first element or null if no more elements
    PathPtr remainder();

    /// @return path minus the last element or null if we have just one element
    PathPtr parent();

    /// @return last element in the path
    std::string last();

    PathPtr prepend(const PathPtr& to_prepend);

    uint32_t length();

    PathPtr sub_path(uint32_t remove_from_front);
    PathPtr sub_path(uint32_t first_index, uint32_t last_index);

    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;

    /// _this doesn't have a very precise meaning, just to reduce
    /// noise from quotes in the rendered path for average cases.
    static bool has_funky_chars(const std::string& s);

private:
    void append_to_stream(std::string& s);

public:
    virtual std::string to_string() override;

    /// to_string() is a debugging-oriented version while this is an
    /// error-message-oriented human-readable one.
    std::string render();

    static PathPtr new_key(const std::string& key);
    static PathPtr new_path(const std::string& path);

private:
    std::string first_;
    PathPtr remainder_;
};

}

#endif // CONFIG_PATH_HPP
