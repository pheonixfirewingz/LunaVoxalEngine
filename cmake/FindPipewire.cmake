#TODO: fix it not finding libspa-0.2 binaries

# Advanced PipeWire library discovery module

include(CheckCSourceCompiles)
include(FindPackageHandleStandardArgs)

# Updated search paths
set(PIPEWIRE_SEARCH_PATHS
    /usr
    /usr/local
    /opt
    /opt/local
    $ENV{PIPEWIRE_ROOT} # Allow user-defined paths via environment variable
)


# Find SPA headers (since PipeWire depends on SPA)
find_path(Spa_INCLUDE_DIR
    NAMES spa/support/plugin.h
    PATHS ${PIPEWIRE_SEARCH_PATHS}
    PATH_SUFFIXES include/spa-0.2
)

# Find SPA library
find_library(Spa_LIBRARY
    NAMES spa-0.2
    PATHS ${PIPEWIRE_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

# Find PipeWire headers
find_path(Pipewire_INCLUDE_DIR
    NAMES pipewire/pipewire.h
    PATHS ${PIPEWIRE_SEARCH_PATHS}
    PATH_SUFFIXES include/pipewire-0.3
)

# Find PipeWire library
find_library(Pipewire_LIBRARY
    NAMES pipewire-0.3
    PATHS ${PIPEWIRE_SEARCH_PATHS}
    PATH_SUFFIXES lib lib64
)

# Fail explicitly if not found
if(NOT Pipewire_INCLUDE_DIR)
    message(FATAL_ERROR "PipeWire header not found. Please install libpipewire-0.3-dev.")
else()
    message(STATUS "PipeWire headers found at ${Pipewire_INCLUDE_DIR}")
endif()

# Display results
message(STATUS "Pipewire Include: ${Pipewire_INCLUDE_DIR}")
message(STATUS "Pipewire Library: ${Pipewire_LIBRARY}")
message(STATUS "SPA Include: ${Spa_INCLUDE_DIR}")
message(STATUS "SPA Library: ${Spa_LIBRARY}")

set(Pipewire_INCLUDE_DIRS ${Pipewire_INCLUDE_DIR} ${Spa_INCLUDE_DIR})
set(Pipewire_LIBRARIES ${Pipewire_LIBRARY} ${Spa_LIBRARY})

# Verify PipeWire functionality
set(CMAKE_REQUIRED_INCLUDES ${Pipewire_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Pipewire_LIBRARY})

check_c_source_compiles("
    #include <pipewire/pipewire.h>
    int main() {
        pw_init(NULL, NULL);
        pw_deinit();
        return 0;
    }
" PIPEWIRE_COMPILE_TEST)

# Handle findings
find_package_handle_standard_args(Pipewire
    FOUND_VAR Pipewire_FOUND
    REQUIRED_VARS
    Pipewire_LIBRARY
    Pipewire_INCLUDE_DIR
    PIPEWIRE_COMPILE_TEST
    VERSION_VAR Pipewire_VERSION
)

# Aggregate libraries and include directories
if(Pipewire_FOUND)
    set(Pipewire_LIBRARIES
        ${Pipewire_LIBRARY}
    )
    set(Pipewire_INCLUDE_DIRS ${Pipewire_INCLUDE_DIR})
endif()

# Mark paths as advanced
mark_as_advanced(
    Pipewire_INCLUDE_DIR
    Pipewire_LIBRARY
)
