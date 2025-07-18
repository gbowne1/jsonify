#ifndef JSONLINTER_H
#define JSONLINTER_H

#include <string>
#include <vector>
#include <memory>
#include "jsonparser.h"

struct JsonLintIssue {
    enum class Severity { Error, Warning, Info };
    Severity severity;
    std::string message;
    int line, column; // Optional: set -1 if unavailable
};

std::vector<JsonLintIssue> lintJson(const std::string& jsonStr);
std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root);

#endif
