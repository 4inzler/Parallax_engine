#### nsis_config.cmake ########################################################
#
#  zzzzz       zzz  zzzzzzzzzzzzz    zzzz      zzzz       zzzzzz  zzzzz
#  zzzzzzz     zzz  zzzz                    zzzz       zzzz           zzzz
#  zzz   zzz   zzz  zzzzzzzzzzzzz         zzzz        zzzz             zzz
#  zzz    zzz  zzz  z                  zzzz  zzzz      zzzz           zzzz
#  zzz         zzz  zzzzzzzzzzzzz    zzzz       zzz      zzzzzzz  zzzzz
#
#  Author:      Guillaume HEIN
#  Date:        25/11/2024
#  Description: CMake to configure the NSIS installer for the Parallax Engine.
#
###############################################################################

# Install Icon
set (CPACK_NSIS_MUI_ICON ${PARALLAX_ICON_PATH})

# Uninstall Icon
set (CPACK_NSIS_MUI_UNIICON ${PARALLAX_ICON_PATH})

# Start Menu Icon
set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/Parallax.exe")

# Icon displayed in installer
# See https://stackoverflow.com/questions/28768417/how-to-set-an-icon-in-nsis-install-cmake
# On linux to convert image to the right bmp format:
#  convert logo.png -background white -flatten BMP3:logo_parallax.bmp
set (CPACK_PACKAGE_ICON
     "${CMAKE_SOURCE_DIR}/resources\\\\parallax_header.bmp")

# Adds shortcuts to the Start Menu
# It will find exe in any subdirectory of the install directory
set(CPACK_PACKAGE_EXECUTABLES "Parallax" "PARALLAX Engine")

set(CPACK_NSIS_MENU_LINKS
    "https://parallaxengine.github.io/game-engine/" "PARALLAX Engine Website"
)
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON) # Alert the user if the program is already installed
set(CPACK_NSIS_MODIFY_PATH ON) # Let user decide where to install
set(CPACK_NSIS_MANIFEST_DPI_AWARE ON) # Make the installer DPI aware, less blurry
