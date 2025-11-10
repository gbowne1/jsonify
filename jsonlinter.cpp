#include "jsonlinter.h"
#include <unordered_set>
#include <cmath>

static void lintNumber(const std::shared_ptr<JsonValue>& v,
                       const std::string& src,
                       std::vector<JsonLintIssue>& issues) {
    if (v->getType() != JsonValue::Type::Number) return;
    double n = v->getNumber();
    if (std::isinf(n) || std::isnan(n)) {
        JsonParser::Pos p = JsonParser::currentPos(src,
                /* crude – we don’t store start index, but the error is obvious */
                0);
        issues.push_back({JsonLintIssue::Severity::Error,
                          "Invalid number (inf/nan)", static_cast<int>(p.line),
                          static_cast<int>(p.col)});
    }
}

static void lintRec(const std::shared_ptr<JsonValue>& node,
                    const std::string& src,
                    std::vector<JsonLintIssue>& issues) {
    if (!node) return;

    lintNumber(node, src, issues);

    if (node->getType() == JsonValue::Type::Object) {
        std::unordered_set<std::string> seen;
        for (const auto& kv : node->getObject()) {
            if (!seen.insert(kv.first).second) {
                issues.push_back({JsonLintIssue::Severity::Warning,
                                  "Duplicate key: " + kv.first, -1, -1});
            }
            lintRec(kv.second, src, issues);
        }
    } else if (node->getType() == JsonValue::Type::Array) {
        for (const auto& el : node->getArray())
            lintRec(el, src, issues);
    }
}

std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root,
                                    const std::string& source) {
    std::vector<JsonLintIssue> issues;
    lintRec(root, source, issues);
    return issues;
}
