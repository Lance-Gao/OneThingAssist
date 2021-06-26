
#include "detail/simple_include_context.hpp"
#include "detail/parseable.hpp"
#include "detail/simple_includer.hpp"

namespace config {

SimpleIncludeContext::SimpleIncludeContext(const ParseablePtr& parseable) :
    parseable(parseable) {
}

SimpleIncluderContextPtr SimpleIncludeContext::with_parseable(const ParseablePtr&
        parseable) {
    if (parseable == this->parseable) {
        return shared_from_this();
    } else {
        return make_instance(parseable);
    }
}

ConfigParseablePtr SimpleIncludeContext::relative_to(const std::string& filename) {
    if (parseable) {
        return parseable->relative_to(filename);
    } else {
        return nullptr;
    }
}

ConfigParseOptionsPtr SimpleIncludeContext::parse_options() {
    return SimpleIncluder::clear_for_include(parseable->options());
}

}
