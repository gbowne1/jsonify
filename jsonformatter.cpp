#include "jsonformatter.h"
#include <cmath>
#include <iomanip>
#include <string>

void printIndent(std::ostream& os, int indent) {
    os << std::string(indent, ' ');
}

// Updated function signature with the 'useColor' parameter
void printJson(const std::shared_ptr<JsonValue>& value,
               std::ostream& os,
               int indent, int indentStep, bool compact, bool useColor) {
    
    // Helper to get color code or empty string
    auto get_color = [&](const std::string& color_code) {
        return useColor ? color_code : "";
    };
    
    // Helper to get the reset code or empty string
    auto get_reset = [&]() {
        return useColor ? AnsiColor::RESET : "";
    };

    if (!value) {
        os << get_color(AnsiColor::NULL_T) << "null" << get_reset();
        return;
    }

    using Type = JsonValue::Type;
    switch (value->getType()) {
    case Type::Null:
        os << get_color(AnsiColor::NULL_T) << "null" << get_reset();
        break;
    case Type::Bool:
        os << get_color(AnsiColor::BOOL) << (value->getBool() ? "true" : "false") << get_reset();
        break;
    case Type::Number: {
        double n = value->getNumber();
        os << get_color(AnsiColor::NUMBER);
        if (std::isinf(n) || std::isnan(n)) os << "null";
        else if (n==(long long)n) os << (long long)n;
        else os << std::fixed << std::setprecision(15) << n;
        os << get_reset();
        break;
    }
    case Type::String: {
        os << get_color(AnsiColor::STRING) << '"';
        for (char c : value->getString()) {
            switch (c) {
                case '"':  os << "\\\""; break;
                case '\\': os << "\\\\"; break;
                case '\b': os << "\\b"; break;
                case '\f': os << "\\f"; break;
                case '\n': os << "\\n"; break;
                case '\r': os << "\\r"; break;
                case '\t': os << "\\t"; break;
                default:   os << c; break;
            }
        }
        os << '"' << get_reset();
        break;
    }
    case Type::Array: {
        const auto& a = value->getArray();
        os << '[';
        if (!compact && !a.empty()) os << '\n';
        for (size_t i = 0; i < a.size(); ++i) {
            if (!compact) printIndent(os, indent + indentStep);
            // Recursive call must pass the useColor state
            printJson(a[i], os, indent + indentStep, indentStep, compact, useColor);
            if (i + 1 < a.size()) os << ',';
            if (!compact) os << '\n';
        }
        if (!compact && !a.empty()) printIndent(os, indent);
        os << ']';
        break;
    }
    case Type::Object: {
        const auto& o = value->getObject();
        os << '{';
        if (!compact && !o.empty()) os << '\n';
        size_t i = 0;
        for (const auto& kv : o) {
            if (!compact) printIndent(os, indent + indentStep);
            
            // Apply color to the key string only
            os << get_color(AnsiColor::KEY) << '"' << kv.first << "\":" << get_reset();
            os << (!compact ? " " : "");

            // Recursive call must pass the useColor state
            printJson(kv.second, os, indent + indentStep, indentStep, compact, useColor);
            
            if (i + 1 < o.size()) os << ',';
            if (!compact) os << '\n';
            ++i;
        }
        if (!compact && !o.empty()) printIndent(os, indent);
        os << '}';
        break;
    }
    }
}
