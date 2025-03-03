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

# Source files
set(SOURCES
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp
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

# Add test executable
add_executable(lexer_tests tests/unit/lexer/LexerTest.cpp)
target_link_libraries(lexer_tests tinyc ${GTEST_LIBRARIES} pthread)

# Register tests
gtest_discover_tests(lexer_tests)

# Install targets
install(TARGETS tinyc tinyc-compiler
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Install headers
install(DIRECTORY include/ DESTINATION include)