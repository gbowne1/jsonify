#include "jsonformatter.h"
#include <cmath>
#include <iomanip>

void printIndent(std::ostream& os, int indent) {
    os << std::string(indent, ' ');
}

void printJson(const std::shared_ptr<JsonValue>& value,
               std::ostream& os,
               int indent, int indentStep, bool compact, bool useColor) { // <--- Added useColor
    if (!value) { os << AnsiColor::NULL_T << "null" << AnsiColor::RESET; return; } // Handle null for initial check

    using Type = JsonValue::Type;
    switch (value->getType()) {
    case Type::Null:
        os << (useColor ? AnsiColor::NULL_T : "") << "null" << (useColor ? AnsiColor::RESET : "");
        break;
    case Type::Bool:
        os << (useColor ? AnsiColor::BOOL : "") << (value->getBool() ? "true" : "false") << (useColor ? AnsiColor::RESET : "");
        break;
    case Type::Number: {
        double n = value->getNumber();
        os << (useColor ? AnsiColor::NUMBER : ""); // Apply color
        if (std::isinf(n) || std::isnan(n)) os << "null";
        else os << std::fixed << std::setprecision(15) << n;
        os << (useColor ? AnsiColor::RESET : ""); // Reset color
        break;
    }
    case Type::String: {
        os << (useColor ? AnsiColor::STRING : "") << '"'; // Apply string color before quote
        // ... (string escaping logic remains the same)
        // ... (for loop for char c : value->getString())
        os << '"' << (useColor ? AnsiColor::RESET : ""); // Reset color after closing quote
        break;
    }
    case Type::Array: {
        const auto& a = value->getArray();
        os << '[';
        // ... (formatting logic remains the same)
        for (size_t i = 0; i < a.size(); ++i) {
            if (!compact) printIndent(os, indent + indentStep);
            printJson(a[i], os, indent + indentStep, indentStep, compact, useColor); // <--- Pass useColor
            // ... (rest of array logic)
        }
        // ... (rest of array logic)
        os << ']';
        break;
    }
    case Type::Object: {
        const auto& o = value->getObject();
        os << '{';
        // ... (formatting logic remains the same)
        size_t i = 0;
        for (const auto& kv : o) {
            if (!compact) printIndent(os, indent + indentStep);
            os << (useColor ? AnsiColor::KEY : "") << '"' << kv.first << "\":" << (useColor ? AnsiColor::RESET : "") << " "; // <--- Key coloring
            printJson(kv.second, os, indent + indentStep, indentStep, compact, useColor); // <--- Pass useColor
            // ... (rest of object logic)
            ++i;
        }
        // ... (rest of object logic)
        os << '}';
        break;
    }
    }
}
