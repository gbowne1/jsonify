#include "jsonparser.h"
#include <iostream>      
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <cctype>
#include <utility>
#include <cstdint>
#include <fstream> // Added for std::ifstream

const std::string COLOR_RESET = "\033[0m";
const std::string COLOR_ERROR = "\033[31m"; // Red
const std::string COLOR_WARNING = "\033[33m"; // Yellow
const std::string COLOR_ALERT = "\033[32m"; // Green

// JsonValue implementation
JsonValue::JsonValue() : type_(Type::Null) {}
JsonValue::JsonValue(bool value) : type_(Type::Bool), bool_(value) {}
JsonValue::JsonValue(double value) : type_(Type::Number), number_(value) {}
JsonValue::JsonValue(const std::string &value) : type_(Type::String), string_(value) {}
JsonValue::JsonValue(JsonArray value) : type_(Type::Array), array_(std::move(value)) {}
JsonValue::JsonValue(JsonObject value) : type_(Type::Object), object_(std::move(value)) {}

JsonValue::Type JsonValue::getType() const { return type_; }
bool JsonValue::getBool() const { return bool_; }
double JsonValue::getNumber() const { return number_; }
const std::string &JsonValue::getString() const { return string_; }
const JsonArray &JsonValue::getArray() const { return array_; }
const JsonObject &JsonValue::getObject() const { return object_; }

// JsonParser implementation

std::shared_ptr<JsonValue> JsonParser::loadFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str(); // Get the content as a string

    std::cout << "Loaded JSON content:\n" << jsonContent << std::endl; // Debug output

    return parse(jsonContent); 
}

std::string JsonParser::decodeUnicode(const std::string &hex)
{
    if (hex.length() != 4) {
        throw std::runtime_error("Invalid Unicode escape sequence: must be 4 hex digits");
    }

    // Convert hex string to uint32_t
    uint32_t codepoint; // Change to uint32_t
    try {
        size_t pos;
        codepoint = std::stoul(hex, &pos, 16);
        if (pos != 4) {
            throw std::runtime_error("Invalid Unicode escape sequence: incomplete hex value");
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Invalid Unicode escape sequence: invalid hex value");
    }

    std::string result;

    // Convert Unicode codepoint to UTF-8
    if (codepoint <= 0x7F) {
        // 1-byte UTF-8 (ASCII)
        result += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        // 2-byte UTF-8
        result += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0xFFFF) {
        // 3-byte UTF-8
        result += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else if (codepoint <= 0x10FFFF) {
        // 4-byte UTF-8 (valid range for Unicode)
        result += static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07));
        result += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        throw std::runtime_error("Unicode codepoint out of range");
    }

    return result;
}

std::shared_ptr<JsonValue> JsonParser::parse(const std::string &json)
{
    std::istringstream is(json);
    return parseValue(is);
}

void JsonParser::skipWhitespace(std::istream &is)
{
    while (std::isspace(is.peek()))
    {
        is.get();
    }
}

std::shared_ptr<JsonValue> JsonParser::parseValue(std::istream &is)
{
    skipWhitespace(is);
    char c = is.get();
    std::cout << "Parsing value, got character: '" << c << "'" << std::endl;

    if (c == '{')
        return std::make_shared<JsonValue>(parseObject(is));
    if (c == '[')
        return std::make_shared<JsonValue>(parseArray(is));
    if (c == '"')
        return std::make_shared<JsonValue>(parseString(is));
    if (c == 't' || c == 'f')
        return std::make_shared<JsonValue>(parseBoolean(is, c));
    if (c == 'n')
        return parseNull(is);
    if (std::isdigit(c) || c == '-')
    {
        is.unget();
        return std::make_shared<JsonValue>(parseNumber(is));
    }

    throw std::runtime_error("Unexpected character");
}

JsonObject JsonParser::parseObject(std::istream &is)
{
    JsonObject obj;
    skipWhitespace(is);
    char c = is.get();
    if (c == '}')
        return obj; // Empty object
    is.unget(); // Put back the character for further processing

    while (true)
    {
        skipWhitespace(is);
        c = is.get();
        if (c != '"') // Check for opening quote
            throw std::runtime_error("Expected '\"' at the beginning of key in object, got '" + std::string(1, c) + "'");

        auto key = parseString(is);
        std::cout << "Parsed key: " << key << std::endl; // Debug output
        skipWhitespace(is);
        c = is.get();
        std::cout << "Next character after key: '" << c << "'" << std::endl; // Debug output
        if (c != ':')
            throw std::runtime_error("Expected ':' after key in object, got '" + std::string(1, c) + "'");

        obj[key] = parseValue(is);

        skipWhitespace(is);
        c = is.get();
        if (c == '}')
            break;
        if (c != ',')
            throw std::runtime_error("Expected ',' or '}' in object, got '" + std::string(1, c) + "'");
    }
    return obj;
}

JsonArray JsonParser::parseArray(std::istream &is)
{
    JsonArray arr;
    skipWhitespace(is);
    char c = is.get();
    if (c == ']')
        return arr;
    is.unget();

    while (true)
    {
        arr.push_back(parseValue(is));
        skipWhitespace(is);
        c = is.get();
        if (c == ']')
            break;
        if (c != ',')
            throw std::runtime_error("Expected ',' or ']' in array, got '" + std::string(1, c) + "'");
    }
    return arr;
}

std::string JsonParser::parseString(std::istream &is)
{
    std::string str;
    char c;
    while (is.get(c))
    {
        if (c == '"')
            return str; // Return the string when the closing quote is found
        if (c == '\\')
        {
            is.get(c);
            switch (c)
            {
            case '"':
            case '\\':
            case '/':
                str += c;
                break;
            case 'b':
                str += '\b';
                break;
            case 'f':
                str += '\f';
                break;
            case 'n':
                str += '\n';
                break;
            case 'r':
                str += '\r';
                break;
            case 't':
                str += '\t';
                break;
            case 'u': {
                std::string hex(4, ' ');
                is.read(&hex[0], 4);
                try {
                    std::string decoded = decodeUnicode(hex); // Call static member function
                    str += decoded;
                } catch (const std::exception&) {
                    throw std::runtime_error("Invalid Unicode escape sequence");
                }
                break;
            }
            default:
                throw std::runtime_error("Invalid escape sequence");
            }
        }
        else
        {
            str += c;
        }
    }
    throw std::runtime_error("Unterminated string"); // Handle unterminated string
}

bool JsonParser::parseBoolean(std::istream &is, char first)
{
    std::string token(1, first);
    char c;
    while (is.peek() != EOF && std::isalpha(is.peek())) {
        is.get(c);
        token += c;
    }
    if (token == "true") return true;
    if (token == "false") return false;
    throw std::runtime_error("Invalid boolean value: " + token);
}

std::shared_ptr<JsonValue> JsonParser::parseNull(std::istream &is)
{
    std::string rest(3, ' ');
    is.read(&rest[0], 3);
    if (rest == "ull")
        return std::make_shared<JsonValue>();
    throw std::runtime_error("Invalid null value");
}

double JsonParser::parseNumber(std::istream &is)
{
    std::string numStr;
    char c;
    bool hasDigit = false;
    while (is.peek() != EOF && (std::isdigit(is.peek()) || is.peek() == '.' || 
                                is.peek() == 'e' || is.peek() == 'E' || 
                                is.peek() == '+' || is.peek() == '-')) {
        is.get(c);
        numStr += c;
        if (std::isdigit(c)) hasDigit = true;
    }
    if (!hasDigit) {
        throw std::runtime_error("Invalid number format: no digits found");
    }
    try {
        size_t pos;
        double result = std::stod(numStr, &pos);
        if (pos != numStr.length()) {
            throw std::runtime_error("Invalid number format: trailing characters");
        }
        return result;
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Invalid number format");
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Number out of range");
    }
}

std::string correctJson(const std::string &json) {
    std::string corrected = json;
    bool inString = false, inEscape = false;
    for (size_t i = 0; i < corrected.length() - 1; ++i) {
        if (corrected[i] == '"' && !inEscape) inString = !inString;
        inEscape = corrected[i] == '\\' && !inEscape;
        if (!inString && (corrected[i] == '}' || corrected[i] == ']') && 
            corrected[i + 1] != ',' && corrected[i + 1] != '}' && corrected[i + 1] != ']') {
            corrected.insert(i + 1, ",");
        }
    }
    return corrected;
}
