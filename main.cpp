#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include "jsonparser.h"
#include "jsonformatter.h"
#include "jsonlinter.h"

void printUsage() {
    std::cout << "Usage: jsonify [options] <file.json>\n"
              << "Options:\n"
              << "  --lint              Lint the JSON file\n"
              << "  --format            Format the JSON file\n"
              << "  --compact           Output compact JSON (no pretty-print)\n"
              << "  --indent N          Set indent spaces (default 2)\n"
              << "  --jsonc             Allow JSONC (comments)\n"
              << "  --help              Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    bool doLint = false, doFormat = false, compact = false, allowJsonc = false;
    int indent = 2;
    std::string filename;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--lint") == 0) doLint = true;
        else if (strcmp(argv[i], "--format") == 0) doFormat = true;
        else if (strcmp(argv[i], "--compact") == 0) compact = true;
        else if (strcmp(argv[i], "--jsonc") == 0) allowJsonc = true;
        else if (strcmp(argv[i], "--indent") == 0 && i + 1 < argc) {
            indent = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            printUsage();
            return 0;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        } else {
            std::cerr << "Unknown option: " << argv[i] << "\n";
            return 1;
        }
    }

    if (filename.empty()) {
        std::cerr << "Error: No input file provided.\n";
        return 1;
    }

    try {
        std::ifstream inFile(filename);
        if (!inFile) throw std::runtime_error("Cannot open file: " + filename);

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string jsonContent = buffer.str();

        if (allowJsonc) {
            size_t pos;
            
            while ((pos = jsonContent.find("//")) != std::string::npos) {
                size_t end = jsonContent.find('\n', pos);
                jsonContent.erase(pos, (end != std::string::npos ? end - pos : std::string::npos));
            }

            while ((pos = jsonContent.find("/*")) != std::string::npos) {
                size_t end = jsonContent.find("*/", pos);
                if (end != std::string::npos) {
                    jsonContent.erase(pos, end - pos + 2);
                } else {
                    std::cerr << "Warning: Unclosed multi-line comment.\n";
                    jsonContent.erase(pos);
                    break;
                }
            }
            
            // Fix dangling commas after stripping comments
            size_t last_char_pos = jsonContent.find_last_not_of(" \t\n\r");
            if (last_char_pos != std::string::npos && jsonContent[last_char_pos] == ',') {
                jsonContent.erase(last_char_pos, 1);
            }
        }

        auto root = JsonParser::parse(jsonContent);

        if (doLint) {
            auto issues = lintJson(root);
            if (issues.empty()) {
                std::cout << "No lint issues found.\n";
            } else {
                for (const auto& issue : issues) {
                    std::string severity;
                    switch (issue.severity) {
                        case JsonLintIssue::Severity::Error: severity = "Error"; break;
                        case JsonLintIssue::Severity::Warning: severity = "Warning"; break;
                        case JsonLintIssue::Severity::Info: severity = "Info"; break;
                    }
                    std::cout << severity << ": " << issue.message;
                    if (issue.line != -1 && issue.column != -1) {
                        std::cout << " (line " << issue.line << ", column " << issue.column << ")";
                    }
                    std::cout << "\n";
                }
            }
        }

        if (doFormat) {
            printJson(root, std::cout, 0, indent, compact);
            std::cout << std::endl;
        }

        if (!doLint && !doFormat) {
            std::cout << "Parsed successfully.\n";
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
