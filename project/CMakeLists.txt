cmake_minimum_required(VERSION 3.10)
project(tinyc-compiler VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compile options
if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()

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
        src/ast/ASTNode.cpp
        src/ast/visitors/JSONVisitor.cpp
        src/ast/visitors/DumpVisitor.cpp
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

# TESTS
enable_testing()

# Find GTest
find_package(GTest REQUIRED)
include(GoogleTest)

# Make sure we can find GTest headers
message(STATUS "GTest include dirs: ${GTEST_INCLUDE_DIRS}")

# Add the GTest include directories to all targets
include_directories(${GTEST_INCLUDE_DIRS})

# For macOS with Homebrew, try an additional include path if needed
if (APPLE)
    include_directories(SYSTEM /opt/homebrew/include)
endif ()

# Define test libraries (using variables from FindGTest)
set(TEST_LIBRARIES tinyc ${GTEST_BOTH_LIBRARIES} pthread)

# Create directory structure if it doesn't exist
file(MAKE_DIRECTORY tests/lexer)
file(MAKE_DIRECTORY tests/parser)

# Add lexer test executable
add_executable(lexer_tests tests/lexer/LexerTest.cpp)
target_link_libraries(lexer_tests ${TEST_LIBRARIES})
target_include_directories(lexer_tests PRIVATE ${GTEST_INCLUDE_DIRS})
gtest_discover_tests(lexer_tests)

# Define parser test sources
set(PARSER_TEST_SOURCES
        tests/parser/ParserTest.cpp
        tests/parser/ParserTypeTests.cpp
        tests/parser/ParserExpressionTests.cpp
        tests/parser/ParserStatementTests.cpp
        tests/parser/ParserDeclarationTests.cpp
)

# Create separate test executables for each parser test file
foreach (TEST_SOURCE ${PARSER_TEST_SOURCES})
    # Extract the file name without extension
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    # Create an executable for this test file
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} ${TEST_LIBRARIES})
    target_include_directories(${TEST_NAME} PRIVATE ${GTEST_INCLUDE_DIRS})

    # Register the test with CTest
    gtest_discover_tests(${TEST_NAME})
endforeach ()

# Install targets
install(TARGETS tinyc tinyc-compiler
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Install headers
install(DIRECTORY include/ DESTINATION include)