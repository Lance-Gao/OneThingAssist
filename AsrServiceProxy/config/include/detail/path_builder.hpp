
#ifndef CONFIG_PATH_BUILER_HPP
#define CONFIG_PATH_BUILER_HPP

#include "detail/config_base.hpp"

namespace config {

class PathBuilder : public ConfigBase {
public:
    CONFIG_CLASS(PathBuilder);

private:
    void check_can_append();

public:
    void append_key(const std::string& key);
    void append_path(const PathPtr& path);
    PathPtr result();

private:
    /// the keys are kept "backward" (top of stack is end of path)
    StackString _keys;

    PathPtr _result;
};

}

#endif // CONFIG_PATH_BUILER_HPP
