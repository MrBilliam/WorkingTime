cmake_minimum_required(VERSION 3.20)

project(WorkingTimeTest)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Ensure the compiler supports it
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable non-standard compiler-specific extensions

# Find Catch2 (assuming it's installed via CMake or FetchContent)
find_package(Catch2 REQUIRED)

# add the tests
add_executable(TestCommands test_command_parsing.cpp)

# add includes
target_include_directories(TestCommands PRIVATE ../src)
# linking to catch2
target_link_libraries(TestCommands PRIVATE Catch2::Catch2WithMain)
