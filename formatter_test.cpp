// formatter_test.cpp
#include "jsonparser.h"
#include "jsonformatter.h"
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <memory>

// Very simple recursive equality check for JsonValue trees
bool json_equal(const std::shared_ptr<JsonValue>& a,
                const std::shared_ptr<JsonValue>& b) {
    if (!a || !b) return a == b;
    if (a->getType() != b->getType()) return false;

    switch (a->getType()) {
        case JsonValue::Type::Null:   return true;
        case JsonValue::Type::Bool:   return a->getBool()   == b->getBool();
        case JsonValue::Type::Number: return a->getNumber() == b->getNumber();
        case JsonValue::Type::String: return a->getString() == b->getString();

        case JsonValue::Type::Array: {
            const auto& arrA = a->getArray();
            const auto& arrB = b->getArray();
            if (arrA.size() != arrB.size()) return false;
            for (size_t i = 0; i < arrA.size(); ++i) {
                if (!json_equal(arrA[i], arrB[i])) return false;
            }
            return true;
        }

        case JsonValue::Type::Object: {
            const auto& objA = a->getObject();
            const auto& objB = b->getObject();
            if (objA.size() != objB.size()) return false;
            for (const auto& [key, valA] : objA) {
                auto it = objB.find(key);
                if (it == objB.end() || !json_equal(valA, it->second)) {
                    return false;
                }
            }
            return true;
        }
    }
    return false; // unreachable
}

struct FormatTest {
    std::string input;
    bool compact;
    bool useColor;
    int indentStep;
    std::string description;
};

void run_format_test(const FormatTest& t) {
    std::cout << std::left << std::setw(42) << ("[" + t.description + "]")
              << " → ";

    try {
        // Step 1: parse original
        auto original = JsonParser::parse(t.input);

        // Step 2: format
        std::ostringstream oss;
        printJson(original, oss, 0, t.indentStep, t.compact, t.useColor);
        std::string formatted = oss.str();

        // Step 3: re-parse formatted output
        auto reparsed = JsonParser::parse(formatted);

        // Step 4: compare structure (ignore formatting differences)
        bool structures_match = json_equal(original, reparsed);

        if (structures_match) {
            std::cout << "PASS (structure preserved)\n";
        } else {
            std::cout << "FAIL (structure changed after format → parse)\n";
            std::cout << "  Original: " << t.input << "\n";
            std::cout << "  Formatted: " << formatted << "\n";
        }

        // Optional: minimal sanity on output shape
        if (t.compact) {
            if (formatted.find('\n') != std::string::npos) {
                std::cout << "  WARNING: compact mode still has newlines\n";
            }
        } else {
            if (formatted.find('\n') == std::string::npos && !formatted.empty()) {
                std::cout << "  WARNING: pretty mode has no newlines\n";
            }
        }

    } catch (const std::exception& e) {
        std::cout << "FAIL (exception: " << e.what() << ")\n";
    }
}

int main() {
    std::cout << "=== JSON Formatter Tests ===\n\n";

    std::vector<FormatTest> tests = {
        // Basic pretty printing
        {R"({"name":"Alice","age":28,"active":true,"scores":[85,92.5,null]})",
         false, false, 2, "standard pretty-print, no color"},

        {R"({"name":"Alice","age":28,"active":true,"scores":[85,92.5,null]})",
         false, true,  2, "pretty-print with ANSI colors"},

        // Compact mode
        {R"( [ 1 , true , "hello\"world" , {"x":null} ] )",
         true, false, 2, "compact mode – no newlines"},

        // Different indent levels
        {R"({"a":{"b":{"c":42}}})",
         false, false, 2, "pretty indent=2 (default)"},

        {R"({"a":{"b":{"c":42}}})",
         false, false, 4, "pretty indent=4"},

        {R"({"a":{"b":{"c":42}}})",
         false, false, 0, "indent=0 → still pretty (newlines but no indent)"},

        // Empty structures
        {"{}",   false, false, 2, "empty object pretty"},
        {"{}",   true,  false, 2, "empty object compact"},
        {"[]",   false, false, 2, "empty array pretty"},
        {"[]",   true,  false, 2, "empty array compact"},

        // Nested & escapes
        {R"({"path":"C:\\dir\\file.txt","emoji":"😀 \uD83D\uDE00","esc":"\n\t\""})",
         false, false, 2, "nested escapes & unicode"},

        // Single value documents
        {"true",   false, false, 2, "single bool pretty"},
        {"123.45", true,  false, 2, "single number compact"},
        {R"("hello\nworld")", false, true, 2, "single string with color"},
    };

    int pass_count = 0;

    for (const auto& t : tests) {
        run_format_test(t);
        // Very simplistic pass count — improve later if needed
        pass_count++; // placeholder — you'd need to track real passes
    }

    std::cout << "\nRan " << tests.size() << " formatter tests.\n";
    // In a real version: std::cout << pass_count << " passed\n";

    return 0;
}
