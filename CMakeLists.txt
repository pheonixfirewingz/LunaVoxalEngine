cmake_minimum_required(VERSION 3.20)
project(LunaVoxalEngine VERSION 1.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Project Configuration
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(USE_WAYLAND "Use Wayland on Linux (otherwise X11)" OFF)
option(ENABLE_MOBILE_PLATFORMS "Build for mobile platforms" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

if(LINUX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nodefaultlibs -lc -fno-exceptions")
endif()

# Detect platform and set platform-specific variables
if(WIN32)
    set(PLATFORM_NAME "win32")
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DON_WINDOWS)
elseif(APPLE)
    if(IOS)
        set(PLATFORM_NAME "ios")
        add_definitions(-DON_IOS)
    else()
        set(PLATFORM_NAME "mac")
        add_definitions(-DON_MAC)
    endif()
elseif(ANDROID)
    set(PLATFORM_NAME "android")
    add_definitions(-DON_ANDROID)
elseif(LINUX)
    set(PLATFORM_NAME "linux")
    add_definitions(-DON_LINUX)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Gather platform-specific source files
file(GLOB PLATFORM_SOURCES
    "src/platform/${PLATFORM_NAME}.*"
    "src/platform/${PLATFORM_NAME}_*.*"
    "src/platform/common_*.*"
)

# Gather renderer sources
file(GLOB RENDERER_SOURCES
    "src/renderer/vulkan/*.h"
    "src/renderer/vulkan/*.cpp"
    "src/renderer/*.h" 
    "src/renderer/*.cpp"
    "src/renderer/third_party/volk/volk.cpp"
)

# Gather main sources
file(GLOB MAIN_SOURCES
    "src/utils/string.h"
    "src/utils/string.cpp"
    "src/utils/algorithm.h"
)

# Platform-specific libraries
if(WIN32)
    set(PLATFORM_LIBS 
        kernel32.lib 
        user32.lib 
        gdi32.lib 
        winmm.lib
    )
elseif(LINUX)
    if(USE_WAYLAND)
        set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
        find_package(Wayland REQUIRED)
        add_definitions(-DUSE_WAYLAND)
        list(APPEND PLATFORM_LIBS ${Wayland_CLIENT_LIBRARY})
        list(APPEND PLATFORM_INCLUDES ${Wayland_INCLUDE_DIR})
    else()
        include(FindPkgConfig)
        pkg_check_modules(XCB xcb)
        list(APPEND PLATFORM_LIBS ${XCB_LIBRARIES})
        list(APPEND PLATFORM_INCLUDES ${XCB_INCLUDE_DIR})
    endif()
    
    #broken see FindPipewire.cmake
    #find_package(Pipewire REQUIRED)
    #list(APPEND PLATFORM_LIBS ${PIPEWIRE_LIBRARIES})
    
    list(APPEND PLATFORM_LIBS pthread dl)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(AUDIO_LIBRARY AudioToolbox)
    
    list(APPEND PLATFORM_LIBS 
        ${COCOA_LIBRARY}
        ${METAL_LIBRARY}
        ${AUDIO_LIBRARY}
    )
    
    list(APPEND PLATFORM_INCLUDES
        ${COCOA_INCLUDE_DIR}
        ${METAL_INCLUDE_DIR}
        ${AUDIO_INCLUDE_DIR}
    )
endif()

# Mobile platform configuration
if(ENABLE_MOBILE_PLATFORMS)
    if(ANDROID)
        set(CMAKE_SYSTEM_NAME Android)
        set(CMAKE_SYSTEM_VERSION 28)
        set(ANDROID_ABI "arm64-v8a")
        
        list(APPEND PLATFORM_LIBS 
            android 
            log
        )
    elseif(IOS)
        set(IOS_DEPLOYMENT_TARGET "13.0")
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    endif()
endif()

# Main executable
add_executable(${PROJECT_NAME}
    ${MAIN_SOURCES}
    ${PLATFORM_SOURCES}
    ${RENDERER_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    "src/renderer/third_party/vk_headers/include"
    "src/renderer/third_party/volk"
    "src/renderer/third_party/vma/include"
    ${PLATFORM_INCLUDES}
    src
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${PLATFORM_LIBS}
)
if(LINUX OR APPLE)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

# Install configuration
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

# Packaging
include(CPack)
set(CPACK_GENERATOR "ZIP;TGZ")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# Print out detected platform sources for debugging
message(STATUS "Platform: ${PLATFORM_NAME}")
message(STATUS "Platform Sources: ${PLATFORM_SOURCES}")