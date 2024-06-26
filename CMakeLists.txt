cmake_minimum_required(VERSION 3.28) # 3.28 C++20 Modules Support

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(DEBUG_BUILD TRUE)
endif()

# This thing is used by intellisense info provider (CMakeTools extension for VSCode)
# By default it is ON, but we want to ensure it is ON for intellesense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON
    CACHE BOOL "Enable/Disable output of compile commands during generation." FORCE)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the project name, later we can use this variable to refer to the project
set(this_project fire-starter)

# Set the project name and version
project(${this_project} VERSION 0.1.0 LANGUAGES CXX)

# Set the folder layout - project source, include and assets directories
# Don't use PROJECT_NAME, PROJECT_SOURCE_DIR, etc. as they are set by CMake
set(APP_SOURCE_DIR ${CMAKE_SOURCE_DIR}/app)
set(APP_INCLUDE_DIR ${APP_SOURCE_DIR}/include)
set(APP_INCLUDE_DIRS ${APP_INCLUDE_DIR})
set(APP_ASSETS_DIR ${APP_SOURCE_DIR}/assets)

# Setup the module path and used modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# The scripts below expect APP_INCLUDE_DIR is set
foreach(script cmake-tools;intellisense;targets;
    options;reuse;packages;submodules;assets;testing)
    include(${script})
endforeach()

# Set the output directories (could use `install` also)
set_preferable_output_dirs()

# Enable testing invoked before main add_subdirectory
# to allow the tests to be discovered
enable_testing()

# Include main project.
add_subdirectory("app")

# We need this to let VS Code intelisense work properly with
# C++20 modules. This, hopefully, is a temporary workaround
support_intellisense_and_unlock_all()

# Include tests.
add_subdirectory("tests")