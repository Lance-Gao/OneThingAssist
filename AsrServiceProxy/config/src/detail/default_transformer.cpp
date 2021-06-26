
#include "detail/default_transformer.hpp"
#include "detail/config_int64.hpp"
#include "detail/config_double.hpp"
#include "detail/config_null.hpp"
#include "detail/config_boolean.hpp"
#include "detail/config_string.hpp"
#include "config_value_type.hpp"

namespace config {

AbstractConfigValuePtr DefaultTransformer::transform(const AbstractConfigValuePtr& value,
        ConfigValueType requested) {
    if (value->value_type() == ConfigValueType::STRING) {
        std::string s = value->unwrapped<std::string>();

        switch (requested) {
        case ConfigValueType::NUMBER:
            try {
                int64_t v = boost::lexical_cast<int64_t>(s);
                return ConfigInt64::make_instance(value->origin(), v, s);
            } catch (boost::bad_lexical_cast&) {
                // try double
            }

            try {
                double v = boost::lexical_cast<double>(s);
                return ConfigDouble::make_instance(value->origin(), v, s);
            } catch (boost::bad_lexical_cast&) {
                // oh well
            }

            break;

        case ConfigValueType::NONE:
            if (s == "null") {
                return ConfigNull::make_instance(value->origin());
            }

            break;

        case ConfigValueType::BOOLEAN:
            if (s == "true" || s == "yes" || s == "on") {
                return ConfigBoolean::make_instance(value->origin(), true);
            } else if (s == "false" || s == "no" || s == "off") {
                return ConfigBoolean::make_instance(value->origin(), false);
            }

            break;

        case ConfigValueType::LIST:
            // can't go STRING to LIST automatically
            break;

        case ConfigValueType::OBJECT:
            // can't go STRING to OBJECT automatically
            break;

        case ConfigValueType::STRING:
            // no-op STRING to STRING
            break;
        }
    } else if (requested == ConfigValueType::STRING) {
        // if we converted null to string here, then you wouldn't properly
        // get a missing-value error if you tried to get a null value
        // as a string.
        switch (value->value_type()) {
        case ConfigValueType::NUMBER: // FALL THROOUGH
        case ConfigValueType::BOOLEAN:
            return ConfigString::make_instance(value->origin(), value->transform_to_string());

        case ConfigValueType::NONE:
            // want to be sure this throws instead of returning "null" as a
            // string
            break;

        case ConfigValueType::OBJECT:
            // no OBJECT to STRING automatically
            break;

        case ConfigValueType::LIST:
            // no LIST to STRING automatically
            break;

        case ConfigValueType::STRING:
            // no-op STRING to STRING
            break;
        }
    }

    return value;
}

}
