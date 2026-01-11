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
                    std::vector<JsonLintIssue>& issues,
                    int depth = 0) {
    if (!node) return;

    if (depth > 20) {
        issues.push_back({JsonLintIssue::Severity::Warning,
                          "Nesting exceeds 20 levels (" + std::to_string(depth) + ")", -1, -1});
    }

    lintNumber(node, src, issues);

    if (node->getType() == JsonValue::Type::Object) {
        std::unordered_set<std::string> seen;
        for (const auto& kv : node->getObject()) {
            if (kv.first.empty()) {
                issues.push_back({JsonLintIssue::Severity::Warning,
                                  "Empty key detected in object.", -1, -1});
            }
            auto isCamelCase = [](const std::string& key) {
                for (size_t i = 0; i < key.size(); ++i) {
                    char c = key[i];
                    if (i == 0 && !std::islower(c)) return false;
                    if (c == '_') return false;
                    if (std::isupper(c) && i > 0 && !std::islower(key[i-1])) return false;
                }
                return true;
            };
            if (!kv.first.empty() && !isCamelCase(kv.first)) {
                issues.push_back({JsonLintIssue::Severity::Warning,
                                  "Key does not follow camelCase: " + kv.first, -1, -1});
            }
            if (!seen.insert(kv.first).second) {
                issues.push_back({JsonLintIssue::Severity::Warning,
                                  "Duplicate key: " + kv.first, -1, -1});
            }
            lintRec(kv.second, src, issues, depth + 1);
        }
    } else if (node->getType() == JsonValue::Type::Array) {
        const auto& arr = node->getArray();
        if (!arr.empty()) {
            auto firstType = arr[0] ? arr[0]->getType() : JsonValue::Type::Null;
            bool mixed = false;
            for (const auto& el : arr) {
                if (el && el->getType() != firstType) {
                    mixed = true;
                    break;
                }
            }
            if (mixed) {
                issues.push_back({JsonLintIssue::Severity::Warning,
                                  "Array contains mixed data types.", -1, -1});
            }
        }
        for (const auto& el : arr)
            lintRec(el, src, issues, depth + 1);
    }
}

std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root,
                                    const std::string& source) {
    std::vector<JsonLintIssue> issues;
    lintRec(root, source, issues, 0);
    return issues;
}
