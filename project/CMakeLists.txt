cmake_minimum_required(VERSION 3.10)
project(tinyc-compiler VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compile options
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include directories
include_directories(include)

# Source files - Lexer
set(LEXER_SOURCES
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
)

# Source files - Parser
set(PARSER_SOURCES
    src/parser/Parser.cpp
    src/parser/ParserTypeNodes.cpp
    src/parser/ParserDeclarationNodes.cpp
    src/parser/ParserStatementNodes.cpp
    src/parser/ParserExpressionNodes.cpp
)

# Source files - AST
set(AST_SOURCES
    src/ast/ProgramNode.cpp
    src/ast/BaseNodes.cpp
    src/ast/TypeNodes.cpp
    src/ast/DeclarationNodes.cpp
    src/ast/StatementNodes.cpp
    src/ast/ExpressionNodes.cpp
)

# Combine all sources
set(SOURCES
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${AST_SOURCES}
)

# Library target
add_library(tinyc ${SOURCES})
target_include_directories(tinyc PUBLIC include)

# Executable target
add_executable(tinyc-compiler src/main.cpp)
target_link_libraries(tinyc-compiler tinyc)

# Tests
enable_testing()

# Find GTest
find_package(GTest REQUIRED)
include(GoogleTest)
include_directories(${GTEST_INCLUDE_DIRS})

# Add lexer test executable
add_executable(lexer_tests tests/unit/lexer/LexerTest.cpp)
target_link_libraries(lexer_tests tinyc ${GTEST_LIBRARIES} pthread)

# Add parser test executable
add_executable(parser_tests tests/unit/parser/ParserTest.cpp)
target_link_libraries(parser_tests tinyc ${GTEST_LIBRARIES} pthread)

# Register tests with CTest
gtest_discover_tests(lexer_tests)
gtest_discover_tests(parser_tests)

# Install targets
install(TARGETS tinyc tinyc-compiler
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Install headers
install(DIRECTORY include/ DESTINATION include)