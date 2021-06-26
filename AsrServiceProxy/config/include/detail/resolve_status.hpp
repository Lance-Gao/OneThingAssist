
#ifndef CONFIG_RESOLVE_STATUS_HPP
#define CONFIG_RESOLVE_STATUS_HPP

#include "config_types.hpp"

namespace config {

///
/// _status of substitution resolution.
///
enum class ResolveStatus : public uint32_t {
    UNRESOLVED, RESOLVED
};

class ResolveStatusEnum {
public:
    static ResolveStatus from_values(const VectorAbstractConfigValue& values);
    static ResolveStatus from_values(const MapAbstractConfigValue& values);
    static ResolveStatus from_bool(bool resolved);
};

}

#endif // CONFIG_RESOLVE_STATUS_HPP
