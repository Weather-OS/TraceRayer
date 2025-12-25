cmake_minimum_required(VERSION 3.5)
project(TraceRayer LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

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

enable_language(CXX)

# comui
add_library( comui SHARED
        Source/UI/GTK/GTKWindow.c
        Source/UI/GTK/GTK.c
        Source/UI/GTK/GTKBox.c
        Source/UI/GTK/GTKWidget.c
        Source/UI/GTK/GTKDrawingArea.c
        Source/UI/GTK/GTKWindowHandle.c
        Source/UI/GTK/GTKPicture.c
        Source/UI/GTK/GTKLabel.c
        Source/UI/GTK/GTKSpinner.c
        Source/UI/GTK/GTKOverlay.c )

target_include_directories(comui PRIVATE ${CMAKE_SOURCE_DIR}/Include)

target_link_libraries(comui ${GTK4_LIBRARIES})
target_link_libraries(comui ${UUID_LIBRARIES})
target_link_libraries(comui ${ADWAITA_LIBRARIES})

target_compile_options(comui PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wno-unused-parameter -O3 -fvisibility=hidden>
        $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Wno-unused-parameter -O2 -fvisibility=hidden>
)

# comasync
add_library( comasync SHARED
        Source/Core/Async/AsyncOperation.c
        Source/Core/Async/AsyncState.c
        Source/Core/Async/AsyncInfo.c )

target_include_directories(comasync PRIVATE ${CMAKE_SOURCE_DIR}/Include)

target_link_libraries(comasync ${GTK4_LIBRARIES})
target_link_libraries(comasync ${UUID_LIBRARIES})
target_link_libraries(comasync ${ADWAITA_LIBRARIES})

target_compile_options(comasync PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -O3 -fvisibility=hidden>
        $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -O2 -fvisibility=hidden>
)

# TraceRayer
add_executable(TraceRayer main.c
        Source/IO/Arguments.c
        Source/IO/Logging.c
        Source/IO/Path.c
        Source/IO/FetchResources.c
        Source/Core/Application.cpp
        Source/Core/ActivationLoop.cpp
        Source/Core/Splash/SplashWindow.cpp)

target_include_directories(TraceRayer PRIVATE ${CMAKE_SOURCE_DIR}/Include)

target_compile_options(TraceRayer PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-O3 -fvisibility=hidden>
        $<$<COMPILE_LANGUAGE:C>:-O2 -fvisibility=hidden>
)

target_link_libraries(TraceRayer ${UUID_LIBRARIES})
target_link_libraries(TraceRayer comui)
target_link_libraries(TraceRayer comasync)

target_compile_definitions(TraceRayer PRIVATE
        RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/share/TraceRayer/Resources"
)

install( TARGETS TraceRayer
         RUNTIME DESTINATION bin )

install(DIRECTORY ${CMAKE_SOURCE_DIR}/Resources
        DESTINATION share/TraceRayer)