cmake_minimum_required(VERSION 4.1)
project(L3)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(SFML)

add_executable(L3 main.cpp
        Shapes/Picture.h
        Shapes/Figures/IShapeGeometry.h
        ShapeParams.h
        Shapes/Figures/Ellipse.h
        Shapes/Figures/Rectangle.h
        Shapes/Figures/Triangle.h
        gfx/ICanvas.h
        gfx/SFMLCanvas.h
        Shapes/Bounds.h
        State/EditorState.h
        State/DrawSelectionFrame.h
        View/EditorView.h
        Controller/EditorController.h
        IO/SaveService.h
        IO/LoadService.h
        View/Toolbar.h
        State/Handle.h
        Shapes/ResizePolicy.h
        View/CursorType.h)
target_include_directories(L3 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(L3 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)