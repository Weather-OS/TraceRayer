cmake_minimum_required(VERSION 3.5)
project(TraceRayer LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

find_package(PkgConfig REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GTK REQUIRED)

pkg_check_modules(UUID REQUIRED uuid)
include_directories(${UUID_INCLUDE_DIRS})
link_directories(${UUID_LIBRARY_DIRS})

pkg_check_modules(ADWAITA REQUIRED libadwaita-1)
include_directories(${ADWAITA_INCLUDE_DIRS})
link_directories(${ADWAITA_LIBRARY_DIRS})

include_directories(TraceRayer PRIVATE ${CMAKE_SOURCE_DIR}/Include)

add_library(options INTERFACE)
target_compile_options( options INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers -Wno-deprecated-declarations -fvisibility=hidden>
        $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers -Wno-deprecated-declarations -fvisibility=hidden>

        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O3> )

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

target_link_libraries(comui options)
target_link_libraries(comui ${GTK4_LIBRARIES})
target_link_libraries(comui ${UUID_LIBRARIES})
target_link_libraries(comui ${ADWAITA_LIBRARIES})

# comasync
add_library( comasync SHARED
        Source/Core/Async/AsyncOperation.c
        Source/Core/Async/AsyncState.c
        Source/Core/Async/AsyncInfo.c
        Source/Core/Async/AsyncOperationCompletedHandlerDefault.c )

target_link_libraries(comasync options)
target_link_libraries(comasync ${GTK4_LIBRARIES})
target_link_libraries(comasync ${UUID_LIBRARIES})
target_link_libraries(comasync ${ADWAITA_LIBRARIES})

# comvulkan
add_library( comvulkan SHARED
        Source/Core/Vulkan/Vulkan.c
        Source/Core/Vulkan/VulkanDevice.c )

target_include_directories(comvulkan PRIVATE ${Vulkan_INCLUDE_DIRS})

target_link_libraries(comvulkan options)
target_link_libraries(comvulkan ${UUID_LIBRARIES})
target_link_libraries(comvulkan ${Vulkan_LIBRARIES})

# TraceRayer
add_executable(TraceRayer main.c
        Source/IO/Arguments.c
        Source/IO/Logging.c
        Source/IO/Path.c
        Source/IO/FetchResources.c
        Source/Application/Application.cpp
        Source/Application/ActivationLoop.cpp
        Source/Application/Splash/SplashWindow.cpp )

target_compile_options(TraceRayer PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-O3 -fvisibility=hidden>
        $<$<COMPILE_LANGUAGE:C>:-O2 -fvisibility=hidden> )

target_link_libraries(TraceRayer ${UUID_LIBRARIES})
target_link_libraries(TraceRayer comui)
target_link_libraries(TraceRayer comasync)
target_link_libraries(TraceRayer comvulkan)

target_compile_definitions(TraceRayer PRIVATE
        RESOURCE_DIR=\"${CMAKE_INSTALL_PREFIX}/share/TraceRayer/Resources\" )

install( TARGETS TraceRayer
         RUNTIME DESTINATION bin )

install( DIRECTORY ${CMAKE_SOURCE_DIR}/Resources
         DESTINATION share/TraceRayer )