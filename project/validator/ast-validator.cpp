#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <schema_file> <ast_file>" << std::endl;
        return 1;
    }

    std::string schemaFile = argv[1];
    std::string astFile = argv[2];

    // Read schema file
    std::ifstream schemaStream(schemaFile);
    if (!schemaStream.is_open()) {
        std::cerr << "Failed to open schema file: " << schemaFile << std::endl;
        return 1;
    }

    json schemaJson;
    try {
        schemaStream >> schemaJson;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse schema file: " << e.what() << std::endl;
        return 1;
    }

    // Read AST file
    std::ifstream astStream(astFile);
    if (!astStream.is_open()) {
        std::cerr << "Failed to open AST file: " << astFile << std::endl;
        return 1;
    }

    json astJson;
    try {
        astStream >> astJson;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse AST file: " << e.what() << std::endl;
        return 1;
    }

    // Set up validation
    valijson::Schema schema;
    valijson::SchemaParser parser;
    valijson::adapters::NlohmannJsonAdapter schemaAdapter(schemaJson);

    try {
        parser.populateSchema(schemaAdapter, schema);
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse schema: " << e.what() << std::endl;
        return 1;
    }

    // Validate AST against schema
    valijson::Validator validator;
    valijson::adapters::NlohmannJsonAdapter astAdapter(astJson);

    valijson::ValidationResults results;
    if (!validator.validate(schema, astAdapter, &results)) {
        std::cerr << "Validation failed:" << std::endl;

        // Print validation errors
        valijson::ValidationResults::Error error;
        unsigned int errorNum = 1;
        while (results.popError(error)) {
            std::string context;
            for (const std::string& contextElement : error.context) {
                context += contextElement + " ";
            }

            std::cerr << "Error #" << errorNum++ << std::endl;
            std::cerr << "  Context: " << context << std::endl;
            std::cerr << "  Description: " << error.description << std::endl;
            std::cerr << std::endl;
        }

        return 1;
    }

    std::cout << "Validation successful!" << std::endl;
    return 0;
}
