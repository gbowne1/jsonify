#include "jsonlinter.h"
#include <unordered_set>

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
    // Implement checks here:
    // - Duplicate keys
    // - Bad values
    // - (etc)
    // Traverse the tree recursively if needed
    return issues;
}
