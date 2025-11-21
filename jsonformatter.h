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
                bool useColor = false);

namespace AnsiColor {
    const std::string RESET = "\033[0m";
    const std::string KEY   = "\033[36m"; // Cyan
    const std::string STRING = "\033[32m"; // Green
    const std::string NUMBER = "\033[33m"; // Yellow
    const std::string BOOL   = "\033[35m"; // Magenta 
    const std::string NULL_T = "\033[35m"; // Magenta
}

#endif
