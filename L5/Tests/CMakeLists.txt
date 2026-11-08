include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(TestUndoRedo
        TestUndoRedo.cpp
)

target_include_directories(TestUndoRedo PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TestUndoRedo PRIVATE
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(TestUndoRedo)