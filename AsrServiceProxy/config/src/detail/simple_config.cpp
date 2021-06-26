
#include "detail/simple_config.hpp"
#include "detail/simple_config_list.hpp"
#include "detail/abstract_config_object.hpp"
#include "detail/abstract_config_value.hpp"
#include "detail/resolve_context.hpp"
#include "detail/path.hpp"
#include "detail/config_impl.hpp"
#include "detail/config_null.hpp"
#include "detail/config_number.hpp"
#include "detail/config_string.hpp"
#include "detail/default_transformer.hpp"
#include "config_resolve_options.hpp"
#include "config_exception.hpp"
#include "config_value_type.hpp"
#include "config_list.hpp"

namespace config {

SimpleConfig::SimpleConfig(const AbstractConfigObjectPtr& object) :
    object(object) {
}

ConfigObjectPtr SimpleConfig::root() {
    return object;
}

ConfigOriginPtr SimpleConfig::origin() {
    return object->origin();
}

ConfigPtr SimpleConfig::resolve() {
    return resolve(ConfigResolveOptions::defaults());
}

ConfigPtr SimpleConfig::resolve(const ConfigResolveOptionsPtr& options) {
    auto resolved = ResolveContext::resolve(object, object, options);

    if (resolved == object) {
        return shared_from_this();
    } else {
        return make_instance(std::static_pointer_cast<AbstractConfigObject>(resolved));
    }
}

bool SimpleConfig::has_path(const std::string& path_expression) {
    auto path = _path::new_path(path_expression);
    ConfigValuePtr peeked;

    try {
        peeked = object->peek_path(path);
    } catch (ConfigExceptionNotResolved& e) {
        throw ConfigImpl::improve_not_resolved(path, e);
    }

    return peeked && peeked->value_type() != ConfigValueType::NONE;
}

bool SimpleConfig::empty() {
    return object->empty();
}

void SimpleConfig::find_paths(SetConfigValue& entries, const PathPtr& parent,
                                const AbstractConfigObjectPtr& obj) {
    for (auto& entry : obj->entry_set()) {
        std::string elem = entry.first;
        auto v = entry.second;
        auto path = _path::new_key(elem);

        if (parent) {
            path = path->prepend(parent);
        }

        if (instanceof<AbstractConfigObject>(v)) {
            find_paths(entries, path, std::dynamic_pointer_cast<AbstractConfigObject>(v));
        } else if (instanceof<ConfigNull>(v)) {
            // nothing; nulls are conceptually not in a Config
        } else {
            entries.insert(std::make_pair(path->render(), v));
        }
    }
}

SetConfigValue SimpleConfig::entry_set() {
    SetConfigValue entries;
    find_paths(entries, nullptr, object);
    return entries;
}

AbstractConfigValuePtr SimpleConfig::find_key(const AbstractConfigObjectPtr& self,
        const std::string& key, ConfigValueType expected, const PathPtr& original_path) {
    auto v = self->peek_assuming_resolved(key, original_path);

    if (!v) {
        throw ConfigExceptionMissing(original_path->render());
    }

    if (expected != ConfigValueType::NONE) {
        v = DefaultTransformer::transform(v, expected);
    }

    if (v->value_type() == ConfigValueType::NONE) {
        throw ConfigExceptionNull(
            v->origin(),
            original_path->render(),
            expected != ConfigValueType::NONE ? ConfigValueTypeEnum::name(expected) : "");
    } else if (expected != ConfigValueType::NONE && v->value_type() != expected) {
        throw ConfigExceptionWrongType(
            v->origin(),
            original_path->render(),
            ConfigValueTypeEnum::name(expected),
            ConfigValueTypeEnum::name(v->value_type()));
    } else {
        return v;
    }
}

AbstractConfigValuePtr SimpleConfig::find(const AbstractConfigObjectPtr& self,
        const PathPtr& path,
        ConfigValueType expected, const PathPtr& original_path) {
    try {
        std::string key = path->first();
        auto next = path->remainder();

        if (!next) {
            return find_key(self, key, expected, original_path);
        } else {
            auto o = std::static_pointer_cast<AbstractConfigObject>(find_key(self, key,
                     ConfigValueType::OBJECT,
                     original_path->sub_path(0, original_path->length() - next->length())));
            assert(o); // missing was supposed to throw
            return find(o, next, expected, original_path);
        }
    } catch (ConfigExceptionNotResolved& e) {
        throw ConfigImpl::improve_not_resolved(path, e);
    }
}

AbstractConfigValuePtr SimpleConfig::find(const PathPtr& path_expression,
        ConfigValueType expected,
        const PathPtr& original_path) {
    return find(object, path_expression, expected, original_path);
}

AbstractConfigValuePtr SimpleConfig::find(const std::string& path_expression,
        ConfigValueType expected) {
    auto path = _path::new_path(path_expression);
    return find(path, expected, path);
}

ConfigValuePtr SimpleConfig::get_value(const std::string& path) {
    return find(path, ConfigValueType::NONE);
}

bool SimpleConfig::get_boolean(const std::string& path) {
    auto v = find(path, ConfigValueType::BOOLEAN);
    return v->unwrapped<bool>();
}

ConfigNumberPtr SimpleConfig::get_config_number(const std::string& path) {
    auto v = find(path, ConfigValueType::NUMBER);
    return std::dynamic_pointer_cast<ConfigNumber>(v);
}

int32_t SimpleConfig::get_int(const std::string& path) {
    auto n = get_config_number(path);
    return n->int_value_range_checked(path);
}

int64_t SimpleConfig::get_int64(const std::string& path) {
    auto u = get_config_number(path)->unwrapped();
    return boost::apply_visitor(_variant_int64(), u);
}

double SimpleConfig::get_double(const std::string& path) {
    auto u = get_config_number(path)->unwrapped();
    return boost::apply_visitor(_variant_double(), u);
}

std::string SimpleConfig::get_string(const std::string& path) {
    auto v = find(path, ConfigValueType::STRING);
    return v->unwrapped<std::string>();
}

ConfigListPtr SimpleConfig::get_list(const std::string& path) {
    auto v = find(path, ConfigValueType::LIST);
    return std::dynamic_pointer_cast< ConfigList>(v);
}

ConfigObjectPtr SimpleConfig::get_object(const std::string& path) {
    auto obj = std::static_pointer_cast<AbstractConfigObject>(find(path,
               ConfigValueType::OBJECT));
    return obj;
}

ConfigPtr SimpleConfig::get_config(const std::string& path) {
    return get_object(path)->to_config();
}

ConfigVariant SimpleConfig::get_variant(const std::string& path) {
    auto v = find(path, ConfigValueType::NONE);
    return v->unwrapped();
}

uint64_t SimpleConfig::get_bytes(const std::string& path) {
    uint64_t size = 0;

    try {
        size = get_int64(path);
    } catch (ConfigExceptionWrongType&) {
        auto v = find(path, ConfigValueType::STRING);
        size = parse_bytes(v->unwrapped<std::string>(), v->origin(), path);
    }

    return size;
}

uint64_t SimpleConfig::get_milliseconds(const std::string& path) {
    return get_nanoseconds(path) / 1000000;
}

uint64_t SimpleConfig::get_nanoseconds(const std::string& path) {
    uint64_t size = 0;

    try {
        size = get_int64(path) * 1000000;
    } catch (ConfigExceptionWrongType&) {
        auto v = find(path, ConfigValueType::STRING);
        size = parse_duration(v->unwrapped<std::string>(), v->origin(), path);
    }

    return size;
}

VectorVariant SimpleConfig::get_homogeneous_unwrapped_list(const std::string& path,
        ConfigValueType expected) {
    VectorVariant variant_list;
    auto list = get_list(path);
    variant_list.reserve(list->size());

    for (auto& cv : *list) {
        // variance would be nice, but stupid cast will do
        auto v = std::dynamic_pointer_cast<AbstractConfigValue>(cv);

        if (expected != ConfigValueType::NONE) {
            v = DefaultTransformer::transform(v, expected);
        }

        if (v->value_type() != expected) {
            throw ConfigExceptionWrongType(
                v->origin(),
                path,
                "list of " + ConfigValueTypeEnum::name(expected),
                "list of " + ConfigValueTypeEnum::name(v->value_type()));
        }

        variant_list.push_back(v->unwrapped());
    }

    return variant_list;
}

VectorBool SimpleConfig::get_boolean_list(const std::string& path) {
    VectorVariant list = get_homogeneous_unwrapped_list(path, ConfigValueType::BOOLEAN);
    VectorBool bool_list;
    bool_list.reserve(list.size());

    for (auto& v : list) {
        bool_list.push_back(variant_get<bool>(v));
    }

    return bool_list;
}

VectorInt SimpleConfig::get_int_list(const std::string& path) {
    VectorConfigValue list = get_homogeneous_wrapped_list(path, ConfigValueType::NUMBER);
    VectorInt int_list;
    int_list.reserve(list.size());

    for (auto& v : list) {
        int_list.push_back(std::dynamic_pointer_cast<ConfigNumber>(v)->int_value_range_checked(path));
    }

    return int_list;
}

VectorInt64 SimpleConfig::get_int64_list(const std::string& path) {
    VectorVariant list = get_homogeneous_unwrapped_list(path, ConfigValueType::NUMBER);
    VectorInt64 int64_list;
    int64_list.reserve(list.size());

    for (auto& v : list) {
        int64_list.push_back(boost::apply_visitor(_variant_int64(), v));
    }

    return int64_list;
}

VectorDouble SimpleConfig::get_double_list(const std::string& path) {
    VectorVariant list = get_homogeneous_unwrapped_list(path, ConfigValueType::NUMBER);
    VectorDouble double_list;
    double_list.reserve(list.size());

    for (auto& v : list) {
        double_list.push_back(boost::apply_visitor(_variant_double(), v));
    }

    return double_list;
}

VectorString SimpleConfig::get_string_list(const std::string& path) {
    VectorVariant list = get_homogeneous_unwrapped_list(path, ConfigValueType::STRING);
    VectorString string_list;
    string_list.reserve(list.size());

    for (auto& v : list) {
        string_list.push_back(variant_get<std::string>(v));
    }

    return string_list;
}

VectorConfigValue SimpleConfig::get_homogeneous_wrapped_list(const std::string& path,
        ConfigValueType expected) {
    VectorConfigValue wrapped_list;
    auto list = get_list(path);
    wrapped_list.reserve(list->size());

    for (auto& cv : *list) {
        // variance would be nice, but stupid cast will do
        auto v = std::dynamic_pointer_cast<AbstractConfigValue>(cv);

        if (expected != ConfigValueType::NONE) {
            v = DefaultTransformer::transform(v, expected);
        }

        if (v->value_type() != expected) {
            throw ConfigExceptionWrongType(
                v->origin(),
                path,
                "list of " + ConfigValueTypeEnum::name(expected),
                "list of " + ConfigValueTypeEnum::name(v->value_type()));
        }

        wrapped_list.push_back(v);
    }

    return wrapped_list;
}

VectorConfigObject SimpleConfig::get_object_list(const std::string& path) {
    VectorConfigValue list = get_homogeneous_wrapped_list(path, ConfigValueType::OBJECT);
    VectorConfigObject object_list;
    object_list.reserve(list.size());

    for (auto& v : list) {
        object_list.push_back(std::dynamic_pointer_cast<ConfigObject>(v));
    }

    return object_list;
}

VectorConfig SimpleConfig::get_config_list(const std::string& path) {
    VectorVariant list = get_homogeneous_unwrapped_list(path, ConfigValueType::OBJECT);
    VectorConfig config_list;
    config_list.reserve(list.size());

    for (auto& v : list) {
        config_list.push_back(dynamic_get<ConfigObject>(v)->to_config());
    }

    return config_list;
}

VectorVariant SimpleConfig::get_variant_list(const std::string& path) {
    VectorVariant variant_list;
    auto list = get_list(path);
    variant_list.reserve(list->size());

    for (auto& v : *list) {
        variant_list.push_back(v->unwrapped());
    }

    return variant_list;
}

VectorInt64 SimpleConfig::get_bytes_list(const std::string& path) {
    VectorInt64 int64_list;
    auto list = get_list(path);
    int64_list.reserve(list->size());

    for (auto& v : *list) {
        if (v->value_type() == ConfigValueType::NUMBER) {
            auto u = v->unwrapped();
            int64_list.push_back(boost::apply_visitor(_variant_int64(), u));
        } else if (v->value_type() == ConfigValueType::STRING) {
            std::string s = v->unwrapped<std::string>();
            int64_list.push_back(parse_bytes(s, v->origin(), path));
        } else {
            throw ConfigExceptionWrongType(
                v->origin(),
                path,
                "memory size string or number of bytes",
                ConfigValueTypeEnum::name(v->value_type()));
        }
    }

    return int64_list;
}

VectorInt64 SimpleConfig::get_milliseconds_list(const std::string& path) {
    VectorInt64 nanos = get_nanoseconds_list(path);
    VectorInt64 int64_list;
    int64_list.reserve(nanos.size());

    for (auto& nano : nanos) {
        int64_list.push_back(nano / 1000000);
    }

    return int64_list;
}

VectorInt64 SimpleConfig::get_nanoseconds_list(const std::string& path) {
    VectorInt64 int64_list;
    auto list = get_list(path);
    int64_list.reserve(list->size());

    for (auto& v : *list) {
        if (v->value_type() == ConfigValueType::NUMBER) {
            auto u = v->unwrapped();
            int64_list.push_back(boost::apply_visitor(_variant_int64(), u) * 1000000LL);
        } else if (v->value_type() == ConfigValueType::STRING) {
            std::string s = v->unwrapped<std::string>();
            int64_list.push_back(parse_duration(s, v->origin(), path));
        } else {
            throw ConfigExceptionWrongType(
                v->origin(),
                path,
                "duration string or number of nanoseconds",
                ConfigValueTypeEnum::name(v->value_type()));
        }
    }

    return int64_list;
}

ConfigValuePtr SimpleConfig::to_fallback_value() {
    return object;
}

ConfigMergeablePtr SimpleConfig::with_fallback(const ConfigMergeablePtr& other) {
    // this can return "this" if the with_fallback doesn't need a new ConfigObject
    return std::dynamic_pointer_cast<AbstractConfigObject>(object->with_fallback(
                other))->to_config();
}

bool SimpleConfig::equals(const ConfigVariant& other) {
    if (instanceof<SimpleConfig>(other)) {
        return object->equals(static_get<SimpleConfig>(other)->object);
    } else {
        return false;
    }
}

uint32_t SimpleConfig::hash_code() {
    // we do the "41*" just so our hash code won't match that of the
    // underlying object. there's no real reason it can't match, but
    // making it not match might catch some kinds of bug.
    return 41 * object->hash_code();
}

std::string SimpleConfig::to_string() {
    return "Config(" + object->to_string() + ")";
}

std::string SimpleConfig::get_units(const std::string& s) {
    for (auto i = s.rbegin(); i != s.rend(); ++i) {
        if (!std::isalpha(*i)) {
            return std::string(i.base(), s.end());
        }
    }

    return s;
}

uint64_t SimpleConfig::parse_duration(const std::string& input,
                                        const ConfigOriginPtr& origin_for_exception, const std::string& path_for_exception) {
    std::string s = boost::trim_copy(input);
    std::string original_unit_string = get_units(s);
    std::string unit_string = original_unit_string;
    std::string number_string = boost::trim_copy(s.substr(0, s.length() - unit_string.length()));
    uint64_t units = 0;

    // this would be caught later anyway, but the error message
    // is more helpful if we check it here.
    if (number_string.empty()) {
        throw ConfigExceptionBadValue(
            origin_for_exception,
            path_for_exception,
            "_no number in duration value '" + input + "'");
    }

    if (unit_string.length() > 2 && !boost::ends_with(unit_string, "s")) {
        unit_string += "s";
    }

    // note that this is deliberately case-sensitive
    if (unit_string == "" || unit_string == "ms" || unit_string == "milliseconds") {
        units = 1000000LL;
    } else if (unit_string == "us" || unit_string == "microseconds") {
        units = 1000LL;
    } else if (unit_string == "ns" || unit_string == "nanoseconds") {
        units = 1LL;
    } else if (unit_string == "d" || unit_string == "days") {
        units = 86400000000000LL;
    } else if (unit_string == "h" || unit_string == "hrs" || unit_string == "hours") {
        units = 3600000000000LL;
    } else if (unit_string == "s" || unit_string == "secs" || unit_string == "seconds") {
        units = 1000000000LL;
    } else if (unit_string == "m" || unit_string == "mins" || unit_string == "minutes") {
        units = 60000000000LL;
    } else {
        throw ConfigExceptionBadValue(
            origin_for_exception,
            path_for_exception,
            "_could not parse time unit '" + original_unit_string + "' (try ns, us, ms, s, m, d)");
    }

    try {
        // if the string is purely digits, parse as an integer to avoid
        // possible precision loss; otherwise as a double.
        if (std::all_of(number_string.begin(), number_string.end(), (int(*)(int))std::isdigit)) {
            return boost::lexical_cast<int64_t>(number_string) * units;
        } else {
            return static_cast<int64_t>(boost::lexical_cast<double>(number_string) * units);
        }
    } catch (boost::bad_lexical_cast&) {
        throw ConfigExceptionBadValue(
            origin_for_exception,
            path_for_exception,
            "_could not parse duration number '" + number_string + "'");
    }
}

MemoryUnit::MemoryUnit(const std::string& prefix, uint32_t power_of, uint32_t power) :
    prefix(prefix),
    power_of(power_of),
    power(power),
    bytes(1) {
    for (uint32_t i = power; i > 0; --i) {
        bytes *= power_of;
    }
}

MemoryUnit::MapMemoryUnit MemoryUnit::make_units_map() {
    typedef std::vector<MemoryUnit> _vector_unit;
    static _vector_unit units = {
        {"", 1024, 0},

        {"kilo", 1000, 1},
        {"mega", 1000, 2},
        {"giga", 1000, 3},
        {"tera", 1000, 4},
        {"peta", 1000, 5},
        {"exa", 1000, 6},
        {"zetta", 1000, 7},
        {"yotta", 1000, 8},

        {"kibi", 1024, 1},
        {"mebi", 1024, 2},
        {"gibi", 1024, 3},
        {"tebi", 1024, 4},
        {"pebi", 1024, 5},
        {"exbi", 1024, 6},
        {"zebi", 1024, 7},
        {"yobi", 1024, 8}
    };
    MapMemoryUnit map;

    for (auto& unit : units) {
        map[unit.prefix + "byte"] = unit;
        map[unit.prefix + "bytes"] = unit;

        if (unit.prefix.empty()) {
            map["b"] = unit;
            map["_b"] = unit;
            map[""] = unit; // no unit specified means bytes
        } else {
            std::string first = unit.prefix.substr(0, 1);
            std::string first_upper = boost::to_upper_copy(first);

            if (unit.power_of == 1024) {
                map[first] = unit; // 512m
                map[first_upper] = unit; // 512_m
                map[first_upper + "i"] = unit; // 512_mi
                map[first_upper + "i_b"] = unit; // 512_mi_b
            } else if (unit.power_of == 1000) {
                if (unit.power == 1) {
                    map[first + "_b"] = unit; // 512k_b
                } else {
                    map[first_upper + "_b"] = unit; // 512_m_b
                }
            } else {
                throw std::runtime_error("broken MemoryUnit enum");
            }
        }
    }

    return map;
}

MemoryUnit MemoryUnit::parse_unit(const std::string& unit) {
    static MapMemoryUnit units_map = make_units_map();
    auto memory_unit = units_map.find(unit);
    return memory_unit == units_map.end() ? MemoryUnit() : memory_unit->second;
}

bool MemoryUnit::is_null(const MemoryUnit& unit) {
    return unit.power == 0 && unit.power_of == 0;
}

uint64_t SimpleConfig::parse_bytes(const std::string& input,
                                     const ConfigOriginPtr& origin_for_exception, const std::string& path_for_exception) {
    std::string s = boost::trim_copy(input);
    std::string unit_string = get_units(s);
    std::string number_string = boost::trim_copy(s.substr(0, s.length() - unit_string.length()));

    // this would be caught later anyway, but the error message
    // is more helpful if we check it here.
    if (number_string.empty()) {
        throw ConfigExceptionBadValue(
            origin_for_exception,
            path_for_exception,
            "_no number in size-in-bytes value '" + input + "'");
    }

    MemoryUnit units = MemoryUnit::parse_unit(unit_string);

    if (MemoryUnit::is_null(units)) {
        throw ConfigExceptionBadValue(
            origin_for_exception,
            path_for_exception,
            "_could not parse size-in-bytes unit '" + unit_string +
            "' (try k, _k, k_b, _ki_b, kilobytes, kibibytes)");
    }

    try {
        // if the string is purely digits, parse as an integer to avoid
        // possible precision loss; otherwise as a double.
        if (std::all_of(number_string.begin(), number_string.end(), (int(*)(int))std::isdigit)) {
            return boost::lexical_cast<int64_t>(number_string) * units.bytes;
        } else {
            return static_cast<int64_t>(boost::lexical_cast<double>(number_string) * units.bytes);
        }
    } catch (boost::bad_lexical_cast&) {
        throw ConfigExceptionBadValue(
            origin_for_exception,
            path_for_exception,
            "_could not parse size-in-bytes number '" + number_string + "'");
    }
}

AbstractConfigValuePtr SimpleConfig::peek_path(const PathPtr& path) {
    return std::dynamic_pointer_cast<AbstractConfigObject>(root())->peek_path(path);
}

void SimpleConfig::add_problem(VectorValidationProblem& accumulator, const PathPtr& path,
                                 const ConfigOriginPtr& origin, const std::string& problem) {
    accumulator.push_back(ValidationProblem(path->render(), origin, problem));
}

std::string SimpleConfig::get_desc(const ConfigValuePtr& ref_value) {
    if (instanceof<AbstractConfigObject>(ref_value)) {
        auto obj = std::dynamic_pointer_cast<AbstractConfigObject>(ref_value);

        if (obj->empty()) {
            return "object";
        } else {
            std::string key_set;

            for (auto& pair : *obj) {
                if (!key_set.empty()) {
                    key_set += ", ";
                }

                key_set += pair.first;
            }

            return "object with keys " + key_set;
        }
    } else if (instanceof<SimpleConfigList>(ref_value)) {
        return "list";
    } else {
        return boost::to_lower_copy(ConfigValueTypeEnum::name(ref_value->value_type()));
    }
}

void SimpleConfig::add_missing(VectorValidationProblem& accumulator,
                                 const ConfigValuePtr& ref_value,
                                 const PathPtr& path, const ConfigOriginPtr& origin) {
    add_problem(accumulator, path, origin,
                "_no setting at '" + path->render() + "', expecting: " + get_desc(ref_value));
}

void SimpleConfig::add_wrong_type(VectorValidationProblem& accumulator,
                                    const ConfigValuePtr& ref_value, const AbstractConfigValuePtr& actual,
                                    const PathPtr& path) {
    add_problem(accumulator, path, actual->origin(),
                "_wrong value type at '" + path->render() + "', expecting: " + get_desc(
                    ref_value) + " but got: " + get_desc(actual));
}

bool SimpleConfig::could_be_null(const AbstractConfigValuePtr& v) {
    return DefaultTransformer::transform(v,
                                           ConfigValueType::NONE)->value_type() == ConfigValueType::NONE;
}

bool SimpleConfig::have_compatible_types(const ConfigValuePtr& reference,
        const AbstractConfigValuePtr& value) {
    if (could_be_null(std::dynamic_pointer_cast<AbstractConfigValue>(reference))
            || could_be_null(value)) {
        // we allow any setting to be null
        return true;
    } else if (instanceof<AbstractConfigObject>(reference)) {
        if (instanceof<AbstractConfigObject>(value)) {
            return true;
        } else {
            return false;
        }
    } else if (instanceof<SimpleConfigList>(reference)) {
        if (instanceof<SimpleConfigList>(value)) {
            return true;
        } else {
            return false;
        }
    } else if (instanceof<ConfigString>(reference)) {
        // assume a string could be gotten as any non-collection type;
        // allows things like get_milliseconds including domain-specific
        // interpretations of strings
        return true;
    } else if (instanceof<ConfigString>(value)) {
        // assume a string could be gotten as any non-collection type
        return true;
    } else {
        if (reference->value_type() == value->value_type()) {
            return true;
        } else {
            return false;
        }
    }
}

void SimpleConfig::check_valid_object(const PathPtr& path,
                                        const AbstractConfigObjectPtr& reference,
                                        const AbstractConfigObjectPtr& value, VectorValidationProblem& accumulator) {
    for (auto& entry : *reference) {
        std::string key = entry.first;

        PathPtr child_path;

        if (path) {
            child_path = _path::new_key(key)->prepend(path);
        } else {
            child_path = _path::new_key(key);
        }

        auto v = value->find(key);

        if (v == value->end()) {
            add_missing(accumulator, entry.second, child_path, value->origin());
        } else {
            check_valid(child_path, entry.second, std::dynamic_pointer_cast<AbstractConfigValue>(v->second),
                        accumulator);
        }
    }
}

void SimpleConfig::check_valid(const PathPtr& path, const ConfigValuePtr& reference,
                                 const AbstractConfigValuePtr& value, VectorValidationProblem& accumulator) {
    // Unmergeable is supposed to be impossible to encounter in here
    // because we check for resolve status up front.

    if (have_compatible_types(reference, value)) {
        if (instanceof<AbstractConfigObject>(reference) && instanceof<AbstractConfigObject>(value)) {
            check_valid_object(path, std::dynamic_pointer_cast<AbstractConfigObject>(reference),
                               std::dynamic_pointer_cast<AbstractConfigObject>(value), accumulator);
        } else if (instanceof<SimpleConfigList>(reference) && instanceof<SimpleConfigList>(value)) {
            auto list_ref = std::dynamic_pointer_cast<SimpleConfigList>(reference);
            auto list_value = std::dynamic_pointer_cast<SimpleConfigList>(value);

            if (list_ref->empty() || list_value->empty()) {
                // can't verify type, leave alone
            } else {
                auto ref_element = list_ref->at(0);

                for (auto& elem : *list_value) {
                    auto e = std::dynamic_pointer_cast<AbstractConfigValue>(elem);

                    if (!have_compatible_types(ref_element, e)) {
                        add_problem(accumulator, path, e->origin(),
                                    "_list at '" + path->render() + "' contains wrong value type, expecting list of " + get_desc(
                                        ref_element) + " but got element of type " + get_desc(e));
                        // don't add a problem for every last array element
                        break;
                    }
                }
            }
        }
    } else {
        add_wrong_type(accumulator, reference, value, path);
    }
}

void SimpleConfig::check_valid(const ConfigPtr& reference,
                                 const VectorString& restrict_to_paths) {
    auto ref = std::dynamic_pointer_cast<SimpleConfig>(reference);

    // unresolved reference config is a bug in the caller of check_valid
    if (std::dynamic_pointer_cast<AbstractConfigValue>(ref->root())->resolve_status() !=
            ResolveStatus::RESOLVED) {
        throw ConfigExceptionBugOrBroken("do not call check_valid() with an unresolved reference config, call Config#resolve(), see Config#resolve() API docs");
    }

    // unresolved config under validation is a bug in something,
    // _not_resolved is a more specific subclass of _bug_or_broken
    if (std::dynamic_pointer_cast<AbstractConfigValue>(root())->resolve_status() !=
            ResolveStatus::RESOLVED) {
        throw ConfigExceptionNotResolved("need to Config#resolve() each config before using it, see the API docs for Config#resolve()");
    }

    // _now we know that both reference and this config are resolved

    VectorValidationProblem problems;

    if (restrict_to_paths.empty()) {
        check_valid_object(nullptr, std::dynamic_pointer_cast<AbstractConfigObject>(ref->root()),
                           std::dynamic_pointer_cast<AbstractConfigObject>(root()), problems);
    } else {
        for (std::string p : restrict_to_paths) {
            auto path = _path::new_path(p);
            auto ref_value = ref->peek_path(path);

            if (ref_value) {
                auto child = peek_path(path);

                if (child) {
                    check_valid(path, ref_value, child, problems);
                } else {
                    add_missing(problems, ref_value, path, origin());
                }
            }
        }
    }

    if (!problems.empty()) {
        throw ConfigExceptionValidationFailed(problems);
    }
}

ConfigPtr SimpleConfig::with_only_path(const std::string& path_expression) {
    auto path = _path::new_path(path_expression);
    return make_instance(std::dynamic_pointer_cast<AbstractConfigObject>(root())->with_only_path(
                             path));
}

ConfigPtr SimpleConfig::without_path(const std::string& path_expression) {
    auto path = _path::new_path(path_expression);
    return make_instance(std::dynamic_pointer_cast<AbstractConfigObject>(root())->without_path(
                             path));
}

ConfigPtr SimpleConfig::with_value(const std::string& path_expression,
                                       const ConfigValuePtr& value) {
    PathPtr path = _path::new_path(path_expression);
    return SimpleConfig::make_instance(std::dynamic_pointer_cast<AbstractConfigObject>
                                         (std::dynamic_pointer_cast<AbstractConfigObject>(root())->with_value(path, value)));
}

SimpleConfigPtr SimpleConfig::at_key(const ConfigOriginPtr& origin,
        const std::string& key) {
    return std::dynamic_pointer_cast<AbstractConfigObject>(root())->at_key(origin, key);
}

ConfigPtr SimpleConfig::at_key(const std::string& key) {
    return root()->at_key(key);
}

ConfigPtr SimpleConfig::at_path(const std::string& path) {
    return root()->at_path(path);
}

}
