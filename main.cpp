#include <iostream>
#include <exception>
#include <string>
#include "jsonparser.h"

int main(int argc, char *argv[])
{   
    /* Check if a filename is provided as a command-line argument */
    if (argc > 1)
    {
        std::string filename = argv[1];

        try
        {
            auto jsonValue = JsonParser::loadFromFile(filename);
            if (jsonValue->getType() == JsonValue::Type::Object)
            {
                const auto &obj = jsonValue->getObject();
                std::cout << "Name: " << obj.at("name")->getString() << std::endl;
                std::cout << "Age: " << obj.at("age")->getNumber() << std::endl;
                std::cout << "Is Student: " << (obj.at("isStudent")->getBool() ? "Yes" : "No") << std::endl;

                std::cout << "Grades: ";
                for (const auto &grade : obj.at("grades")->getArray())
                {
                    std::cout << grade->getNumber() << " ";
                }
                std::cout << std::endl;

                const auto &address = obj.at("address")->getObject();
                std::cout << "Street: " << address.at("street")->getString() << std::endl;
                std::cout << "Zip: " << address.at("zip")->getString() << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    else
    {
        // Fallback to hardcoded JSON string if no filename is provided
        std::string jsonStr = R"(
            {
                "name": "John Doe",
                "age": 30,
                "city": "New York",
                "isStudent": false,
                "grades": [85, 90, 78],
                "address": {
                    "street": "123 Main St",
                    "zip": "10001"
                }
            }
        )";

        try
        {
            auto jsonValue = JsonParser::parse(jsonStr); // Parse the hardcoded JSON string
            if (jsonValue->getType() == JsonValue::Type::Object)
            {
                const auto &obj = jsonValue->getObject();
                std::cout << "Name: " << obj.at("name")->getString() << std::endl;
                std::cout << "Age: " << obj.at("age")->getNumber() << std::endl;
                std::cout << "Is Student: " << (obj.at("isStudent")->getBool() ? "Yes" : "No") << std::endl;

                std::cout << "Grades: ";
                for (const auto &grade : obj.at("grades")->getArray())
                {
                    std::cout << grade->getNumber() << " ";
                }
                std::cout << std::endl;

                const auto &address = obj.at("address")->getObject();
                std::cout << "Street: " << address.at("street")->getString() << std::endl;
                std::cout << "Zip: " << address.at("zip")->getString() << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
