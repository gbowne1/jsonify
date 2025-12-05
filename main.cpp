#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "jsonparser.h"
#include "jsonformatter.h"
#include "jsonlinter.h"

const std::string APP_VERSION = "0.0.1";

void printUsage() {
    std::cout <<
        "Usage: jsonify [options] <file.json>\n"
        "Options:\n"
        "  --lint          Lint the JSON file\n"
        "  --format        Pretty-print the JSON file\n"
        "  --fix, -f       Attempt to auto-correct the JSON\n"
        "  --compact       Compact output (no newlines/indent)\n"
        "  --indent N      Indent width (default 2)\n"
        "  --jsonc         Allow comments (JSONC)\n"
        "  --color         Enable color output (default)\n"
        "  --no-color      Disable color output\n"
        "  -v, --version   Show version information\n"
        "  --help          Show this help\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) { printUsage(); return 1; }

    bool doLint = false, doFormat = false, compact = false, jsonc = false, doFix = false;
    bool useColor = true;
    bool colorSpecified = false;
    
    int indent = 2;
    std::string filename;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--version" || arg == "-version" || arg == "-v" || arg == "-V") {
            std::cout << "jsonify v" << APP_VERSION << " (Semantic Versioning)\n";
            return 0;
        }
        
        if (arg == "--lint")      doLint   = true;
        else if (arg == "--format") doFormat = true;
        else if (arg == "--compact") compact = true;
        else if (arg == "--jsonc")    jsonc    = true;
        else if (arg == "--fix" || arg == "-f" ) {
            doFix = true;
        }
        else if (arg == "--color") { useColor = true; colorSpecified = true; }
        else if (arg == "--no-color") { useColor = false; colorSpecified = true; }
        else if (arg == "--indent" && i+1 < argc) { indent = std::stoi(argv[++i]); }
        else if (arg == "--help") { printUsage(); return 0; }
        else if (arg[0] != '-')    filename = arg;
        else { std::cerr << "Unknown option: " << arg << '\n'; return 1; }
    }

    if (filename.empty()) { std::cerr << "No input file.\n"; return 1; }

    try {
        std::ifstream f(filename);
        if (!f) throw std::runtime_error("Cannot open " + filename);
        std::stringstream buf; buf << f.rdbuf();
        std::string src = buf.str();

        // ---- JSONC comment stripping ----
        if (jsonc) {
            std::string clean;
            bool inStr = false, esc = false;
            for (size_t i = 0; i < src.size(); ++i) {
                char c = src[i];
                if (esc) { clean += c; esc = false; continue; }
                if (c == '\\') { esc = true; clean += c; continue; }
                if (c == '"' && !esc) inStr = !inStr;
                if (!inStr && i+1 < src.size()) {
                    if (src[i] == '/' && src[i+1] == '/') { // // comment
                        i = src.find('\n', i) ; if (i == std::string::npos) i = src.size()-1;
                        continue;
                    }
                    if (src[i] == '/' && src[i+1] == '*') { // /* */
                        size_t end = src.find("*/", i+2);
                        i = (end == std::string::npos) ? src.size()-1 : end+1;
                        continue;
                    }
                }
                clean += c;
            }
            src = clean;
        }

        if (doFix) {
            src = correctJson(src);
        }

        // ---- Parse ----
        auto root = JsonParser::parse(src);

        // ---- Lint ----
        if (doLint) {
            auto issues = lintJson(root, src);
            if (issues.empty()) {
                std::cout << "No lint issues.\n";
            } else {
                for (const auto& iss : issues) {
                    std::string sev;
                    switch (iss.severity) {
                        case JsonLintIssue::Severity::Error:    sev = "Error";    break;
                        case JsonLintIssue::Severity::Warning: sev = "Warning"; break;
                        case JsonLintIssue::Severity::Info:    sev = "Info";     break;
                    }
                    std::cout << sev << ": " << iss.message;
                    if (iss.line != -1)
                        std::cout << " (line " << iss.line << ", col " << iss.column << ")";
                    std::cout << '\n';
                }
            }
        }

        // ---- Format ----
        if (doFormat) {
            printJson(root, std::cout, 0, indent, compact, useColor);
            std::cout << '\n';
        }

        if (!doLint && !doFormat) std::cout << "Parsed successfully.\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
