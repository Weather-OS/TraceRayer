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

pkg_check_modules(ADWAITA REQUIRED libadwaita-1)
include_directories(${ADWAITA_INCLUDE_DIRS})
link_directories(${ADWAITA_LIBRARY_DIRS})

add_executable(TraceRayer main.c
        Source/IO/Arguments.c
        Source/IO/Logging.c
        Source/IO/Path.c
        Source/IO/FetchResources.c
        Source/UI/GTK/GTKWindow.c
        Source/UI/GTK/GTK.c
        Source/UI/GTK/GTKWidget.c
        Source/UI/GTK/GTKDrawingArea.c
        Source/UI/GTK/GTKWindowHandle.c
        Source/UI/GTK/GTKPicture.c
        Source/Core/ActivationLoop.c
        Source/Core/Async/AsyncOperation.c
        Source/Core/Async/AsyncState.c
        Source/Core/Async/AsyncInfo.c
        Source/Core/Splash/SplashWindow.c
)

target_include_directories(TraceRayer PRIVATE ${CMAKE_SOURCE_DIR}/Include)
target_link_libraries(TraceRayer ${GTK4_LIBRARIES})
target_link_libraries(TraceRayer ${UUID_LIBRARIES})
target_link_libraries(TraceRayer ${ADWAITA_LIBRARIES})

target_compile_definitions(TraceRayer PRIVATE
        RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/share/TraceRayer/Resources"
)

install( TARGETS TraceRayer
         RUNTIME DESTINATION bin )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Resources
        DESTINATION share/TraceRayer)