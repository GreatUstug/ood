cmake_minimum_required(VERSION 4.1)
project(L4)

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

add_executable(L4 main.cpp
        Model/Picture.h
        Model/Figures/IShapeGeometry.h
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
        View/FileDialog.h
        Model/MakeShapeUtils.h
        Model/ShapeInfo.h
        Observers/EditorStateObserver.h
        View/ShapeListView.h
        Controller/DocumentController.h
        Controller/ShapeTransformController.h
        Controller/ShapeCreateController.h
        Controller/ShortcutController.h
        Controller/InputRouter.h
        Controller/EditorWindowController.h
        Controller/AppCommand.h
        Model/Figures/IShapeGeometryVisitor.h
        View/DrawVisitor.h
        Commands/ICommand.h
        Commands/AddCommand.h
        Commands/CommandHistory.h
        Commands/DeleteCommand.h
        Commands/MoveShapesCommand.h
        Commands/ResizeCommand.h
        Model/Figures/Line.h
        Model/Figures/Polygon.h)
target_include_directories(L4 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(L4 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)