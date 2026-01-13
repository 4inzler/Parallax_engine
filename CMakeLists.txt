cmake_minimum_required(VERSION 3.28)

# PROJECT
project(client CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PARALLAX_COVERAGE OFF CACHE BOOL "Enable coverage for binaries")
set(PARALLAX_GIT_SUBMODULE OFF CACHE BOOL "Enable git submodules init and update")
set(PARALLAX_BOOTSTRAP_VCPKG OFF CACHE BOOL "Enable vcpkg bootstrap")
set(PARALLAX_BUILD_TESTS ON CACHE BOOL "Enable tests")
set(PARALLAX_BUILD_EXAMPLES OFF CACHE BOOL "Enable examples")
set(PARALLAX_GRAPHICS_API "OpenGL" CACHE STRING "Graphics API to use")

if(MINGW)
    message(WARNING "Disabling tests on MinGW: prebuilt vcpkg x64-windows gtest is incompatible with MinGW; build tests with MSVC or a MinGW triplet to run tests.")
    set(PARALLAX_BUILD_TESTS OFF CACHE BOOL "Enable tests" FORCE)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(PARALLAX_COMPILER_FLAGS_ALL --std=c++${CMAKE_CXX_STANDARD})
    set(PARALLAX_COMPILER_FLAGS_DEBUG -g -Wmissing-field-initializers -Wall -Wextra -Wpedantic)
    set(PARALLAX_COMPILER_FLAGS_RELEASE -O3 -DNDEBUG)
    set(PARALLAX_COVERAGE_FLAGS -O0 --coverage)

    set(PARALLAX_LINKER_FLAGS_ALL "")
    set(PARALLAX_LINKER_FLAGS_DEBUG "")
    set(PARALLAX_LINKER_FLAGS_RELEASE "-flto")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(PARALLAX_COMPILER_FLAGS_ALL /nologo /W4 /std:c++${CMAKE_CXX_STANDARD} /Zc:preprocessor /utf-8)
    set(PARALLAX_COMPILER_FLAGS_DEBUG /Zi /Od /Zc:preprocessor /MDd /D_DEBUG /D_ITERATOR_DEBUG_LEVEL=2 /D_SECURE_SCL=1)
    set(PARALLAX_COMPILER_FLAGS_RELEASE /O2 /Zc:preprocessor /DNDEBUG /MD)
    set(PARALLAX_COVERAGE_FLAGS "")  # MSVC doesn't support coverage in the same way

    set(PARALLAX_LINKER_FLAGS_ALL "")
    set(PARALLAX_LINKER_FLAGS_DEBUG "")
    set(PARALLAX_LINKER_FLAGS_RELEASE "/LTCG")
else()
    message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}, using default flags")
endif()

add_compile_options(
        "${PARALLAX_COMPILER_FLAGS_ALL}"
        "$<$<CONFIG:Debug>:${PARALLAX_COMPILER_FLAGS_DEBUG}>"
        "$<$<CONFIG:Release>:${PARALLAX_COMPILER_FLAGS_RELEASE}>"
)

add_link_options(
        "${PARALLAX_LINKER_FLAGS_ALL}"
        "$<$<CONFIG:Debug>:${PARALLAX_LINKER_FLAGS_DEBUG}>"
        "$<$<CONFIG:Release>:${PARALLAX_LINKER_FLAGS_RELEASE}>"
)

# Prevent Visual Studio (or other build tools) from creating per config sub-directories (e.g. Debug, Release)
# Useful to look for resource files relative to the executable path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}>)

if (PARALLAX_COVERAGE)
    message(STATUS "Coverage enabled, adding flags: ${PARALLAX_COVERAGE_FLAGS}")
    add_compile_options("$<$<CONFIG:Debug>:${PARALLAX_COVERAGE_FLAGS}>")
    add_link_options("$<$<CONFIG:Debug>:${PARALLAX_COVERAGE_FLAGS}>")
endif()

# SETUP GIT SUBMODULES
if (PARALLAX_GIT_SUBMODULE)
    find_package(Git QUIET)

    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule sync --recursive
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule sync --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()
endif()

# SETUP VCPKG
if(PARALLAX_BOOTSTRAP_VCPKG)
    message(STATUS "Bootstraping VCPKG")
    if (WIN32)
        execute_process(
                COMMAND .\\vcpkg\\bootstrap-vcpkg.bat
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    else()
        execute_process(
                COMMAND ./vcpkg/bootstrap-vcpkg.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
else()
    message(STATUS "Skipping VCPKG bootstrap")
endif()

# RUNNING VCPKG
message(STATUS "Running VCPKG...")
include("${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
message(STATUS "VCPKG done.")

# SETUP EDITOR
include("${CMAKE_CURRENT_SOURCE_DIR}/editor/CMakeLists.txt")
# SETUP ENGINE
include("${CMAKE_CURRENT_SOURCE_DIR}/engine/CMakeLists.txt")
# SETUP MANAGED CSHARP LIB
include("${CMAKE_CURRENT_SOURCE_DIR}/engine/src/scripting/managed/CMakeLists.txt")
add_dependencies(Parallax parallaxManaged)
# SETUP EXAMPLE
include("${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
# SETUP TESTS
if (PARALLAX_BUILD_TESTS)
    enable_testing()
    include("${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
endif()

include_directories("./common")

include("${CMAKE_CURRENT_SOURCE_DIR}/scripts/pack.cmake")
