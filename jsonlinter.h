#ifndef JSONLINTER_H
#define JSONLINTER_H

#include <string>
#include <vector>
#include <memory>
#include "jsonparser.h"

class JsonValue;

struct JsonLintIssue {
    enum class Severity { Error, Warning, Info };
    Severity severity;
    std::string message;
    int line, column;
};

// std::vector<JsonLintIssue> lintJson(const std::string& jsonStr);
std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root);

#endif
