
#include "detail/substitution_expression.hpp"
#include "detail/path.hpp"
#include "detail/variant_utils.hpp"

namespace config {

SubstitutionExpression::SubstitutionExpression(const PathPtr& path, bool optional) :
    path_(path),
    optional_(optional) {
}

PathPtr SubstitutionExpression::path() {
    return path_;
}

bool SubstitutionExpression::optional() {
    return optional_;
}

SubstitutionExpressionPtr SubstitutionExpression::change_path(const PathPtr& new_path) {
    if (new_path == path_) {
        return shared_from_this();
    } else {
        return make_instance(new_path, optional_);
    }
}

std::string SubstitutionExpression::to_string() {
    return std::string("${") + (optional_ ? "?" : "") + path_->render() + "}";
}

bool SubstitutionExpression::equals(const ConfigVariant& other) {
    if (instanceof<SubstitutionExpression>(other)) {
        auto other_exp = static_get<SubstitutionExpression>(other);
        return other_exp->path_->equals(this->path_) && other_exp->optional_ == this->optional_;
    } else {
        return false;
    }
}

uint32_t SubstitutionExpression::hash_code() {
    uint32_t hash = 41 * (41 + path_->hash_code());
    hash = 41 * (hash + (optional_ ? 1 : 0));
    return hash;
}

}
