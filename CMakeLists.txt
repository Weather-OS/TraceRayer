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
        IO/Arguments.c
        Include/IO/Arguments.h
        Include/Types.h
        IO/Logging.c
        Include/IO/Logging.h
        IO/Path.c
        Include/IO/Path.h
        Include/Statics.h
        UI/Window/GTKWindow.c
        Include/UI/Window/GTKWindow.h
        Include/Object.h
        Include/UI/GTK.h
        UI/GTK.c
        Include/UI/GTKWidget.h
        Include/InitGUID.h)

target_include_directories(TraceRayer PRIVATE ${CMAKE_SOURCE_DIR}/Include)
target_link_libraries(TraceRayer ${GTK4_LIBRARIES})
target_link_libraries(TraceRayer ${UUID_LIBRARIES})