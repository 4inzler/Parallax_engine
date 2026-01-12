# Ray Tracing Implementation Guide - Parallax Engine

**Date:** 2026-01-12
**Developer:** Parel Development Team (Arosen Systems / Parel LLC)
**Project:** Parel VR
**Status:** 🎨 Framework Complete - Integration Pending

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [Visual Ray Tracing](#visual-ray-tracing)
3. [Audio Ray Tracing](#audio-ray-tracing)
4. [Quick Start](#quick-start)
5. [Component Reference](#component-reference)
6. [System Reference](#system-reference)
7. [Performance Optimization](#performance-optimization)
8. [VR-Specific Considerations](#vr-specific-considerations)
9. [Integration Steps](#integration-steps)
10. [Troubleshooting](#troubleshooting)

---

## Overview

Parallax Engine features **two advanced ray tracing systems** designed specifically for **Parel VR**:

### 🌟 Visual Ray Tracing
- **Realistic lighting** with global illumination and light bouncing
- **Ray-traced reflections** for mirrors, water, metallic surfaces
- **Ray-traced shadows** with soft penumbra
- **Ambient occlusion** for contact shadows
- **HDR lighting** with physically accurate attenuation

### 🔊 Audio Ray Tracing
- **Realistic sound propagation** with geometric acoustics
- **Occlusion and obstruction** - sound blocked by walls
- **Reverb and reflections** - authentic room acoustics
- **Frequency-dependent absorption** - highs muffled more than lows
- **3D binaural audio** for VR headphones (HRTF)

---

## Visual Ray Tracing

### What It Does

Ray tracing simulates realistic light behavior by tracing rays of light from the camera through the scene:

1. **Primary Rays** - From camera to scene (what you see)
2. **Shadow Rays** - From surface to light (is it in shadow?)
3. **Reflection Rays** - Bounce off shiny surfaces (mirrors, water)
4. **Refraction Rays** - Through transparent objects (glass, water)
5. **Global Illumination Rays** - Indirect lighting (light bouncing between surfaces)

### Key Features

**Realistic Lighting:**
- Light bounces off surfaces and brightens neighboring areas
- Colored light bleed (red wall makes nearby objects slightly red)
- Physically accurate light falloff (inverse square law)

**Beautiful Reflections:**
- Perfect mirror reflections
- Glossy reflections (slightly blurry based on roughness)
- Reflections in water, metal, glass

**Soft Shadows:**
- Physically accurate penumbra (shadow softness)
- Area light shadows
- Transparent object shadows (colored shadows through stained glass)

**Ambient Occlusion:**
- Contact shadows in corners and crevices
- More realistic depth perception

### Supported APIs

1. **DirectX Raytracing (DXR)** - Windows, DirectX 12, RTX GPUs
2. **Vulkan Ray Tracing** - Cross-platform, Vulkan 1.2+
3. **NVIDIA OptiX** - NVIDIA GPUs only, highest quality
4. **Software Ray Tracing** - CPU fallback (slow but works everywhere)

---

## Audio Ray Tracing

### What It Does

Audio ray tracing simulates realistic sound propagation by tracing sound rays through the scene:

1. **Direct Path** - Straight line from sound source to listener
2. **Reflections** - Sound bouncing off walls, floor, ceiling
3. **Diffraction** - Sound bending around corners
4. **Transmission** - Sound passing through walls (muffled)

### Key Features

**Realistic Occlusion:**
- Walls block sound
- Distance-based muffling
- Frequency-dependent (highs blocked more than lows)

**Authentic Reverb:**
- Room-accurate reverb (small room vs. cathedral)
- Early reflections (first bounces, critical for VR presence)
- Late reverb (diffuse tail)
- Material-specific reflections (concrete vs. carpet)

**3D Spatial Audio:**
- HRTF (Head-Related Transfer Function) for VR
- Accurate directional audio
- Distance attenuation
- Doppler effect (sound pitch changes with motion)

**Material-Aware:**
- Different materials absorb different frequencies
- Wood absorbs more than concrete
- Carpet absorbs more than tile
- Glass transmits some sound

### Supported Libraries

1. **Steam Audio** - Valve, free, high quality, VR-optimized
2. **Oculus Phonon** - Deprecated but excellent for VR
3. **FMOD Studio** - Commercial, professional features
4. **Wwise Spatial** - AAA game standard
5. **Resonance Audio** - Google, open source

---

## Quick Start

### Enable Visual Ray Tracing

```cpp
#include "components/RayTracing.hpp"
#include "systems/RayTracingSystem.hpp"

// Create ray tracing settings entity
auto rtSettings = scene->createEntity("RT Settings");
auto& settings = rtSettings.addComponent<RayTracingSettingsComponent>();

// Configure for VR (lower quality for 90 FPS)
settings.enabled = true;
settings.api = RayTracingAPI::VULKAN_RT;  // Or DXR for Windows
settings.quality = RayTracingQuality::MEDIUM;
settings.maxRaysPerPixel = 2;             // VR: keep low (1-2)
settings.maxBounces = 2;                  // Limit bounces for performance
settings.vrOptimizations = true;

// Hybrid mode: RT for specific effects only
settings.hybridMode = true;
settings.rtReflections = true;            // Enable reflections
settings.rtShadows = true;                // Enable shadows
settings.rtGlobalIllumination = false;    // Disable GI (expensive)
settings.rtAmbientOcclusion = true;       // Enable AO

// Denoising (critical for VR at low ray counts)
settings.enableDenoising = true;
settings.denoiserStrength = 0.8f;

// Register ray tracing systems
scene->registerSystem<RayTracingInitSystem>();
scene->registerSystem<AccelerationStructureBuildSystem>();
scene->registerSystem<RayTracingRenderSystem>();
scene->registerSystem<DenoisingSystem>();
scene->registerSystem<VRRayTracingSystem>();  // VR-specific optimizations
```

### Add Ray-Traced Materials

```cpp
// Create a reflective sphere
auto sphere = scene->createEntity("Mirror Sphere");
auto& transform = sphere.addComponent<TransformComponent>();
transform.pos = glm::vec3(0.0f, 1.0f, -3.0f);

auto& render = sphere.addComponent<Render3DComponent>();
render.meshType = MeshType::SPHERE;

auto& rtMaterial = sphere.addComponent<RayTracedMaterialComponent>();
rtMaterial.roughness = 0.0f;              // Perfect mirror
rtMaterial.metallic = 1.0f;               // Fully metallic
rtMaterial.enableReflections = true;
rtMaterial.reflectionStrength = 1.0f;
rtMaterial.maxReflectionBounces = 3;
```

### Enable Audio Ray Tracing

```cpp
#include "components/AudioRayTracing.hpp"
#include "systems/AudioRayTracingSystem.hpp"

// Create audio RT settings
auto audioRTSettings = scene->createEntity("Audio RT Settings");
auto& audioSettings = audioRTSettings.addComponent<AudioRayTracingSettingsComponent>();

audioSettings.enabled = true;
audioSettings.api = AudioRayTracingAPI::STEAM_AUDIO;
audioSettings.quality = AudioRayTracingSettingsComponent::Quality::MEDIUM;
audioSettings.maxRaysPerSource = 512;
audioSettings.maxBounces = 3;

// VR-specific
audioSettings.enableHRTF = true;          // 3D audio for headphones
audioSettings.useBinauralRendering = true;

// Register audio RT systems
scene->registerSystem<AudioRayTracingInitSystem>();
scene->registerSystem<AcousticGeometryBuildSystem>();
scene->registerSystem<AudioPropagationSystem>();
scene->registerSystem<AudioOcclusionSystem>();
scene->registerSystem<AudioReverbSystem>();
scene->registerSystem<SpatialAudioRenderSystem>();
scene->registerSystem<VRAudioSystem>();
```

### Add 3D Audio Source

```cpp
// Create audio source
auto audioSource = scene->createEntity("Fire Sound");
auto& sourceTransform = audioSource.addComponent<TransformComponent>();
sourceTransform.pos = glm::vec3(5.0f, 0.5f, -2.0f);

auto& source = audioSource.addComponent<AudioRayTracedSourceComponent>();
source.audioFilePath = "sounds/fire_loop.wav";
source.isLooping = true;
source.is3D = true;
source.enableRayTracing = true;
source.volume = 0.8f;
source.minDistance = 1.0f;
source.maxDistance = 50.0f;
```

### Add Acoustic Materials to Geometry

```cpp
// Make wall reflect sound realistically
auto wall = scene->createEntity("Concrete Wall");
auto& wallTransform = wall.addComponent<TransformComponent>();
auto& wallRender = wall.addComponent<Render3DComponent>();

auto& acousticGeom = wall.addComponent<AcousticGeometryComponent>();
acousticGeom.material = AcousticMaterialProperties::Concrete();
acousticGeom.affectsSound = true;
```

---

## Component Reference

### Visual Ray Tracing Components

#### RayTracingSettingsComponent
Global ray tracing configuration.

**Key Fields:**
- `enabled` (bool) - Enable/disable ray tracing
- `api` (RayTracingAPI) - DXR, Vulkan RT, OptiX, or Software
- `quality` (RayTracingQuality) - LOW, MEDIUM, HIGH, ULTRA, CUSTOM
- `maxRaysPerPixel` (uint32) - **VR: 1-2**, Desktop: 4-8
- `maxBounces` (uint32) - Light bounce depth (1-5)
- `enableDenoising` (bool) - Critical for VR
- `vrOptimizations` (bool) - Enable VR-specific optimizations
- `foveatedRayTracing` (bool) - Variable rate RT (requires eye tracking)

#### RayTracedMaterialComponent
Material properties for realistic rendering.

**Key Fields:**
- `roughness` (float) - 0 = mirror, 1 = diffuse
- `metallic` (float) - 0 = dielectric, 1 = metal
- `ior` (float) - Index of refraction (glass ~1.5)
- `transparency` (float) - 0 = opaque, 1 = transparent
- `emissiveColor` (vec3) - Self-illumination
- `enableReflections` (bool) - Ray-traced reflections
- `enableRefractions` (bool) - Ray-traced refractions (glass)
- `enableSSS` (bool) - Subsurface scattering (skin, wax)
- `clearcoat` (float) - Car paint, varnish effect

#### GlobalIlluminationProbeComponent
Indirect lighting probe for light bouncing.

**Key Fields:**
- `type` (ProbeType) - STATIC, DYNAMIC, SEMI_DYNAMIC
- `position` (vec3) - Probe center
- `extents` (vec3) - Probe volume size
- `probeGrid` (ivec3) - Resolution (8x8x8 = 512 probes)
- `raysPerProbe` (uint32) - Quality (256-1024)
- `bounces` (uint32) - Indirect light bounces

#### ReflectionProbeComponent
Environment capture for reflections.

**Key Fields:**
- `resolution` (Resolution) - LOW (256), MEDIUM (512), HIGH (1024), ULTRA (2048)
- `position` (vec3) - Probe center
- `radius` (float) - Influence radius
- `isDynamic` (bool) - Update every frame?
- `useParallaxCorrection` (bool) - More accurate reflections

### Audio Ray Tracing Components

#### AudioRayTracingSettingsComponent
Global audio raytracing configuration.

**Key Fields:**
- `enabled` (bool) - Enable/disable audio RT
- `api` (AudioRayTracingAPI) - Steam Audio, Phonon, FMOD, etc.
- `quality` (Quality) - LOW, MEDIUM, HIGH, ULTRA
- `maxRaysPerSource` (uint32) - 128-2048 (512 recommended)
- `maxBounces` (uint32) - Sound reflections (1-5)
- `enableOcclusion` (bool) - Sound blocked by walls
- `enableReverb` (bool) - Room acoustics
- `enableHRTF` (bool) - 3D audio for VR

#### AudioRayTracedSourceComponent
3D sound source with raytracing.

**Key Fields:**
- `audioFilePath` (string) - Path to audio file
- `isLooping` (bool) - Loop audio?
- `volume` (float) - 0.0-1.0
- `is3D` (bool) - 3D positional or 2D ambient?
- `minDistance` (float) - No attenuation within this distance
- `maxDistance` (float) - Max audible distance
- `directional` (bool) - Sound cone?
- `enableRayTracing` (bool) - Use acoustic simulation?
- `rayCount` (uint32) - Override global ray count

#### AcousticGeometryComponent
Makes objects affect sound propagation.

**Key Fields:**
- `affectsSound` (bool) - Enable acoustic interaction
- `material` (AcousticMaterialProperties) - Absorption/transmission per frequency
- `detail` (GeometryDetail) - Mesh detail level

**Material Presets:**
- `AcousticMaterialProperties::Wood()` - Moderate absorption
- `AcousticMaterialProperties::Concrete()` - Low absorption, hard surfaces
- `AcousticMaterialProperties::Carpet()` - High absorption
- `AcousticMaterialProperties::Glass()` - Low absorption, some transmission
- `AcousticMaterialProperties::Metal()` - Very low absorption

#### AcousticZoneComponent
Region with specific reverb characteristics.

**Key Fields:**
- `shape` (ZoneShape) - BOX, SPHERE, CONVEX_HULL
- `preset` (ReverbPreset) - SMALL_ROOM, HALL, CATHEDRAL, CAVE, etc.
- `reverbTime` (float) - RT60 in seconds
- `wetLevel` (float) - Reverb mix (0-1)

---

## System Reference

### Visual Ray Tracing Systems

**Execution Order:**
1. **RayTracingInitSystem** - Initialize API
2. **AccelerationStructureBuildSystem** - Build BVH/TLAS/BLAS
3. **GlobalIlluminationSystem** - Update GI probes
4. **ReflectionProbeSystem** - Update reflection probes
5. **RayTracingRenderSystem** - Main RT rendering
6. **DenoisingSystem** - Denoise ray-traced image
7. **VRRayTracingSystem** - VR-specific optimizations

### Audio Ray Tracing Systems

**Execution Order:**
1. **AudioRayTracingInitSystem** - Initialize Steam Audio/etc
2. **AcousticGeometryBuildSystem** - Build acoustic meshes
3. **AudioListenerSystem** - Update VR headset/camera
4. **AudioPropagationSystem** - Trace sound rays
5. **AudioOcclusionSystem** - Compute occlusion
6. **AudioReverbSystem** - Compute reverb
7. **SpatialAudioRenderSystem** - Apply HRTF and render
8. **AcousticEffectsSystem** - Apply filters (occlusion, reverb)
9. **VRAudioSystem** - VR binaural audio

---

## Performance Optimization

### Visual Ray Tracing for VR

**Challenge:** VR requires 90 FPS minimum (11.1ms per frame), but ray tracing is expensive.

**Solutions:**

1. **Hybrid Rendering** ✅
   - Use rasterization for base rendering
   - Use ray tracing only for specific effects (reflections, shadows)
   ```cpp
   settings.hybridMode = true;
   settings.rtReflections = true;   // RT reflections only
   settings.rtShadows = false;      // Raster shadows (faster)
   settings.rtGlobalIllumination = false;  // Too expensive for VR
   ```

2. **Lower Ray Counts** ✅
   - VR: 1-2 rays per pixel (with aggressive denoising)
   - Desktop: 4-8 rays per pixel
   ```cpp
   settings.maxRaysPerPixel = 2;
   settings.shadowRays = 1;
   ```

3. **Aggressive Denoising** ✅
   - Use AI denoisers (NVIDIA NRD, Intel OIDN)
   - Temporal denoising (use previous frames)
   ```cpp
   settings.enableDenoising = true;
   settings.denoiserStrength = 0.9f;  // Aggressive for VR
   ```

4. **Foveated Ray Tracing** ✅
   - More rays where user is looking
   - Fewer rays in periphery (requires eye tracking)
   ```cpp
   settings.foveatedRayTracing = true;
   ```

5. **Lower Resolution** ✅
   - Render RT at 0.7x or 0.8x resolution
   - Upscale with DLSS/FSR
   ```cpp
   settings.renderScale = 0.8f;  // 80% resolution
   ```

6. **Async Compute** ✅
   - Overlap RT with rasterization on GPU
   ```cpp
   settings.asyncCompute = true;
   ```

7. **Adaptive Quality** ✅
   - Reduce ray count dynamically when frame time exceeds budget
   ```cpp
   settings.adaptiveRayCounts = true;
   ```

### Audio Ray Tracing Optimization

**Target:** <2ms per frame for audio (leave budget for visual RT)

**Solutions:**

1. **Lower Ray Counts** ✅
   - 256-512 rays per source (not thousands)
   ```cpp
   audioSettings.maxRaysPerSource = 512;
   ```

2. **Lower Update Rate** ✅
   - Update audio propagation at 30-60 Hz (not every frame)
   ```cpp
   audioSettings.updateRate = 30.0f;  // Hz
   audioSettings.updateOnlyWhenMoving = true;
   ```

3. **Skip Static Sources** ✅
   - Don't recompute if source and listener haven't moved
   ```cpp
   audioSettings.updateOnlyWhenMoving = true;
   ```

4. **Simplified Geometry** ✅
   - Use LOW detail acoustic meshes
   ```cpp
   acousticGeom.detail = AcousticGeometryComponent::GeometryDetail::LOW;
   ```

5. **Limit Bounces** ✅
   - 2-3 bounces is usually sufficient
   ```cpp
   audioSettings.maxBounces = 2;
   ```

6. **Multithreading** ✅
   - Use multiple CPU threads for audio rays
   ```cpp
   audioSettings.useMultithreading = true;
   audioSettings.maxThreads = 4;
   ```

---

## VR-Specific Considerations

### Visual Ray Tracing in VR

**Challenges:**
- 90 FPS requirement (11.1ms per frame)
- Render twice (left eye + right eye)
- High resolution (1832x1920 per eye for Quest 2)

**Best Practices:**
- **Always use hybrid mode** - RT only for specific effects
- **Aggressive denoising** - Hide low ray count noise
- **Foveated rendering** - If eye tracking available
- **Shared acceleration structures** - Both eyes use same BVH
- **Async reprojection** - Previous frame while RT renders

### Audio Ray Tracing in VR

**Requirements:**
- **HRTF mandatory** - 3D audio is critical for VR presence
- **Binaural rendering** - Left/right ear separation
- **Head tracking** - Update audio as head rotates
- **Low latency** - Audio must sync with visuals (<20ms)

**Best Practices:**
- Use Steam Audio (designed for VR)
- Enable HRTF and binaural rendering
- Update at 60 Hz minimum
- Pre-bake reverb when possible

---

## Integration Steps

### 1. Add Dependencies

**Visual RT:**
- DXR: Windows SDK, DirectX 12
- Vulkan RT: Vulkan 1.2+ SDK, VK_KHR_ray_tracing
- OptiX: NVIDIA OptiX SDK 7.x

**Audio RT:**
- Steam Audio: vcpkg install steam-audio
- FMOD: Download FMOD Studio SDK
- Wwise: Download Audiokinetic Wwise SDK

### 2. Update CMakeLists.txt

```cmake
# Ray Tracing (Visual)
option(PARALLAX_ENABLE_RAY_TRACING "Enable ray tracing" ON)

if(PARALLAX_ENABLE_RAY_TRACING)
    find_package(Vulkan REQUIRED COMPONENTS RayTracing)

    target_sources(parallax-engine PRIVATE
        engine/src/systems/RayTracingSystem.cpp
    )

    target_link_libraries(parallax-engine PUBLIC Vulkan::Vulkan)
    target_compile_definitions(parallax-engine PUBLIC PARALLAX_RT_ENABLED)
endif()

# Audio Ray Tracing
option(PARALLAX_ENABLE_AUDIO_RT "Enable audio raytracing" ON)

if(PARALLAX_ENABLE_AUDIO_RT)
    find_package(SteamAudio REQUIRED)

    target_sources(parallax-engine PRIVATE
        engine/src/systems/AudioRayTracingSystem.cpp
    )

    target_link_libraries(parallax-engine PUBLIC SteamAudio::SteamAudio)
    target_compile_definitions(parallax-engine PUBLIC PARALLAX_AUDIO_RT_ENABLED)
endif()
```

### 3. Register Systems

```cpp
// In Scene initialization:
#ifdef PARALLAX_RT_ENABLED
scene->registerSystem<RayTracingInitSystem>();
scene->registerSystem<AccelerationStructureBuildSystem>();
scene->registerSystem<RayTracingRenderSystem>();
scene->registerSystem<DenoisingSystem>();
scene->registerSystem<VRRayTracingSystem>();
#endif

#ifdef PARALLAX_AUDIO_RT_ENABLED
scene->registerSystem<AudioRayTracingInitSystem>();
scene->registerSystem<AudioPropagationSystem>();
scene->registerSystem<SpatialAudioRenderSystem>();
scene->registerSystem<VRAudioSystem>();
#endif
```

---

## Troubleshooting

### Visual Ray Tracing Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Black screen | RT API not initialized | Check logs for RT init errors |
| Very low FPS | Too many rays | Reduce maxRaysPerPixel to 1-2 for VR |
| Noisy image | Not enough rays | Enable denoising, increase ray count |
| No reflections | Material not RT-enabled | Add RayTracedMaterialComponent |
| Crash on startup | Unsupported GPU | Use software fallback or check GPU drivers |

### Audio Ray Tracing Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| No 3D audio | HRTF disabled | Enable HRTF in settings |
| Sound not occluded | No acoustic geometry | Add AcousticGeometryComponent to walls |
| No reverb | Reverb disabled | Enable reverb in settings |
| Poor performance | Too many rays | Reduce maxRaysPerSource to 256-512 |
| Crackling audio | Buffer underrun | Increase audio buffer size |

---

## Next Steps

**To complete ray tracing implementation:**

1. **Implement RT backend** (1-2 weeks)
   - Vulkan RT or DXR integration
   - Shader binding tables
   - Ray generation shaders

2. **Implement denoising** (3-5 days)
   - Integrate NVIDIA NRD or Intel OIDN
   - Temporal accumulation

3. **Implement Steam Audio integration** (1 week)
   - Audio geometry export
   - Ray casting implementation
   - HRTF rendering

4. **Optimize for VR** (1 week)
   - Profile frame time
   - Implement foveated rendering
   - Tune quality settings

5. **Create Parel VR demo scene** (3-5 days)
   - Showcase ray-traced lighting
   - Demonstrate spatial audio
   - Optimize for Quest 2

**Estimated Total:** 4-6 weeks for full implementation

---

**Implementation By:** Parel Development Team (Arosen Systems / Parel LLC)
**Last Updated:** 2026-01-12
**Status:** Framework Complete, Integration Pending

🚀 **Ready to make Parel VR look and sound absolutely stunning!**
