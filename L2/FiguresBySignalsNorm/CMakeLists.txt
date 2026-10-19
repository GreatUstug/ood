cmake_minimum_required(VERSION 4.1)
project(FiguresBySignalsNorm)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
add_subdirectory(Tests)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

add_executable(FiguresBySignalsNorm main.cpp
        ShapesCommandHandler.h
        Shapes/Picture.h
        Shapes/Figures/IShapeGeometry.h
        ShapeParams.h
        Shapes/Figures/Circle.h
        Shapes/Figures/Rectangle.h
        Shapes/Figures/Line.h
        Shapes/Figures/Triangle.h
        Shapes/Figures/Text.h
        gfx/ICanvas.h
        gfx/SFMLCanvas.h
        Observer/IFigureObserver.h
        Observer/IPictureObserver.h
        Observer/ObserverList.h
        Observer/ConsolePictureObserver.h
        Observer/Subscription.h
        Observer/ObserverListById.h
        Observer/SubscriptionById.h
        Observer/Event.h
)
target_include_directories(FiguresBySignalsNorm PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(FiguresBySignalsNorm PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)