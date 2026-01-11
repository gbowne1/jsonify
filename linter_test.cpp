#include "jsonparser.h"
#include "jsonlinter.h"
#include <iostream>
#include <fstream>
#include <string>

void run_lint(const std::string& json, const std::string& test_name) {
    try {
        auto root = JsonParser::parse(json);
        auto issues = lintJson(root, json);
        std::cout << "Test: " << test_name << std::endl;
        if (issues.empty()) {
            std::cout << "  No issues found.\n";
        } else {
            for (const auto& issue : issues) {
                std::cout << "  [";
                switch (issue.severity) {
                    case JsonLintIssue::Severity::Error: std::cout << "Error"; break;
                    case JsonLintIssue::Severity::Warning: std::cout << "Warning"; break;
                    case JsonLintIssue::Severity::Info: std::cout << "Info"; break;
                }
                std::cout << "] " << issue.message;
                if (issue.line > 0 && issue.column > 0)
                    std::cout << " (line " << issue.line << ", col " << issue.column << ")";
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "Test: " << test_name << "\n  Exception: " << ex.what() << "\n\n";
    }
}

int main() {
    std::string json_empty_key = "{\"\": \"value\"}";
    run_lint(json_empty_key, "Empty Key");

    std::string json_deep = "{";
    for (int i = 0; i < 21; ++i) json_deep += "\"a\":{";
    json_deep += "\"end\":1";
    for (int i = 0; i < 21; ++i) json_deep += "}";
    run_lint(json_deep, "Deep Nesting");

    std::string json_bad_naming = "{\"bad_key\": 1, \"goodKey\": 2}";
    run_lint(json_bad_naming, "Naming Convention");

    std::string json_mixed_array = "[1, \"two\", 3]";
    run_lint(json_mixed_array, "Mixed Array Types");

    std::string json_clean = "{\"goodKey\": [1, 2, 3], \"anotherKey\": {\"nestedKey\": 5}}";
    run_lint(json_clean, "Clean JSON");

    return 0;
}
