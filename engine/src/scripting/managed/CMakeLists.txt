#### CMakeLists.txt ###########################################################
#
#  zzzzz       zzz  zzzzzzzzzzzzz    zzzz      zzzz       zzzzzz  zzzzz
#  zzzzzzz     zzz  zzzz                    zzzz       zzzz           zzzz
#  zzz   zzz   zzz  zzzzzzzzzzzzz         zzzz        zzzz             zzz
#  zzz    zzz  zzz  z                  zzzz  zzzz      zzzz           zzzz
#  zzz         zzz  zzzzzzzzzzzzz    zzzz       zzz      zzzzzzz  zzzzz
#
#  Author:      Guillaume HEIN
#  Date:        09/05/2025
#  Description: CMakeLists.txt for the PARALLAX managed library
#
###############################################################################

cmake_minimum_required(VERSION 3.20)

# Set project name
project(parallaxManaged LANGUAGES NONE)

# Define variables for configuration
set(PARALLAX_MANAGED_OUTPUT_DIR ${CMAKE_BINARY_DIR})
file(RELATIVE_PATH PARALLAX_MANAGED_OUTPUT_DIR_REL "${CMAKE_CURRENT_LIST_DIR}" "${PARALLAX_MANAGED_OUTPUT_DIR}")
set(PARALLAX_FRAMEWORK "net9.0")

# Locate the dotnet executable early and fall back to the default install path if PATH is missing it
if(EXISTS "C:/Program Files/dotnet/dotnet.exe")
    set(DOTNET_EXECUTABLE "C:/Program Files/dotnet/dotnet.exe")
else()
    find_program(DOTNET_EXECUTABLE dotnet
        HINTS ENV PATH
        PATHS
            "C:/Program Files/dotnet"
            "C:/Program Files (x86)/dotnet"
    )
endif()

get_filename_component(_dotnet_root "${DOTNET_EXECUTABLE}" DIRECTORY)
set(_dotnet_ref_root "${_dotnet_root}/packs/Microsoft.NETCore.App.Ref")
file(GLOB _net9_refs "${_dotnet_ref_root}/9.*")
if(NOT _net9_refs)
    file(GLOB _net8_refs "${_dotnet_ref_root}/8.*")
    if(_net8_refs)
        message(WARNING " .NET 9 reference pack not found at ${_dotnet_ref_root}; falling back to net8.0 for managed build.")
        set(PARALLAX_FRAMEWORK "net8.0")
    else()
        message(FATAL_ERROR "No suitable .NET reference packs found under ${_dotnet_ref_root}. Install the .NET SDK reference packs or adjust PARALLAX_FRAMEWORK.")
    endif()
endif()

set(SOURCES
    Lib.cs
    ObjectFactory.cs
    NativeInterop.cs
    Logger.cs
    Components/Light.cs
    Components/Transform.cs
    Components/ComponentBase.cs
    Components/PhysicsBodyComponent.cs
    Components/PhysicsEnums.cs
    Components/Ui/Field.cs
    Components/Ui/FieldArray.cs
    Components/Ui/FieldType.cs
    Systems/SystemBase.cs
    Systems/WorldState.cs
    Scripts/CubeSystem.cs
)

# Ensure dotnet is present before continuing
if(NOT DOTNET_EXECUTABLE OR NOT EXISTS "${DOTNET_EXECUTABLE}")
    message(FATAL_ERROR "The .NET SDK (dotnet CLI) is not installed or not found at ${DOTNET_EXECUTABLE}. Please install it from https://dotnet.microsoft.com/download.")
endif()

message(STATUS "Using dotnet executable: ${DOTNET_EXECUTABLE}")

# Generate Parallax.csproj from a template (Parallax.csproj.in)
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/Parallax.csproj.in  # Input template
    ${CMAKE_CURRENT_LIST_DIR}/Parallax.csproj     # Output file
    @ONLY                                     # Only replace variables in @VAR@ format
)

if(CMAKE_GENERATOR_PLATFORM)
    set(ARCH_ARG --arch ${CMAKE_GENERATOR_PLATFORM})
else()
    set(ARCH_ARG "")
endif()


# Build step
add_custom_target(parallaxManaged ALL
                  COMMAND ${DOTNET_EXECUTABLE} build Parallax.csproj
                  ${ARCH_ARG}
                  -c $<CONFIG>                        # Matches Debug/Release configuration
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}      # Working directory for the build
                  COMMENT "Building .NET managed project (Parallax.csproj)..."
)

# Clean step
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    ${PARALLAX_MANAGED_OUTPUT_DIR}/ParallaxManaged.dll
    ${PARALLAX_MANAGED_OUTPUT_DIR}/ParallaxManaged.pdb
    ${PARALLAX_MANAGED_OUTPUT_DIR}/ParallaxManaged.runtimeconfig.json
    ${PARALLAX_MANAGED_OUTPUT_DIR}/ParallaxManaged.deps.json
    ${CMAKE_CURRENT_LIST_DIR}/obj
    ${CMAKE_CURRENT_LIST_DIR}/bin
)
