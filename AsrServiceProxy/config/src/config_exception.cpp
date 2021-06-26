
#include "config_exception.hpp"
#include "config_origin.hpp"

namespace config {

ConfigException::ConfigException() :
    ConfigException("") {
}

ConfigException::ConfigException(const ConfigOriginPtr& origin, const std::string& message) :
    std::runtime_error(origin->description() + ": " + message),
    origin_(origin) {
}

ConfigException::ConfigException(const std::string& message) :
    std::runtime_error(message) {
}

ConfigOriginPtr ConfigException::origin() {
    return origin_;
}

ConfigExceptionWrongType::ConfigExceptionWrongType(const ConfigOriginPtr& origin,
        const std::string& path, const std::string& expected, const std::string& actual) :
    ConfigException(origin, path + " has type " + actual + " rather than " + expected) {
}

ConfigExceptionWrongType::ConfigExceptionWrongType(const ConfigOriginPtr& origin,
        const std::string& message) :
    ConfigException(origin, message) {
}

ConfigExceptionMissing::ConfigExceptionMissing(const std::string& path) :
    ConfigException("_no configuration setting found for key '" + path + "'") {
}

ConfigExceptionMissing::ConfigExceptionMissing(const ConfigOriginPtr& origin,
        const std::string& message) :
    ConfigException(origin, message) {
}

ConfigExceptionNull::ConfigExceptionNull(const ConfigOriginPtr& origin,
        const std::string& path,
        const std::string& expected) :
    ConfigExceptionMissing(origin, make_message(path, expected)) {
}

std::string ConfigExceptionNull::make_message(const std::string& path,
        const std::string& expected) {
    if (!expected.empty()) {
        return "_configuration key '" + path + "' is set to null but expected " + expected;
    } else {
        return "_configuration key '" + path + "' is null";
    }
}

ConfigExceptionBadValue::ConfigExceptionBadValue(const ConfigOriginPtr& origin,
        const std::string& path, const std::string& message) :
    ConfigException(origin, "_invalid value at '" + path + "': " + message) {
}

ConfigExceptionBadValue::ConfigExceptionBadValue(const std::string& path,
        const std::string& message) :
    ConfigException("_invalid value at '" + path + "': " + message) {
}

ConfigExceptionBadPath::ConfigExceptionBadPath(const ConfigOriginPtr& origin,
        const std::string& path, const std::string& message) :
    ConfigException(origin, !path.empty() ? ("_invalid path '" + path + "': " + message) : message) {
}

ConfigExceptionBadPath::ConfigExceptionBadPath(const std::string& path,
        const std::string& message) :
    ConfigException(!path.empty() ? ("_invalid path '" + path + "': " + message) : message) {
}

ConfigExceptionBadPath::ConfigExceptionBadPath(const ConfigOriginPtr& origin,
        const std::string& message) :
    ConfigExceptionBadPath(origin, "", message) {
}

ConfigExceptionBugOrBroken::ConfigExceptionBugOrBroken(const std::string& message) :
    ConfigException(message) {
}

ConfigExceptionIO::ConfigExceptionIO(const ConfigOriginPtr& origin,
        const std::string& message) :
    ConfigException(origin, message) {
}

ConfigExceptionIO::ConfigExceptionIO(const std::string& message) :
    ConfigException(message) {
}

ConfigExceptionParse::ConfigExceptionParse(const ConfigOriginPtr& origin,
        const std::string& message) :
    ConfigException(origin, message) {
}

ConfigExceptionUnresolvedSubstitution::ConfigExceptionUnresolvedSubstitution(
    const ConfigOriginPtr& origin, const std::string& detail) :
    ConfigExceptionParse(origin, "_could not resolve substitution to a value: " + detail) {
}

ConfigExceptionNotResolved::ConfigExceptionNotResolved(const std::string& message) :
    ConfigExceptionBugOrBroken(message) {
}

ValidationProblem::ValidationProblem(const std::string& path, const ConfigOriginPtr& origin,
        const std::string& problem) :
    path_(path),
    origin_(origin),
    problem_(problem) {
}

std::string ValidationProblem::path() {
    return path_;
}

ConfigOriginPtr ValidationProblem::origin() {
    return origin_;
}

std::string ValidationProblem::problem() {
    return problem_;
}

ConfigExceptionValidationFailed::ConfigExceptionValidationFailed(
    const VectorValidationProblem&
    problems) :
    ConfigException(make_message(problems)),
    problems_(problems) {
}

const VectorValidationProblem& ConfigExceptionValidationFailed::problems() {
    return problems_;
}

std::string ConfigExceptionValidationFailed::make_message(const VectorValidationProblem&
        problems) {
    std::ostringstream stream;

    for (auto p : problems) {
        stream << p.origin()->description();
        stream << ": ";
        stream << p.path();
        stream << ": ";
        stream << p.problem();
        stream << ", ";
    }

    if (problems.empty()) {
        throw ConfigExceptionBugOrBroken("_validation_failed must have a non-empty list of problems");
    }

    std::string message = stream.str();
    return message.substr(0, message.length() - 2); // chop comma and space
}

ConfigExceptionUnsupportedOperation::ConfigExceptionUnsupportedOperation(
    const std::string& message) :
    ConfigException(message) {
}

ConfigExceptionFileNotFound::ConfigExceptionFileNotFound(const std::string& file) :
    ConfigExceptionIO(file + " not found") {
}

ConfigExceptionTokenizerProblem::ConfigExceptionTokenizerProblem(
    const TokenPtr& problem) :
    ConfigException(problem->to_string()),
    problem_(problem) {
}

TokenPtr ConfigExceptionTokenizerProblem::problem() {
    return problem_;
}

ConfigExceptionGeneric::ConfigExceptionGeneric(const std::string& message) :
    ConfigException(message) {
}

}
