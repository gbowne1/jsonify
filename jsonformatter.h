#ifndef JSONFORMATTER_H
#define JSONFORMATTER_H

#include <memory>
#include <ostream>
#include "jsonparser.h"

void printIndent(std::ostream& os, int indent);
void printJson(const std::shared_ptr<JsonValue>& value,
                std::ostream& os,
                int indent = 0,
                int indentStep = 2,
                bool compact = false);

#endif
