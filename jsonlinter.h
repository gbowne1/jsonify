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
    int line = -1, column = -1;
};

std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root,
                                    const std::string& source);

#endif
