cmake_minimum_required(VERSION 3.11)

# Project name
project(namecheap-cli)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add cpr and nlohmann_json as dependencies
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(nlohmann_json)

# tinyxml2
FetchContent_Declare(
  tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(tinyxml2)

# Executable
add_executable(namecheap-cli src/main.cpp)
# Link libraries
target_link_libraries(namecheap-cli PRIVATE cpr::cpr nlohmann_json::nlohmann_json tinyxml2)

# Add GoogleTest
add_subdirectory(tests/googletest)

# Include GoogleTest headers
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ./include)

# Add test cpp file
add_executable(runUnitTests tests/cli_tests.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests gtest gtest_main cpr::cpr nlohmann_json::nlohmann_json tinyxml2)

# Add a test to be run by CTest
add_test(NAME NamecheapTests COMMAND runUnitTests)
