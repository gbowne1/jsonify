#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <memory>
#include <stdexcept>

class JsonValue;

using JsonObject = std::unordered_map<std::string, std::shared_ptr<JsonValue>>;
using JsonArray = std::vector<std::shared_ptr<JsonValue>>;

class JsonValue
{
public:
    enum class Type
    {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object
    };

    JsonValue();
    explicit JsonValue(bool value);
    explicit JsonValue(double value);
    explicit JsonValue(const std::string &value);
    explicit JsonValue(JsonArray value);
    explicit JsonValue(JsonObject value);

    Type getType() const;
    bool getBool() const;
    double getNumber() const;
    const std::string &getString() const;
    const JsonArray &getArray() const;
    const JsonObject &getObject() const;

private:
    Type type_;
    bool bool_;
    double number_;
    std::string string_;
    JsonArray array_;
    JsonObject object_;
};

class JsonParser
{
public:
    static std::shared_ptr<JsonValue> parse(const std::string &json);
    static std::shared_ptr<JsonValue> loadFromFile(const std::string &filename);

private:
    static void skipWhitespace(std::istream &is);
    static std::shared_ptr<JsonValue> parseValue(std::istream &is);
    static JsonObject parseObject(std::istream &is);
    static JsonArray parseArray(std::istream &is);
    static std::string parseString(std::istream &is);
    static bool parseBoolean(std::istream &is, char first);
    static std::shared_ptr<JsonValue> parseNull(std::istream &is);
    static double parseNumber(std::istream &is);
    static std::string decodeUnicode(const std::string &hex); // Declare as static
};

std::string correctJson(const std::string &json);

#endif // JSONPARSER_H
