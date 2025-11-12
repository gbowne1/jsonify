#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

class JsonValue;

using JsonObject = std::unordered_map<std::string, std::shared_ptr<JsonValue>>;
using JsonArray  = std::vector<std::shared_ptr<JsonValue>>;

class JsonValue {
public:
    enum class Type { Null, Bool, Number, String, Array, Object };

    JsonValue();
    explicit JsonValue(bool v);
    explicit JsonValue(double v);
    explicit JsonValue(std::string v);
    explicit JsonValue(JsonArray v);
    explicit JsonValue(JsonObject v);

    Type               getType()   const;
    bool               getBool()   const;
    double             getNumber() const;
    const std::string& getString() const;
    const JsonArray&   getArray()  const;
    const JsonObject&  getObject() const;

private:
    Type        type_;
    bool        bool_;
    double      number_;
    std::string string_;
    JsonArray   array_;
    JsonObject  object_;
};

class JsonParser {
public:
    static std::shared_ptr<JsonValue> parse(const std::string& json);
    static std::shared_ptr<JsonValue> loadFromFile(const std::string& filename);

    // helpers for line/column tracking
    struct Pos {
        size_t line = 1;
        size_t col = 1;
        Pos() = default;
        Pos(size_t l, size_t c) : line(l), col(c) {}
    };

    static Pos currentPos(const std::string& src, size_t idx);

private:
    static void skipWhitespace(std::istream& is, const std::string& src, Pos& pos);
    static std::shared_ptr<JsonValue> parseValue(std::istream& is,
                                                 const std::string& src, Pos& pos);
    static JsonObject parseObject(std::istream& is,
                                  const std::string& src, Pos& pos);
    static JsonArray  parseArray (std::istream& is,
                                  const std::string& src, Pos& pos);
    static std::string parseString(std::istream& is,
                                   const std::string& src, Pos& pos);
    static bool parseBoolean(std::istream& is,
                             const std::string& src, Pos& pos, char first);
    static std::shared_ptr<JsonValue> parseNull(std::istream& is,
                                                const std::string& src, Pos& pos);
    static double parseNumber(std::istream& is,
                              const std::string& src, Pos& pos);
    static std::string decodeUnicode(const std::string& hex);
};

std::string correctJson(const std::string& json);   // simple auto-correction

#endif // JSONPARSER_H
