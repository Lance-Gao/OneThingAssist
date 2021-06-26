
#ifndef CONFIG_CONFIG_ORIGIN_HPP
#define CONFIG_CONFIG_ORIGIN_HPP

#include "config_types.hpp"

namespace config {

///
/// _represents the origin (such as filename and line number) of a
/// {@link ConfigValue} for use in error messages. _obtain the origin of a value
/// with {@link ConfigValue#origin}. _exceptions may have an origin, see
/// {@link ConfigException#origin}, but be careful because
/// <code>ConfigException.origin()</code> may return null.
///
/// <p>
/// _it's best to use this interface only for debugging; its accuracy is
/// "best effort" rather than guaranteed, and a potentially-noticeable amount of
/// memory could probably be saved if origins were not kept around, so in the
/// future there might be some option to discard origins.
///
/// <p>
/// <em>_do not implement this interface</em>; it should only be implemented by
/// the config library. _arbitrary implementations will not work because the
/// library internals assume a specific concrete implementation. _also, this
/// interface is likely to grow new methods over time, so third-party
/// implementations will break.
///
class ConfigOrigin {
public:
    /// _returns a string describing the origin of a value or exception. _this will
    /// never return null.
    ///
    /// @return string describing the origin
    virtual std::string description() = 0;

    /// _returns a filename describing the origin. _this will return null if the
    /// origin was not a file.
    ///
    /// @return filename of the origin or null
    virtual std::string filename() = 0;

    /// _returns a line number where the value or exception originated. _this will
    /// return -1 if there's no meaningful line number.
    ///
    /// @return line number or -1 if none is available
    virtual int32_t line_number() = 0;

    /// _returns any comments that appeared to "go with" this place in the file.
    /// _often an empty list, but never null. _the details of this are subject to
    /// change, but at the moment comments that are immediately before an array
    /// element or object field, with no blank line after the comment, "go with"
    /// that element or field.
    ///
    /// @return any comments that seemed to "go with" this origin, empty list if
    ///         none
    virtual VectorString comments() = 0;
};

}

#endif // CONFIG_CONFIG_ORIGIN_HPP
