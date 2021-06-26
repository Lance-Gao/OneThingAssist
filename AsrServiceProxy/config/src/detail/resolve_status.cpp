
#include "detail/resolve_status.hpp"
#include "detail/abstract_config_value.hpp"

namespace config {

ResolveStatus ResolveStatusEnum::from_values(const VectorAbstractConfigValue& values) {
    for (auto& v : values) {
        if (v->resolve_status() == ResolveStatus::UNRESOLVED) {
            return ResolveStatus::UNRESOLVED;
        }
    }

    return ResolveStatus::RESOLVED;
}

ResolveStatus ResolveStatusEnum::from_values(const MapAbstractConfigValue& values) {
    for (auto& v : values) {
        if (v.second->resolve_status() == ResolveStatus::UNRESOLVED) {
            return ResolveStatus::UNRESOLVED;
        }
    }

    return ResolveStatus::RESOLVED;
}

ResolveStatus ResolveStatusEnum::from_bool(bool resolved) {
    return resolved ? ResolveStatus::RESOLVED : ResolveStatus::UNRESOLVED;
}

}
