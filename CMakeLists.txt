cmake_minimum_required(VERSION 3.20)
project(LunaVoxalEngine VERSION 1.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Project Configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(ENABLE_VULKAN "Enable Vulkan renderer" ON)
option(USE_WAYLAND "Use Wayland on Linux (otherwise X11)" ON)
option(ENABLE_MOBILE_PLATFORMS "Build for mobile platforms" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

# Detect platform and set platform-specific variables
if(WIN32)
    set(PLATFORM_NAME "win32")
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(ON_WINDOWS)
elseif(APPLE)
    if(IOS)
        set(PLATFORM_NAME "ios")
        add_compile_definitions(ON_IOS)
    else()
        set(PLATFORM_NAME "mac")
        add_compile_definitions(ON_MAC)
    endif()
elseif(ANDROID)
    set(PLATFORM_NAME "android")
    add_compile_definitions(ON_ANDROID)
elseif(LINUX)
    set(PLATFORM_NAME "linux")
    add_compile_definitions(ON_LINUX)
    if(USE_WAYLAND)
        add_compile_definitions(ON_WAYLAND)
    else()
        add_compile_definitions(ON_X11)
    endif()
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# Vulkan SDK Download and Install Macro
macro(download_vulkan_sdk)
    if(NOT Vulkan_FOUND)
        include(FetchContent)
        
        if(WIN32)
            set(VULKAN_SDK_URL "https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe")
            set(VULKAN_DOWNLOAD_PATH "${CMAKE_BINARY_DIR}/vulkan-sdk-installer.exe")
            
            if(NOT EXISTS ${VULKAN_DOWNLOAD_PATH})
                file(DOWNLOAD ${VULKAN_SDK_URL} ${VULKAN_DOWNLOAD_PATH} SHOW_PROGRESS)
                execute_process(
                    COMMAND ${VULKAN_DOWNLOAD_PATH} /S
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                    RESULT_VARIABLE VULKAN_INSTALL_RESULT
                )
                
                if(NOT VULKAN_INSTALL_RESULT EQUAL 0)
                    message(WARNING "Vulkan SDK installation failed")
                endif()
            endif()
        elseif(LINUX)
            find_program(APT_EXECUTABLE apt-get)
            find_program(WGET_EXECUTABLE wget)
            
           # if(APT_EXECUTABLE)
           #     execute_process(
           #         COMMAND wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo apt-key add -
           #         COMMAND sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.4.304-noble.list https://packages.lunarg.com/vulkan/1.4.304/lunarg-vulkan-1.4.304-noble.list
           #     )
           #     execute_process(
           #         COMMAND sudo apt-get update
           #         COMMAND sudo apt-get install -y vulkan-sdk
           #     )
           # endif()
        elseif(APPLE)
            find_program(BREW_EXECUTABLE brew)
            if(BREW_EXECUTABLE)
                execute_process(
                    COMMAND brew install molten-vk
                )
            endif()
        endif()
    endif()
endmacro()

# Gather platform-specific source files
file(GLOB PLATFORM_SOURCES

    "src/platform/${PLATFORM_NAME}.*"
    "src/platform/${PLATFORM_NAME}_*.*"
    "src/platform/common_*.*"
)

# Gather common platform sources
file(GLOB COMMON_PLATFORM_SOURCES 
    "src/platform/platform.h"
    "src/platform/platform_layer.cpp"
)

# Gather renderer sources
file(GLOB RENDERER_SOURCES
    "src/renderer/*.h" 
    "src/renderer/*.cpp"
)

# Gather main sources
file(GLOB MAIN_SOURCES)

# Download Vulkan SDK if enabled
if(ENABLE_VULKAN)
    download_vulkan_sdk()
endif()

# Find Vulkan
find_package(Vulkan REQUIRED)

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
        find_package(Wayland REQUIRED)
        add_definitions(-DUSE_WAYLAND)
        list(APPEND PLATFORM_LIBS ${WAYLAND_LIBRARIES})
    else()
        find_package(X11 REQUIRED)
        list(APPEND PLATFORM_LIBS ${X11_LIBRARIES})
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
    ${COMMON_PLATFORM_SOURCES}
    ${PLATFORM_SOURCES}
    ${RENDERER_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    ${Vulkan_INCLUDE_DIRS}
)

# Compile definitions
if(ENABLE_VULKAN)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_VULKAN)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    ${PLATFORM_LIBS}
)

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