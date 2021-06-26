
#include "detail/misc_utils.hpp"

namespace config {

MiscUtils::MiscUtils() {
    // private
}

bool MiscUtils::file_exists(const std::string& path) {
    struct stat exists;
    return stat(path.c_str(), &exists) != -1;
}

}
