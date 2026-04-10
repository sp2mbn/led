cmake_minimum_required(VERSION 3.10)
project(test_ezLEDSequence)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG        v2.6.1
)

FetchContent_MakeAvailable(unity)

include_directories(
    ${CMAKE_SOURCE_DIR}/../src
    ${unity_SOURCE_DIR}/src
)

add_executable(test_ezLEDSequence
    ../test/test_ezLEDSequence.cpp
    ../src/ezLED.cpp
    ../src/ezLEDSequence.cpp
)

target_link_libraries(test_ezLEDSequence unity)

target_compile_definitions(test_ezLEDSequence PRIVATE ARDUINO=100)

target_include_directories(test_ezLEDSequence PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

add_test(NAME test_ezLEDSequence COMMAND test_ezLEDSequence)