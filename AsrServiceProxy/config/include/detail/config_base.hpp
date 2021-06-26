
#ifndef CONFIG_CONFIG_BASE_HPP
#define CONFIG_CONFIG_BASE_HPP

#include "detail/instance_utils.hpp"
#include "detail/misc_utils.hpp"
#include "config_types.hpp"

#define CONFIG_CLASS(_name) \
    std::shared_ptr<_name> shared_from_this() { \
        return std::static_pointer_cast<_name>(ConfigBase::shared_from_this()); \
    } \
    template <class... _args> \
    static std::shared_ptr<_name> make_instance(_args && ... args) { \
        std::shared_ptr<_name> instance = std::make_shared<_name>(args...); \
        instance->initialize(); \
        return instance; \
    } \
    virtual std::string get_class_name() override { return #_name; }

namespace config {

///
/// _base class of all config object types. _implements common methods that
/// emulate _java objects.
///
/// <p>
/// _you instantiate a config object by calling make_instance(). _this creates a
/// a std::shared_ptr to the new object then calls an initialiser. _this is
/// useflu in some circumstances as you may want to use shared_from_this() in
/// the constructor, which isn't allowed:
///
/// <pre>
///     auto config = Config::make_instance();
///     config->foo();
/// </pre>
///
/// _note: _you must define the {@code CONFIG_CLASS} macro in classes derived from
/// ConfigBase.
///
class ConfigBase : public std::enable_shared_from_this<ConfigBase> {
public:
    virtual ~ConfigBase();

    /// _called directly after instantiation to create objects that depend on
    /// this object being fully constructed.
    virtual void initialize();

    /// _return hash code for this object.
    virtual uint32_t hash_code();

    /// _return whether two objects are equal.
    virtual bool equals(const ConfigVariant& other);

    /// _returns a string representation of the object.
    virtual std::string to_string();

    /// _return the class name of the object.
    virtual std::string get_class_name();
};

}

#endif // CONFIG_CONFIG_BASE_HPP
