#include "jsonparser.h"
#include <cctype>
#include <iomanip>
#include <sstream>

JsonValue::JsonValue() : type_(Type::Null) {}
JsonValue::JsonValue(bool v)       : type_(Type::Bool),   bool_(v) {}
JsonValue::JsonValue(double v)     : type_(Type::Number), number_(v) {}
JsonValue::JsonValue(std::string v): type_(Type::String), string_(std::move(v)) {}
JsonValue::JsonValue(JsonArray v)  : type_(Type::Array),  array_(std::move(v)) {}
JsonValue::JsonValue(JsonObject v) : type_(Type::Object), object_(std::move(v)) {}

JsonValue::Type               JsonValue::getType()   const { return type_; }
bool                          JsonValue::getBool()   const { return bool_; }
double                        JsonValue::getNumber() const { return number_; }
const std::string&            JsonValue::getString() const { return string_; }
const JsonArray&              JsonValue::getArray()  const { return array_; }
const JsonObject&             JsonValue::getObject() const { return object_; }

/* --------------------------------------------------------------- */
JsonParser::Pos JsonParser::currentPos(const std::string& src, size_t idx) {
    Pos p{1,1};
    for (size_t i = 0; i < idx && i < src.size(); ++i) {
        if (src[i] == '\n') { ++p.line; p.col = 1; }
        else                { ++p.col; }
    }
    return p;
}

/* --------------------------------------------------------------- */
std::shared_ptr<JsonValue> JsonParser::loadFromFile(const std::string& filename) {
    std::ifstream f(filename);
    if (!f) throw std::runtime_error("Cannot open file: " + filename);
    std::stringstream buf; buf << f.rdbuf();
    return parse(buf.str());
}

/* --------------------------------------------------------------- */
std::shared_ptr<JsonValue> JsonParser::parse(const std::string& json) {
    std::istringstream is(json);
    Pos pos{1,1};
    return parseValue(is, json, pos);
}

/* --------------------------------------------------------------- */
void JsonParser::skipWhitespace(std::istream& is,
                                const std::string& src, Pos& pos) {
    while (is.peek() != EOF) {
        char c = static_cast<char>(is.peek());
        if (!std::isspace(c)) break;
        is.get();
        if (c == '\n') { ++pos.line; pos.col = 1; }
        else           { ++pos.col; }
    }
}

/* --------------------------------------------------------------- */
std::shared_ptr<JsonValue> JsonParser::parseValue(std::istream& is,
                                                  const std::string& src,
                                                  Pos& pos) {
    skipWhitespace(is, src, pos);
    if (is.eof()) throw std::runtime_error("Unexpected end of input");

    char c = static_cast<char>(is.get());
    ++pos.col;

    if (c == '{') return std::make_shared<JsonValue>(parseObject(is, src, pos));
    if (c == '[') return std::make_shared<JsonValue>(parseArray (is, src, pos));
    if (c == '"') { is.unget(); return std::make_shared<JsonValue>(parseString(is, src, pos)); }
    if (c == 't' || c == 'f') { is.unget(); return std::make_shared<JsonValue>(parseBoolean(is, src, pos, c)); }
    if (c == 'n') { is.unget(); return parseNull(is, src, pos); }
    if (std::isdigit(c) || c == '-') { is.unget(); return std::make_shared<JsonValue>(parseNumber(is, src, pos)); }

    throw std::runtime_error("Unexpected character '" + std::string(1,c) + "'");
}

/* --------------------------------------------------------------- */
JsonObject JsonParser::parseObject(std::istream& is,
                                   const std::string& src, Pos& pos) {
    JsonObject obj;
    skipWhitespace(is, src, pos);
    if (is.peek() == '}') { is.get(); ++pos.col; return obj; }

    while (true) {
        skipWhitespace(is, src, pos);
        if (is.peek() != '"')
            throw std::runtime_error("Expected '\"' for object key");
        std::string key = parseString(is, src, pos);

        skipWhitespace(is, src, pos);
        if (is.get() != ':') {
            Pos p = currentPos(src, int(is.tellg()) - 1);
            throw std::runtime_error("Expected ':' after key (line "
                                     + std::to_string(p.line) + ", col " + std::to_string(p.col) + ")");
        }
        ++pos.col;

        obj[key] = parseValue(is, src, pos);

        skipWhitespace(is, src, pos);
        char sep = static_cast<char>(is.get());
        ++pos.col;
        if (sep == '}') break;
        if (sep != ',')
            throw std::runtime_error("Expected ',' or '}' in object");
    }
    return obj;
}

/* --------------------------------------------------------------- */
JsonArray JsonParser::parseArray(std::istream& is,
                                 const std::string& src, Pos& pos) {
    JsonArray arr;
    skipWhitespace(is, src, pos);
    if (is.peek() == ']') { is.get(); ++pos.col; return arr; }

    while (true) {
        arr.push_back(parseValue(is, src, pos));
        skipWhitespace(is, src, pos);
        char sep = static_cast<char>(is.get());
        ++pos.col;
        if (sep == ']') break;
        if (sep != ',')
            throw std::runtime_error("Expected ',' or ']' in array");
    }
    return arr;
}

/* --------------------------------------------------------------- */
std::string JsonParser::parseString(std::istream& is,
                                    const std::string& src, Pos& pos) {
    if (is.get() != '"') throw std::runtime_error("Internal error: parseString called without opening quote");
    ++pos.col;

    std::string s;
    char c;
    while (is.get(c)) {
        ++pos.col;
        if (c == '"') return s;
        if (c == '\\') {
            if (!is.get(c)) throw std::runtime_error("Unterminated escape sequence");
            ++pos.col;
            switch (c) {
                case '"': case '\\': case '/': s += c; break;
                case 'b': s += '\b'; break;
                case 'f': s += '\f'; break;
                case 'n': s += '\n'; break;
                case 'r': s += '\r'; break;
                case 't': s += '\t'; break;
                case 'u': {
                    std::string hex(4,' ');
                    if (is.read(&hex[0],4).gcount() != 4)
                        throw std::runtime_error("Incomplete Unicode escape");
                    pos.col += 4;
                    s += decodeUnicode(hex);
                    break;
                }
                default: throw std::runtime_error("Invalid escape sequence");
            }
        } else {
            s += c;
        }
    }
    throw std::runtime_error("Unterminated string");
}

/* --------------------------------------------------------------- */
bool JsonParser::parseBoolean(std::istream& is,
                              const std::string& src, Pos& pos, char first) {
    std::string token{first};
    char c;
    while (std::isalpha(is.peek())) {
        is.get(c); token += c; ++pos.col;
    }
    if (token == "true")  return true;
    if (token == "false") return false;
    throw std::runtime_error("Invalid boolean: " + token);
}

/* --------------------------------------------------------------- */
std::shared_ptr<JsonValue> JsonParser::parseNull(std::istream& is,
                                                 const std::string& src, Pos& pos) {
    std::string lit = "null";
    for (char expected : lit.substr(1)) {
        if (is.get() != expected) throw std::runtime_error("Invalid null");
        ++pos.col;
    }
    return std::make_shared<JsonValue>();
}

/* --------------------------------------------------------------- */
double JsonParser::parseNumber(std::istream& is,
                               const std::string& src, Pos& pos) {
    std::string num;
    bool hasDigit = false;
    while (is.peek() != EOF) {
        char c = static_cast<char>(is.peek());
        if (! (std::isdigit(c) || c=='.' || c=='e' || c=='E' || c=='+' || c=='-')) break;
        is.get(); num += c; ++pos.col;
        if (std::isdigit(c)) hasDigit = true;
    }
    if (!hasDigit) throw std::runtime_error("Number without digits");
    try {
        size_t idx;
        double d = std::stod(num, &idx);
        if (idx != num.size()) throw std::runtime_error("Trailing junk in number");
        return d;
    } catch (...) {
        throw std::runtime_error("Invalid number format");
    }
}

/* --------------------------------------------------------------- */
std::string JsonParser::decodeUnicode(const std::string& hex) {
    uint32_t cp = static_cast<uint32_t>(std::stoul(hex, nullptr, 16));
    std::string utf8;
    if (cp <= 0x7F) {
        utf8 += static_cast<char>(cp);
    } else if (cp <= 0x7FF) {
        utf8 += static_cast<char>(0xC0 | (cp >> 6));
        utf8 += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0xFFFF) {
        utf8 += static_cast<char>(0xE0 | (cp >> 12));
        utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp <= 0x10FFFF) {
        utf8 += static_cast<char>(0xF0 | (cp >> 18));
        utf8 += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        utf8 += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        utf8 += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        throw std::runtime_error("Unicode codepoint out of range");
    }
    return utf8;
}

/* --------------------------------------------------------------- */
std::string correctJson(const std::string& json) {
    std::string out;
    bool inStr = false, esc = false;
    for (size_t i = 0; i < json.size(); ++i) {
        char c = json[i];
        if (esc) { out += c; esc = false; continue; }
        if (c == '\\') { esc = true; out += c; continue; }
        if (c == '"' && !esc) { inStr = !inStr; }
        out += c;
        if (!inStr && (c == '}' || c == ']') && i+1 < json.size()) {
            char next = json[i+1];
            if (next != ',' && next != '}' && next != ']') out += ',';
        }
    }
    return out;
}
