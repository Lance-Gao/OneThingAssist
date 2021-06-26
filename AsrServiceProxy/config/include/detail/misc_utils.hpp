
#ifndef CONFIG_MISC_UTILS_HPP
#define CONFIG_MISC_UTILS_HPP

#include "config_types.hpp"

namespace config {

///
/// _contains generic utility methods.
///
class MiscUtils {
private:
    MiscUtils();

public:
    /// _return a new map that is dynamically cast from elements from another
    template <typename _t, typename _u>
    static _t dynamic_map(_u source) {
        _t new_map;

        for (auto& v : source) {
            new_map.insert(std::make_pair(v.first,
                                          std::dynamic_pointer_cast<typename _t::mapped_type::element_type>(v.second)));
        }

        return new_map;
    }

    /// _return a new vector that is dynamically cast from elements from another
    template <typename _t, typename _u>
    static _t dynamic_vector(_u source) {
        _t new_vector;
        new_vector.reserve(source.size());

        for (auto& v : source) {
            new_vector.push_back(std::dynamic_pointer_cast<typename _t::value_type::element_type>(v));
        }

        return new_vector;
    }

    /// _extract map keys
    template <typename _u, typename _v>
    static void key_set(_u first, _u last, _v output) {
        for (; first != last; ++first) {
            *(output++) = first->first;
        }
    }

    /// _compare 2 maps (assumes keys are directly comparable)
    template <typename _t>
    static bool map_equals(const _t& a, const _t& b) {
        if (a.size() != b.size()) {
            return false;
        }

        std::set<typename _t::key_type> a_keys;
        key_set(a.begin(), a.end(), std::inserter(a_keys, a_keys.end()));
        std::set<typename _t::key_type> b_keys;
        key_set(b.begin(), b.end(), std::inserter(b_keys, b_keys.end()));

        if (a_keys != b_keys) {
            return false;
        }

        for (auto& kv : a) {
            if (!config_equals<ConfigBasePtr>()(std::dynamic_pointer_cast<ConfigBase>(kv.second),
                                                   std::dynamic_pointer_cast<ConfigBase>(b.find(kv.first)->second))) {
                return false;
            }
        }

        return true;
    }

    /// _compare 2 vectors (assumes elements are of type ConfigBase)
    template <typename _t>
    static bool vector_equals(const _t& a, const _t& b) {
        if (a.size() != b.size()) {
            return false;
        }

        for (uint32_t i = 0; i < a.size(); ++i) {
            if (!config_equals<ConfigBasePtr>()(std::dynamic_pointer_cast<ConfigBase>(a[i]),
                                                   std::dynamic_pointer_cast<ConfigBase>(b[i]))) {
                return false;
            }
        }

        return true;
    }

    /// _return true is given file path exists
    static bool file_exists(const std::string& path);
};

}

#endif // CONFIG_MISC_UTILS_HPP
