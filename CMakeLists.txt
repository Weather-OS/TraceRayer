cmake_minimum_required(VERSION 3.5)
project(TraceRayer C)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK4 REQUIRED gtk4)
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})


add_executable(TraceRayer main.c
        IO/Arguments.c
        Include/Arguments.h
        Include/Types.h
        IO/Logging.c
        Include/Logging.h
        IO/Path.c
        Include/Path.h
        Include/Statics.h)

target_include_directories(TraceRayer PRIVATE ${CMAKE_SOURCE_DIR}/Include)
target_link_libraries(TraceRayer ${GTK4_LIBRARIES})