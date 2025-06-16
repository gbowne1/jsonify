#include "jsonparser.h"
#include <iostream>      
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <cctype>
#include <utility>

// JsonValue implementation
JsonValue::JsonValue() : type_(Type::Null) {}
JsonValue::JsonValue(bool value) : type_(Type::Boolean), bool_(value) {}
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
        return obj;
    is.unget();

    while (true)
    {
        auto key = parseString(is);
        std::cout << "Parsed key: " << key << std::endl; // Debug output
        skipWhitespace(is);
        char c = is.get();
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
            case 'u':
                throw std::runtime_error("Unicode escape sequences (\\uXXXX) are not supported.");
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
    std::string rest(3, ' ');
    is.read(&rest[0], 3);
    if (first == 't' && rest == "rue")
        return true;
    if (first == 'f' && rest == "als")
    {
        is.get(); // consume 'e'
        return false;
    }
    throw std::runtime_error("Invalid boolean value");
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
    while (is.peek() != EOF && (std::isdigit(is.peek()) || is.peek() == '.' || is.peek() == 'e' ||
                                 is.peek() == 'E' || is.peek() == '+' || is.peek() == '-'))
    {
        is.get(c);
        numStr += c;
    }
    return std::stod(numStr);
}

