# FindWayland.cmake
# Advanced Wayland library discovery module

include(CheckCSourceCompiles)
include(FindPackageHandleStandardArgs)

# Search paths
set(WAYLAND_SEARCH_PATHS
    /usr
    /usr/local
    /opt
    /opt/local
    $ENV{WAYLAND_ROOT}  # Allow user-defined paths via environment variable
)

# Find Wayland core libraries and headers
find_path(Wayland_CLIENT_INCLUDE_DIR
    NAMES wayland-client.h
    PATHS ${WAYLAND_SEARCH_PATHS}
    PATH_SUFFIXES include
)

find_path(Wayland_SERVER_INCLUDE_DIR
    NAMES wayland-server.h
    PATHS ${WAYLAND_SEARCH_PATHS}
    PATH_SUFFIXES include
)

find_library(Wayland_CLIENT_LIBRARY
    NAMES wayland-client
    PATHS ${WAYLAND_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

find_library(Wayland_SERVER_LIBRARY
    NAMES wayland-server
    PATHS ${WAYLAND_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

# Find additional Wayland protocols and extensions
find_library(Wayland_CURSOR_LIBRARY
    NAMES wayland-cursor
    PATHS ${WAYLAND_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

find_library(Wayland_EGL_LIBRARY
    NAMES wayland-egl
    PATHS ${WAYLAND_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

# Version detection
if(Wayland_CLIENT_INCLUDE_DIR)
    file(READ "${Wayland_CLIENT_INCLUDE_DIR}/wayland-version.h" _WAYLAND_VERSION_CONTENT)
    
    if(_WAYLAND_VERSION_CONTENT)
        string(REGEX MATCH "WAYLAND_VERSION[ ]*\"([0-9]+\\.[0-9]+\\.[0-9]+)\"" _VERSION_MATCH ${_WAYLAND_VERSION_CONTENT})
        set(Wayland_VERSION ${CMAKE_MATCH_1})
    endif()
endif()

# Compile test to verify Wayland functionality
set(CMAKE_REQUIRED_INCLUDES ${Wayland_CLIENT_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Wayland_CLIENT_LIBRARY})

check_c_source_compiles("
    #include <wayland-client.h>
    int main() {
        struct wl_display *display = wl_display_connect(NULL);
        if (!display) return 1;
        wl_display_disconnect(display);
        return 0;
    }
" WAYLAND_COMPILE_TEST)

# Set package findings
find_package_handle_standard_args(Wayland
    FOUND_VAR Wayland_FOUND
    REQUIRED_VARS 
        Wayland_CLIENT_LIBRARY 
        Wayland_CLIENT_INCLUDE_DIR
        WAYLAND_COMPILE_TEST
    VERSION_VAR Wayland_VERSION
)

# Aggregate libraries and include directories
if(Wayland_FOUND)
    set(Wayland_LIBRARIES 
        ${Wayland_CLIENT_LIBRARY}
        ${Wayland_SERVER_LIBRARY}
        ${Wayland_CURSOR_LIBRARY}
        ${Wayland_EGL_LIBRARY}
    )

    set(Wayland_INCLUDE_DIRS
        ${Wayland_CLIENT_INCLUDE_DIR}
        ${Wayland_SERVER_INCLUDE_DIR}
    )

    # Optional components detection
    set(Wayland_CURSOR_FOUND ${Wayland_CURSOR_LIBRARY})
    set(Wayland_EGL_FOUND ${Wayland_EGL_LIBRARY})
endif()

# Advanced options
mark_as_advanced(
    Wayland_CLIENT_INCLUDE_DIR
    Wayland_SERVER_INCLUDE_DIR
    Wayland_CLIENT_LIBRARY
    Wayland_SERVER_LIBRARY
    Wayland_CURSOR_LIBRARY
    Wayland_EGL_LIBRARY
)