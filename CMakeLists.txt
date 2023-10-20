cmake_minimum_required(VERSION 3.11)

# Project name
project(namecheap-cli)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add cpr and nlohmann_json as dependencies
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

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
