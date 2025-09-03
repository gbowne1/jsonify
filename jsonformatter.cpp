#include <ostream>
#include "jsonformatter.h"

void printIndent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; ++i) {
        os << "  ";
    }
}

void printJson(const std::shared_ptr<JsonValue>& value, std::ostream& os, int indent, int indentStep, bool compact) {
    if (!value) {
        os << "null";
        return;
    }
    using Type = JsonValue::Type;
    switch (value->getType()) {
    case Type::Null: os << "null"; break;
    case Type::Bool: os << (value->getBool() ? "true" : "false"); break;
    case Type::Number: {
        double num = value->getNumber();
        if (std::isinf(num) || std::isnan(num)) {
            os << "null";
        } else {
            os << num;
        }
        break;
    }
    case Type::String: {
        os << '"';
        const std::string& str = value->getString();
        for (char c : str) {
            switch (c) {
                case '"': os << "\\\""; break;
                case '\\': os << "\\\\"; break;
                case '\b': os << "\\b"; break;
                case '\f': os << "\\f"; break;
                case '\n': os << "\\n"; break;
                case '\r': os << "\\r"; break;
                case '\t': os << "\\t"; break;
                default: os << c; break;
            }
        }
        os << '"';
        break;
    }
    case Type::Array: {
        const auto& arr = value->getArray();
        os << "[";
        if (!compact && !arr.empty()) os << "\n";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (!compact) printIndent(os, indent + indentStep);
            printJson(arr[i], os, indent + indentStep, indentStep, compact);
            if (i + 1 < arr.size()) os << ",";
            if (!compact) os << "\n";
        }
        if (!compact && !arr.empty()) printIndent(os, indent);
        os << "]";
        break;
    }
    case Type::Object: {
        const auto& obj = value->getObject();
        os << "{";
        if (!compact && !obj.empty()) os << "\n";
        size_t i = 0;
        for (const auto& kv : obj) {
            if (!compact) printIndent(os, indent + indentStep);
            os << '"' << kv.first << "\": ";
            printJson(kv.second, os, indent + indentStep, indentStep, compact);
            if (i + 1 < obj.size()) os << ",";
            if (!compact) os << "\n";
            ++i;
        }
        if (!compact && !obj.empty()) printIndent(os, indent);
        os << "}";
        break;
    }
    }
}
