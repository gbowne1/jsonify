#include "jsonformatter.h"
#include <cmath>
#include <iomanip>

void printIndent(std::ostream& os, int indent) {
    os << std::string(indent, ' ');
}

void printJson(const std::shared_ptr<JsonValue>& value,
               std::ostream& os,
               int indent, int indentStep, bool compact) {
    if (!value) { os << "null"; return; }

    using Type = JsonValue::Type;
    switch (value->getType()) {
    case Type::Null:  os << "null"; break;
    case Type::Bool:  os << (value->getBool() ? "true" : "false"); break;
    case Type::Number: {
        double n = value->getNumber();
        if (std::isinf(n) || std::isnan(n)) os << "null";
        else os << std::fixed << std::setprecision(15) << n;
        break;
    }
    case Type::String: {
        os << '"';
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
        os << '"';
        break;
    }
    case Type::Array: {
        const auto& a = value->getArray();
        os << '[';
        if (!compact && !a.empty()) os << '\n';
        for (size_t i = 0; i < a.size(); ++i) {
            if (!compact) printIndent(os, indent + indentStep);
            printJson(a[i], os, indent + indentStep, indentStep, compact);
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
            os << '"' << kv.first << "\": ";
            printJson(kv.second, os, indent + indentStep, indentStep, compact);
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
