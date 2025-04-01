cmake_minimum_required(VERSION 3.14)
project(tinyc-ast-validator VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# FetchContent for dependencies
include(FetchContent)

# Fetch nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Fetch valijson
FetchContent_Declare(
    valijson
    GIT_REPOSITORY https://github.com/tristanpenman/valijson.git
    GIT_TAG v1.0.1
)
FetchContent_MakeAvailable(valijson)

# Executable target
add_executable(ast-validator ast-validator.cpp)
target_link_libraries(ast-validator PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(ast-validator PRIVATE ${valijson_SOURCE_DIR}/include)

# Install target
install(TARGETS ast-validator
        RUNTIME DESTINATION bin)
