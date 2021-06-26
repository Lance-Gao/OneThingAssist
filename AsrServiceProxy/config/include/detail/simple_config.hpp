
#ifndef CONFIG_SIMPLE_CONFIG_HPP
#define CONFIG_SIMPLE_CONFIG_HPP

#include "detail/config_base.hpp"
#include "detail/mergeable_value.hpp"
#include "config.hpp"

namespace config {

///
/// _one thing to keep in mind in the future: as _collection-like APIs are added
/// here, including iterators or size() or anything, they should be consistent
/// with a one-level std::unordered_map from paths to non-null values. _null
/// values are not "in" the map.
///
class SimpleConfig : public virtual Config, public virtual MergeableValue,
    public ConfigBase {
public:
    CONFIG_CLASS(SimpleConfig);

    SimpleConfig(const AbstractConfigObjectPtr& object);

    virtual ConfigObjectPtr root() override;
    virtual ConfigOriginPtr origin() override;
    virtual ConfigPtr resolve() override;
    virtual ConfigPtr resolve(const ConfigResolveOptionsPtr& options) override;
    virtual bool has_path(const std::string& path) override;
    virtual bool empty() override;

private:
    static void find_paths(SetConfigValue& entries,
                           const PathPtr& parent,
                           const AbstractConfigObjectPtr& obj);

public:
    virtual SetConfigValue entry_set() override;

private:
    static AbstractConfigValuePtr find_key(const AbstractConfigObjectPtr& self,
            const std::string& key,
            ConfigValueType expected,
            const PathPtr& original_path);
    static AbstractConfigValuePtr find(const AbstractConfigObjectPtr& self,
                                           const PathPtr& path,
                                           ConfigValueType expected,
                                           const PathPtr& original_path);

    AbstractConfigValuePtr find(const PathPtr& path_expression,
                                    ConfigValueType expected,
                                    const PathPtr& original_path);
    AbstractConfigValuePtr find(const std::string& path,
                                    ConfigValueType expected);

public:
    virtual ConfigValuePtr get_value(const std::string& path) override;
    virtual bool get_boolean(const std::string& path) override;
    virtual ConfigNumberPtr get_config_number(const std::string& path);
    virtual int32_t get_int(const std::string& path) override;
    virtual int64_t get_int64(const std::string& path) override;
    virtual double get_double(const std::string& path) override;
    virtual std::string get_string(const std::string& path) override;
    virtual ConfigListPtr get_list(const std::string& path) override;
    virtual ConfigObjectPtr get_object(const std::string& path) override;
    virtual ConfigPtr get_config(const std::string& path) override;
    virtual ConfigVariant get_variant(const std::string& path) override;
    virtual uint64_t get_bytes(const std::string& path) override;
    virtual uint64_t get_milliseconds(const std::string& path) override;
    virtual uint64_t get_nanoseconds(const std::string& path) override;

private:
    VectorVariant get_homogeneous_unwrapped_list(const std::string& path,
            ConfigValueType expected);

public:
    virtual VectorBool get_boolean_list(const std::string& path) override;
    virtual VectorInt get_int_list(const std::string& path) override;
    virtual VectorInt64 get_int64_list(const std::string& path) override;
    virtual VectorDouble get_double_list(const std::string& path) override;
    virtual VectorString get_string_list(const std::string& path) override;

private:
    VectorConfigValue get_homogeneous_wrapped_list(const std::string& path,
            ConfigValueType expected);

public:
    virtual VectorConfigObject get_object_list(const std::string& path) override;
    virtual VectorConfig get_config_list(const std::string& path) override;
    virtual VectorVariant get_variant_list(const std::string& path) override;
    virtual VectorInt64 get_bytes_list(const std::string& path) override;
    virtual VectorInt64 get_milliseconds_list(const std::string& path) override;
    virtual VectorInt64 get_nanoseconds_list(const std::string& path) override;

    virtual ConfigValuePtr to_fallback_value() override;
    virtual ConfigMergeablePtr with_fallback(const ConfigMergeablePtr& other) override;

    virtual bool equals(const ConfigVariant& other) override;
    virtual uint32_t hash_code() override;
    virtual std::string to_string() override;

private:
    static std::string get_units(const std::string& s);

public:
    /// _parses a duration string. _if no units are specified in the string, it is
    /// assumed to be in milliseconds. _the returned duration is in nanoseconds.
    /// _the purpose of this function is to implement the duration-related methods
    /// in the ConfigObject interface.
    ///
    /// @param input
    ///            the string to parse
    /// @param origin_for_exception
    ///            origin of the value being parsed
    /// @param path_for_exception
    ///            path to include in exceptions
    /// @return duration in nanoseconds
    /// @throws ConfigException
    ///             if string is invalid
    static uint64_t parse_duration(const std::string& input,
                                   const ConfigOriginPtr& origin_for_exception,
                                   const std::string& path_for_exception);

    /// _parses a size-in-bytes string. _if no units are specified in the string,
    /// it is assumed to be in bytes. _the returned value is in bytes. _the purpose
    /// of this function is to implement the size-in-bytes-related methods in the
    /// Config interface.
    ///
    /// @param input
    ///            the string to parse
    /// @param origin_for_exception
    ///            origin of the value being parsed
    /// @param path_for_exception
    ///            path to include in exceptions
    /// @return size in bytes
    /// @throws ConfigException
    ///             if string is invalid
    static uint64_t parse_bytes(const std::string& input,
                                const ConfigOriginPtr& origin_for_exception,
                                const std::string& path_for_exception);

private:
    virtual AbstractConfigValuePtr peek_path(const PathPtr& path);

    static void add_problem(VectorValidationProblem& accumulator,
                            const PathPtr& path,
                            const ConfigOriginPtr& origin,
                            const std::string& problem);

    static std::string get_desc(const ConfigValuePtr& ref_value);

    static void add_missing(VectorValidationProblem& accumulator,
                            const ConfigValuePtr& ref_value,
                            const PathPtr& path,
                            const ConfigOriginPtr& origin);
    static void add_wrong_type(VectorValidationProblem& accumulator,
                               const ConfigValuePtr& ref_value,
                               const AbstractConfigValuePtr& actual,
                               const PathPtr& path);

    static bool could_be_null(const AbstractConfigValuePtr& v);

    static bool have_compatible_types(const ConfigValuePtr& reference,
                                      const AbstractConfigValuePtr& value);

    static void check_valid_object(const PathPtr& path,
                                   const AbstractConfigObjectPtr& reference,
                                   const AbstractConfigObjectPtr& value,
                                   VectorValidationProblem& accumulator);
    static void check_valid(const PathPtr& path,
                            const ConfigValuePtr& reference,
                            const AbstractConfigValuePtr& value,
                            VectorValidationProblem& accumulator);

    virtual void check_valid(const ConfigPtr& reference,
                             const VectorString& restrict_to_paths = VectorString()) override;

    virtual ConfigPtr with_only_path(const std::string& path) override;
    virtual ConfigPtr without_path(const std::string& path) override;

public:
    virtual ConfigPtr with_value(const std::string& path, const ConfigValuePtr& value) override;

    SimpleConfigPtr at_key(const ConfigOriginPtr& origin, const std::string& key);

    virtual ConfigPtr at_key(const std::string& key) override;

    virtual ConfigPtr at_path(const std::string& path) override;

private:
    AbstractConfigObjectPtr object;
};

class MemoryUnit {
public:
    MemoryUnit(const std::string& prefix = "", uint32_t power_of = 0, uint32_t power = 0);

private:
    typedef std::unordered_map<std::string, MemoryUnit> MapMemoryUnit;
    static MapMemoryUnit make_units_map();

public:
    static MemoryUnit parse_unit(const std::string& unit);
    static bool is_null(const MemoryUnit& unit);

public:
    std::string prefix;
    uint32_t power_of;
    uint32_t power;
    uint64_t bytes;
};

}

#endif // CONFIG_SIMPLE_CONFIG_HPP

