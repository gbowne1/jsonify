#include <iostream>
#include <exception>
#include <string>
#include "jsonparser.h"
#include "jsonformatter.h"

// Function to process a JSON object
void processJsonObject(const std::shared_ptr<JsonValue>& jsonValue) {
    if (jsonValue->getType() == JsonValue::Type::Object) {
        const auto &obj = jsonValue->getObject();
        std::cout << "Loaded JSON keys and values:" << std::endl;

        for (const auto &pair : obj) {
            const std::string &key = pair.first;
            const auto &value = pair.second;

            // Print the key
            std::cout << "Key: " << key << " - ";

            // Check the type of the value and print accordingly
            switch (value->getType()) {
                case JsonValue::Type::String:
                    std::cout << "Value: " << value->getString() << std::endl;
                    break;
                case JsonValue::Type::Number:
                    std::cout << "Value: " << value->getNumber() << std::endl;
                    break;
                case JsonValue::Type::Bool:
                    std::cout << "Value: " << (value->getBool() ? "true" : "false") << std::endl;
                    break;
                case JsonValue::Type::Object:
                    std::cout << "Value: [Object]" << std::endl;
                    // Optionally, you can recursively print the object here
                    break;
                case JsonValue::Type::Array:
                    std::cout << "Value: [Array]" << std::endl;
                    // Optionally, you can iterate through the array and print its contents
                    break;
                default:
                    std::cout << "Value: [Unknown Type]" << std::endl;
                    break;
            }
        }
    } else {
        std::cout << "Loaded JSON is not an object." << std::endl;
    }
}

int main(int argc, char *argv[]) {   
    /* Check if a filename is provided as a command-line argument */
    if (argc > 1) {
        std::string filename = argv[1];

        try {
            auto jsonValue = JsonParser::loadFromFile(filename);
            std::cout << "Loaded JSON from file: " << filename << std::endl; // Debug output
            processJsonObject(jsonValue); // Call the function to process the JSON object
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } else {
        // Fallback to hardcoded JSON string if no filename is provided
        std::string jsonStr = R"(
            {
                "key": "value"
            }
        )";

        try {
            std::cout << "Using hardcoded JSON string." << std::endl; // Debug output
            auto jsonValue = JsonParser::parse(jsonStr); // Parse the hardcoded JSON string
            processJsonObject(jsonValue); // Call the function to process the JSON object
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
