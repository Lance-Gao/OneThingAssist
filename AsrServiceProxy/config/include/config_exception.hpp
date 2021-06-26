
#ifndef CONFIG_CONFIG_EXCEPTION_HPP
#define CONFIG_CONFIG_EXCEPTION_HPP

#include "detail/token.hpp"

namespace config {

#define EXCEPTION_CLASS(_name) \
    virtual void raise() { \
        throw *this; \
    } \
    virtual ConfigExceptionPtr clone() { \
        return std::make_shared<_name>(*this); \
    }

///
/// _all exceptions thrown by the library are subclasses of
/// <code>ConfigException</code>.
///
class ConfigException : public std::runtime_error {
public:
    EXCEPTION_CLASS(ConfigException)

    ConfigException();

protected:
    ConfigException(const ConfigOriginPtr& origin, const std::string& message = "");
    ConfigException(const std::string& message);

public:
    /// _returns an "origin" (such as a filename and line number) for the
    /// exception, or null if none is available. _if there's no sensible origin
    /// for a given exception, or the kind of exception doesn't meaningfully
    /// relate to a particular origin file, this returns null. _never assume this
    /// will return non-null, it can always return null.
    ///
    /// @return origin of the problem, or null if unknown / inapplicable
    ConfigOriginPtr origin();

private:
    ConfigOriginPtr origin_;
};

///
/// _exception indicating that the type of a value does not match the type you
/// requested.
///
class ConfigExceptionWrongType : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionWrongType)

    ConfigExceptionWrongType(const ConfigOriginPtr& origin,
                                 const std::string& path,
                                 const std::string& expected,
                                 const std::string& actual);
    ConfigExceptionWrongType(const ConfigOriginPtr& origin,
                                 const std::string& message);
};

///
/// _exception indicates that the setting was never set to anything, not even
/// null.
///
class ConfigExceptionMissing : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionMissing)

    ConfigExceptionMissing(const std::string& path);

protected:
    ConfigExceptionMissing(const ConfigOriginPtr& origin,
                              const std::string& message);
};

///
/// _exception indicates that the setting was treated as missing because it
/// was set to null.
///
class ConfigExceptionNull : public ConfigExceptionMissing {
public:
    EXCEPTION_CLASS(ConfigExceptionNull)

    ConfigExceptionNull(const ConfigOriginPtr& origin,
                           const std::string& path,
                           const std::string& expected);

private:
    static std::string make_message(const std::string& path,
                                    const std::string& expected);
};

///
/// _exception indicating that a value was messed up, for example you may have
/// asked for a duration and the value can't be sensibly parsed as a
/// duration.
///
class ConfigExceptionBadValue : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionBadValue)

    ConfigExceptionBadValue(const ConfigOriginPtr& origin,
                                const std::string& path,
                                const std::string& message);
    ConfigExceptionBadValue(const std::string& path,
                                const std::string& message);
};

///
/// _exception indicating that a path expression was invalid. _try putting
/// double quotes around path elements that contain "special" characters.
///
class ConfigExceptionBadPath : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionBadPath)

    ConfigExceptionBadPath(const ConfigOriginPtr& origin,
                               const std::string& path,
                               const std::string& message);
    ConfigExceptionBadPath(const std::string& path,
                               const std::string& message);
    ConfigExceptionBadPath(const ConfigOriginPtr& origin,
                               const std::string& message);
};

///
/// _exception indicating that there's a bug in something (possibly the
/// library itself) or the runtime environment is broken. _this exception
/// should never be handled; instead, something should be fixed to keep the
/// exception from occurring. _this exception can be thrown by any method in
/// the library.
///
class ConfigExceptionBugOrBroken : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionBugOrBroken)

    ConfigExceptionBugOrBroken(const std::string& message);
};

///
/// _exception indicating that there was an _i_o error.
///
class ConfigExceptionIO : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionIO)

    ConfigExceptionIO(const ConfigOriginPtr& origin,
                          const std::string& message);
    ConfigExceptionIO(const std::string& message);
};

///
/// _exception indicating that there was a parse error.
///
class ConfigExceptionParse : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionParse)

    ConfigExceptionParse(const ConfigOriginPtr& origin,
                            const std::string& message);
};

///
/// _exception indicating that a substitution did not resolve to anything.
/// _thrown by {@link Config#resolve}.
///
class ConfigExceptionUnresolvedSubstitution : public ConfigExceptionParse {
public:
    EXCEPTION_CLASS(ConfigExceptionUnresolvedSubstitution)

    ConfigExceptionUnresolvedSubstitution(const ConfigOriginPtr& origin,
            const std::string& detail);
};

///
/// _exception indicating that you tried to use a function that requires
/// substitutions to be resolved, but substitutions have not been resolved
/// (that is, {@link Config#resolve} was not called). _this is always a bug in
/// either application code or the library; it's wrong to write a handler for
/// this exception because you should be able to fix the code to avoid it by
/// adding calls to {@link Config#resolve}.
///
class ConfigExceptionNotResolved : public ConfigExceptionBugOrBroken {
public:
    EXCEPTION_CLASS(ConfigExceptionNotResolved)

    ConfigExceptionNotResolved(const std::string& message);
};

///
/// _information about a problem that occurred in {@link Config#check_valid}. _a
/// {@link ConfigExceptionValidationFailed} exception thrown from
/// <code>check_valid()</code> includes a list of problems encountered.
///
class ValidationProblem {
public:
    ValidationProblem(const std::string& path,
                        const ConfigOriginPtr& origin,
                        const std::string& problem);

    /// _returns the config setting causing the problem.
    std::string path();

    /// _returns the config setting causing the problem.
    ConfigOriginPtr origin();

    /// _returns a description of the problem.
    std::string problem();

private:
    std::string path_;
    ConfigOriginPtr origin_;
    std::string problem_;
};

///
/// _exception indicating that {@link Config#check_valid} found validity
/// problems. _the problems are available via the {@link #problems()} method.
/// _the <code>what()</code> of this exception is a potentially very
/// long string listing all the problems found.
///
class ConfigExceptionValidationFailed : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionValidationFailed)

    ConfigExceptionValidationFailed(const VectorValidationProblem& problems);

    const VectorValidationProblem& problems();

private:
    static std::string make_message(const VectorValidationProblem& problems);

private:
    VectorValidationProblem problems_;
};

///
/// _exception indicating that an action is unsupported.
///
class ConfigExceptionUnsupportedOperation : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionUnsupportedOperation)

    ConfigExceptionUnsupportedOperation(const std::string& message);
};

///
/// _exception indicating that a file does not exist when attempting to open.
///
class ConfigExceptionFileNotFound : public ConfigExceptionIO {
public:
    EXCEPTION_CLASS(ConfigExceptionFileNotFound)

    ConfigExceptionFileNotFound(const std::string& file);
};

///
/// _exception indicating that a problem occurred during tokenizing.
///
class ConfigExceptionTokenizerProblem : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionTokenizerProblem)

    ConfigExceptionTokenizerProblem(const TokenPtr& problem);

    TokenPtr problem();

private:
    TokenPtr problem_;
};

///
/// _exception that doesn't fall into any other category.
///
class ConfigExceptionGeneric : public ConfigException {
public:
    EXCEPTION_CLASS(ConfigExceptionGeneric)

    ConfigExceptionGeneric(const std::string& message);
};

}

#endif // CONFIG_CONFIG_EXCEPTION_HPP
