#include "jsonlinter.h"
#include <cmath>
#include <unordered_set>

void lintNumber(const std::shared_ptr<JsonValue>& value, std::vector<JsonLintIssue>& issues) {
    if (value->getType() == JsonValue::Type::Number) {
        double num = value->getNumber();
        if (std::isinf(num) || std::isnan(num)) {
            issues.push_back({JsonLintIssue::Severity::Error, "Invalid number (inf or nan)", -1, -1});
        }
    }
}

std::vector<JsonLintIssue> lintJson(const std::string& jsonStr) {
    std::vector<JsonLintIssue> issues;
    // Optionally: parse, catch errors, add to issues
    try {
        auto root = JsonParser::parse(jsonStr);
        auto objIssues = lintJson(root);
        issues.insert(issues.end(), objIssues.begin(), objIssues.end());
    } catch (const std::exception& e) {
        issues.push_back({JsonLintIssue::Severity::Error, e.what(), -1, -1});
    }
    return issues;
}

std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root) {
    std::vector<JsonLintIssue> issues;

    if (!root) return issues;

    lintNumber(root, issues);

    if (root->getType() == JsonValue::Type::Object) {
        std::unordered_set<std::string> seenKeys;
        const auto& obj = root->getObject();

        for (const auto& kv : obj) {
            if (!seenKeys.insert(kv.first).second) {
                issues.push_back({JsonLintIssue::Severity::Warning,
                                  "Duplicate key: " + kv.first,
                                  -1, -1});
            }

            auto childIssues = lintJson(kv.second);
            issues.insert(issues.end(), childIssues.begin(), childIssues.end());
        }
    }

    if (root->getType() == JsonValue::Type::Array) {
        for (const auto& item : root->getArray()) {
            auto childIssues = lintJson(item);
            issues.insert(issues.end(), childIssues.begin(), childIssues.end());
        }
    }

    return issues;
}
