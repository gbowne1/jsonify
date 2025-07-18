#ifndef JSONFORMATTER_H
#define JSONFORMATTER_H

#include "jsonparser.h"
#include <ostream>
#include <string>

void printJson(const std::shared_ptr<JsonValue>& value, std::ostream& os, int indent = 0, int indentStep = 4, bool compact = false);

#endif
