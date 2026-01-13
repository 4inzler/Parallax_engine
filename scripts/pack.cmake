#### pack.cmake ###############################################################
#
#  zzzzz       zzz  zzzzzzzzzzzzz    zzzz      zzzz       zzzzzz  zzzzz
#  zzzzzzz     zzz  zzzz                    zzzz       zzzz           zzzz
#  zzz   zzz   zzz  zzzzzzzzzzzzz         zzzz        zzzz             zzz
#  zzz    zzz  zzz  z                  zzzz  zzzz      zzzz           zzzz
#  zzz         zzz  zzzzzzzzzzzzz    zzzz       zzz      zzzzzzz  zzzzz
#
#  Author:      Guillaume HEIN
#  Date:        24/11/2024
#  Description: CMake to pack the Parallax project and create an installer.
#
###############################################################################

cmake_minimum_required(VERSION 3.28)

project(parallax-pack)

include("${CMAKE_CURRENT_SOURCE_DIR}/scripts/install.cmake")

# Set the version number
set(PARALLAX_VERSION_MAJOR 0)
set(PARALLAX_VERSION_MINOR 2)
set(PARALLAX_VERSION_PATCH 1)

# Set the project name
set(PROJECT_NAME "PARALLAX-Engine")

# CPack specific variables
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "PARALLAX Engine")
set(CPACK_PACKAGE_CONTACT "parallax.engine@gmail.com")
set(CPACK_STRIP_FILES YES)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "PARALLAX Engine - A C++ OpenGL game engine usable in virtual reality.")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
    "Features a scene editor, console logging, and asset importing."
)
set(CPACK_PACKAGE_VERSION "${PARALLAX_VERSION_MAJOR}.${PARALLAX_VERSION_MINOR}.${PARALLAX_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PARALLAX_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PARALLAX_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PARALLAX_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "ParallaxEngine")

# Set the icon path
set(PARALLAX_ICON_PATH "${CMAKE_SOURCE_DIR}/resources/parallax.ico")

# Configs for different package generators (but not the actual CPack configuration)
include("${CMAKE_SOURCE_DIR}/scripts/linux/deb_config.cmake")
include("${CMAKE_SOURCE_DIR}/scripts/windows/nsis_config.cmake")

set(CPACK_GENERATOR "DEB;NSIS")

# Replace @VARS@ with CMake variables in the template file
configure_file("${PROJECT_SOURCE_DIR}/scripts/CMakeCPackOptions.cmake.in"
               "${PROJECT_BINARY_DIR}/CMakeCPackOptions.cmake" @ONLY)

# Use the final generated file as config for CPack
set(CPACK_PROJECT_CONFIG_FILE "${PROJECT_BINARY_DIR}/CMakeCPackOptions.cmake")
# CMakeCPackOptions.cmake is processed in the include(CPack) command below

# This must always be after all CPACK\_\* variables are defined
include(CPack)
cpack_add_component(documentation
                    DISPLAY_NAME "Documentation"
                    DESCRIPTION "The documentation of the PARALLAX Engine. Will be installed in the docs directory."
                    GROUP optional
)
cpack_add_component(headers
                    DISPLAY_NAME "Headers"
                    DESCRIPTION "The headers of the PARALLAX Engine. Will be installed in the headers directory."
                    DISABLED
                    GROUP optional
)

cpack_add_component_group(optional
                          DISPLAY_NAME "Optional Components"
                          DESCRIPTION "Optional components of the PARALLAX Engine."
                          EXPANDED
)
