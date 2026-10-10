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
        Model/Picture.h
        Model/Figures/IShapeGeometry.h
        ShapeParams.h
        Model/Figures/Ellipse.h
        Model/Figures/Rectangle.h
        Model/Figures/Triangle.h
        View/gfx/ICanvas.h
        View/gfx/SFMLCanvas.h
        Model/Bounds.h
        Model/EditorState.h
        View/DrawSelectionFrame.h
        View/EditorView.h
        Controller/EditorController.h
        IO/SaveService.h
        IO/LoadService.h
        View/Toolbar.h
        Model/Handle.h
        Model/ResizePolicy.h
        View/CursorType.h
        IO/DocumentIO.h
        View/FileDialog.h)
target_include_directories(L3 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(L3 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)