# Parallax Engine

👋 Welcome to the Parallax Engine repository! This project is a collaborative effort to create a revolutionary game engine with **VR-first capabilities**. Our goal is to provide a robust, user-friendly, and efficient platform for game development in desktop as well as in virtual reality! 🤩✨🚀

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/4inzler/Parallax_engine)
[![VR Support](https://img.shields.io/badge/VR-OpenXR-blue.svg)](https://www.khronos.org/openxr/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## Table of Contents

- [Parallax Engine](#parallax-engine)
  - [Table of Contents](#table-of-contents)
  - [Project Overview](#project-overview)
  - [VR Support](#vr-support)
  - [Key Features](#key-features)
  - [External Dependencies](#external-dependencies)
  - [Build the Project](#build-the-project)
    - [Option 1: Using CMake Presets (Automatic)](#option-1-using-cmake-presets-automatic-recommended)
    - [Option 2: Using CMake Build (Manual)](#option-2-using-cmake-build-manual)
  - [Install Parallax Engine](#install-parallax-engine)
    - [Install via CMake](#install-via-cmake)
    - [Create an Installer for Windows (NSIS)](#create-an-installer-for-windows-nsis)
    - [Create an Installer for Linux (DEB)](#create-an-installer-for-linux-deb)
  - [VR Development Setup](#vr-development-setup)
  - [Run the Tests](#run-the-tests)
  - [Troubleshooting](#troubleshooting)
  - [Documentation](#documentation)
  - [Contributing](#contributing)
  - [Acknowledgments](#acknowledgments)
  - [License](#license)

## Project Overview

**Parallax Engine** is a modern C++ game engine with comprehensive **VR support** built on OpenXR. Developed by **Arosen Systems** (a subsidiary of **Parel LLC**) for the upcoming **Parel VR** game, the engine features a professional editor, PBR rendering, plugin architecture, and Entity-Component-System (ECS) design.

> [!NOTE]
> Screenshots and gameplay videos of **Parel VR** running on Parallax Engine will be added as development progresses.

> [!WARNING]
> This project is actively under development. VR support is in beta (core framework 70% complete). Some features may change.

## VR Support

Parallax Engine includes **native VR support** via OpenXR:

### Supported Headsets
- ✅ Meta Quest 2 / 3 / Pro
- ✅ Valve Index
- ✅ HTC Vive / Vive Pro
- ✅ Windows Mixed Reality
- ✅ Any OpenXR-compatible HMD

### VR Features
- **6DOF Tracking:** Full head and controller tracking
- **Stereo Rendering:** Per-eye rendering with optimized pipeline
- **Motion Controllers:** Buttons, triggers, thumbsticks, haptics
- **Locomotion:** Teleportation, smooth movement, snap/smooth turning
- **Interactions:** Grabbing, raycasting, physics-based interactions
- **Comfort:** Vignette effects, play area boundaries, motion sickness prevention

### VR Documentation
- 📖 [VR Implementation Guide](VR_IMPLEMENTATION_GUIDE.md) - Complete setup and API reference
- 🗺️ [VR Requirements & Roadmap](VR_REQUIREMENTS_AND_ROADMAP.md) - Development plan
- 📝 [VR Session Summary](VR_SESSION_SUMMARY.md) - Implementation status

## Key Features

### Rendering
- ✨ **PBR (Physically-Based Rendering)** - Cook-Torrance BRDF, metallic/roughness workflow
- 🎨 **Toon Shading** - Cel-shaded rendering for stylized games
- 🌟 **Normal Mapping** - With adjustable strength
- 📷 **Multi-Camera System** - Supports multiple viewports and stereo rendering
- 🖼️ **HDR & Gamma Correction** - Tonemapping for realistic lighting

### 🎨 Ray Tracing (Advanced)
- 🌟 **Ray-Traced Lighting** - Realistic light bouncing and global illumination
- 💎 **Ray-Traced Reflections** - Perfect mirrors, water, metallic surfaces
- 🌑 **Ray-Traced Shadows** - Soft shadows with realistic penumbra
- 🌍 **Global Illumination** - Indirect lighting from light bouncing
- 🎯 **Ambient Occlusion** - Contact shadows for depth perception
- ⚡ **VR-Optimized** - Hybrid rendering, foveated RT, adaptive quality
- 🔧 **Multiple APIs** - DXR (DirectX), Vulkan RT, NVIDIA OptiX, Software fallback

### 🔊 Audio Ray Tracing (Advanced)
- 🎵 **Realistic Sound Propagation** - Geometric acoustics simulation
- 🚪 **Occlusion & Obstruction** - Sound blocked/muffled by walls
- 🏛️ **Authentic Reverb** - Room-accurate acoustics (small room vs. cathedral)
- 📊 **Frequency-Dependent** - Highs attenuate more than lows through materials
- 🎧 **3D Binaural Audio** - HRTF for VR headphones, perfect spatial audio
- 🌊 **Material-Aware** - Wood, concrete, carpet, glass sound differently
- 🎮 **VR-Optimized** - Head tracking, low latency, Steam Audio integration

### Editor
- 🎛️ **ImGui-based UI** - Professional editor interface
- 📁 **Asset Management** - Import textures, models, materials
- 🔍 **Scene Inspector** - Hierarchical scene tree
- 🎬 **Material Inspector** - Real-time material editing
- 📋 **Enhanced Menus** - Save/Import/Export with keyboard shortcuts (Ctrl+S)
- 🪟 **Panel Management** - Hide/show panels without data loss

### Camera System
- 🖱️ **RMB-Gated Controls** - Right mouse button activates camera movement
- ⌨️ **WASD Movement** - Standard FPS-style navigation (only with RMB held)
- 🔼 **E/Q Vertical** - Move up/down (only with RMB held)
- 👆 **LMB Selection** - Left mouse button free for entity selection

### Plugin System
- 🧩 **Dynamic Loading** - Load plugins at runtime (.dll/.so)
- 📝 **Plugin API** - IPlugin interface for extensions
- 🎯 **Registration System** - Custom menus, importers, panels
- 📦 **Example Plugin** - Template for creating your own plugins
- 🖥️ **Cross-Platform** - Windows and Linux support

### Architecture
- 🏗️ **ECS (Entity-Component-System)** - Modern, data-oriented design
- ⚡ **Performance Optimized** - Efficient rendering and physics
- 🔌 **Modular** - Clean separation of concerns
- 📚 **Well Documented** - Comprehensive inline and external documentation

## External Dependencies

To run this project, ensure you have the following:
- **CMake** 3.28 or higher - [Install here](https://cmake.org/download/)
- **C++ Compiler** - MSVC (Windows), GCC/Clang (Linux/macOS)
- **Git** - For cloning with submodules - [Install here](https://github.com/git-guides/install-git)
- **.NET SDK 9.0** - For C# scripting support - [Install here](https://dotnet.microsoft.com/download/dotnet/9.0)
- **OpenXR SDK** - For VR support (installed via vcpkg)
- **vcpkg** - Package manager (included as submodule)

## Build the Project

### 1. Clone the Repository

```bash
git clone --recurse-submodules https://github.com/4inzler/Parallax_engine.git
cd Parallax_engine
```

### 2. Build with CMake

#### Option 1: Using CMake Presets (Automatic) [Recommended]

```bash
cmake --workflow --preset=build-debug
```

> [!NOTE]
> Available presets: `build-debug`, `build-release`, `build-coverage`, `minimal-build`, `test-debug`, `test-coverage`.

#### Option 2: Using CMake Build (Manual)

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=external/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

### 3. Launch the Engine

#### Windows:
```bash
.\build\Release\parallax-editor.exe
```

#### Linux/macOS:
```bash
./build/parallax-editor
```

## Install Parallax Engine

### Install via CMake

First build the project, then run:

```bash
cmake --install build --prefix /path/to/install
```

This installs Parallax Engine with all necessary files to `/path/to/install`.

### Create an Installer for Windows (NSIS)

1. Install [NSIS](https://nsis.sourceforge.io/Download)
2. Run:

```bash
cd build
cpack -G NSIS -C Release
```

Run the generated installer to install Parallax Engine.

### Create an Installer for Linux (DEB)

```bash
cd build
cpack -G DEB
```

Install the package:

```bash
sudo dpkg -i Parallax-Engine-*-Linux.deb
```

## VR Development Setup

### Prerequisites
- VR headset (Meta Quest 2/3, Valve Index, etc.)
- OpenXR runtime installed (SteamVR, Oculus App, WMR Portal)
- GPU: NVIDIA GTX 1060 / AMD RX 480 or better

### Build with VR Support

VR support is enabled by default. To build:

```bash
cmake -B build -DPARALLAX_ENABLE_VR=ON
cmake --build build --config Release
```

### Quick VR Setup

See [VR_IMPLEMENTATION_GUIDE.md](VR_IMPLEMENTATION_GUIDE.md) for:
- Complete VR setup tutorial
- Component and system reference
- Performance optimization guide
- Troubleshooting VR issues

**Current VR Status:** Core framework 70% complete. Action system and compositor integration in progress.

## Run the Tests

Build and run tests using gtest:

```bash
cmake -B build -DPARALLAX_BUILD_TESTS=ON
cmake --build build
cd build
ctest -C Debug
```

## Troubleshooting

### Common Issues

| Issue | Solution |
|-------|----------|
| CMake can't find OpenXR | Install via vcpkg: `vcpkg install openxr-loader` |
| VR headset not detected | Ensure OpenXR runtime is running (SteamVR, Oculus) |
| Build errors with plugins | Disable plugins: `-DPARALLAX_BUILD_PLUGINS=OFF` |
| ImGui headers not found | Update submodules: `git submodule update --init --recursive` |

For detailed troubleshooting, see our documentation.

## Documentation

### Core Documentation
- [CHANGELOG](CHANGELOG_PARALLAX_IMPROVEMENTS.md) - Feature changelog and migration guide
- [Implementation Summary](IMPLEMENTATION_SUMMARY.md) - Completed features and testing checklist
- [Test Validation Report](TEST_VALIDATION_REPORT.md) - Pre-build validation results

### VR Documentation
- [VR Implementation Guide](VR_IMPLEMENTATION_GUIDE.md) - Complete VR development guide
- [VR Requirements](VR_REQUIREMENTS_AND_ROADMAP.md) - VR technology overview and roadmap
- [VR Session Summary](VR_SESSION_SUMMARY.md) - Implementation status and next steps

### Ray Tracing Documentation
- [Ray Tracing Guide](RAYTRACING_GUIDE.md) - Complete guide to visual and audio ray tracing
  - Visual ray tracing (realistic lighting, reflections, shadows)
  - Audio ray tracing (spatial audio, occlusion, reverb)
  - VR-specific optimizations
  - Performance tuning for 90 FPS

### Code Documentation
- Inline documentation in all headers
- Component reference in VR_IMPLEMENTATION_GUIDE.md
- System reference in VR_IMPLEMENTATION_GUIDE.md

## Contributing

Contributions are welcome! This project is developed by:

**Current Development:**
- **Arosen Systems** - A subsidiary of Parel LLC
- **Parel Development Team** - VR development and engine enhancements
- **Project:** Parel VR - Next-generation VR gaming experience

Based on the original PARALLAX Engine by the EPITECH Strasbourg team.

### How to Contribute

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Acknowledgments

Parallax Engine is built upon the foundation of **[PARALLAX Engine](https://github.com/ParallaxEngine/game-engine)**, created by talented students from EPITECH Strasbourg.

### PARALLAX Engine Team

We extend our deepest gratitude to the original PARALLAX Engine team:
- **Guillaume HEIN** ([@Thyodas](https://github.com/Thyodas))
- **Jean CARDONNE** ([@jcardonne](https://github.com/jcardonne))
- **Marie GIACOMEL** ([@Sauterelle57](https://github.com/Sauterelle57))
- **Mehdy MORVAN** ([@iMeaNz](https://github.com/iMeaNz))
- **Thomas PARENTEAU** ([@ThomasParenteau](https://github.com/ThomasParenteau))

Their excellent work on the core ECS architecture, rendering pipeline, editor framework, and build system made Parallax Engine possible.

**What we inherited from PARALLAX:**
- Complete Entity-Component-System architecture
- Modern C++20 codebase with excellent code quality
- Robust OpenGL rendering pipeline with multi-camera support
- Professional ImGui-based editor with docking system
- Physics integration (Jolt Physics)
- Cross-platform build system (CMake + vcpkg)
- Comprehensive documentation and CI/CD

**What Parallax added:**
- VR support with OpenXR integration
- Stereo rendering and VR camera system
- Motion controller input and interactions
- RMB-gated camera controls
- PBR and toon shading systems
- Plugin architecture and API

For complete credits and third-party attributions, see **[CREDITS.md](CREDITS.md)**.

### Links to PARALLAX Engine

- **Repository:** https://github.com/ParallaxEngine/game-engine
- **Website:** https://www.parallax-engine.com/
- **Documentation:** https://parallaxengine.github.io/game-engine/

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

**Original Copyright Notice:**
```
Copyright (c) 2024-2025 PARALLAX Engine Team
```

**Derivative Work:**
```
Copyright (c) 2026 Parel LLC / Arosen Systems
Parallax Engine - Developed by Parel Development Team for Parel VR
Based on PARALLAX Engine
```

As required by the MIT License, the above copyright notices and permission notice must be included in all copies or substantial portions of the Software.

For third-party licenses, see `external/licenses/` directory or **[CREDITS.md](CREDITS.md)** for complete attribution.

### Contact

For license inquiries or questions, please open an issue on GitHub.

---

**Parallax Engine** - VR-Ready Game Development Platform

*Built with modern C++20, OpenXR, and a passion for immersive experiences.* 🚀🥽
