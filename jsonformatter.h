#ifndef JSONFORMATTER_H
#define JSONFORMATTER_H

#include <memory>
#include <ostream>
#include "jsonparser.h"

void printIndent(std::ostream& os, int indent);
void printJson(const std::shared_ptr<JsonValue>& value, std::ostream& os, int indent, int indentStep, bool compact);

std::vector<JsonLintIssue> lintJsonVector(const std::string& jsonStr);
std::vector<JsonLintIssue> lintJson(const std::shared_ptr<JsonValue>& root);

#endif
