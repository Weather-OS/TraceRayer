cmake_minimum_required(VERSION 3.5)
project(TraceRayer C)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED gtk4)
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

pkg_check_modules(UUID REQUIRED uuid)
include_directories(${UUID_INCLUDE_DIRS})
link_directories(${UUID_LIBRARY_DIRS})

add_executable(TraceRayer main.c
        Source/IO/Arguments.c
        Source/IO/Logging.c
        Source/IO/Path.c
        Source/UI/Window/GTKWindow.c
        Source/UI/GTK.c
        Source/UI/GTKWidget.c
        Source/Core/WindowLoop.c
        Source/Core/Async/AsyncOperation.c
        Source/Core/Async/AsyncState.c
        Source/Core/Async/AsyncInfo.c
)

target_include_directories(TraceRayer PRIVATE ${CMAKE_SOURCE_DIR}/Include)
target_link_libraries(TraceRayer ${GTK4_LIBRARIES})
target_link_libraries(TraceRayer ${UUID_LIBRARIES})