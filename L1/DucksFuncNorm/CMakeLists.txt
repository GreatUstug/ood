cmake_minimum_required(VERSION 4.1)
project(DucksFunc)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(DucksFunc
    main.cpp
    DuckFunctions.h
    DanceBehavior.h
    Duck.h
    DuckConstructBehavior.h
    FlyBehavior.h
    QuackBehavior.h
)

enable_testing()
include(GoogleTest)

add_executable(ducks_tests Tests/tests.cpp)
target_include_directories(ducks_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ducks_tests PRIVATE GTest::gtest_main)

gtest_discover_tests(ducks_tests)