add_executable(my_tests
        TestTask1.cpp
        TestTask2.cpp
#        TestTask3.cpp
#        Lab1Shape.cpp
#        Lab1Picture.cpp
#        MockCanvas.h
)

target_link_libraries(my_tests
        PRIVATE
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
)

gtest_discover_tests(my_tests)