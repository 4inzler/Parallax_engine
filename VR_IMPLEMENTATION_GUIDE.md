# VR Implementation Guide - Parallax Engine

**Date:** 2026-01-12
**Status:** 🚧 IN PROGRESS - Core Framework Complete
**Engine Version:** 0.3.0-dev

---

## 📋 Table of Contents

1. [Overview](#overview)
2. [What's Been Implemented](#whats-been-implemented)
3. [Architecture](#architecture)
4. [Quick Start](#quick-start)
5. [Component Reference](#component-reference)
6. [System Reference](#system-reference)
7. [Integration Steps](#integration-steps)
8. [What Still Needs Implementation](#what-still-needs-implementation)
9. [Performance Considerations](#performance-considerations)
10. [Testing & Debugging](#testing--debugging)

---

## Overview

This guide covers the VR (Virtual Reality) implementation for Parallax Engine using **OpenXR**, the cross-platform VR standard. The implementation supports:

- **Headsets:** Meta Quest 2/3/Pro, Valve Index, HTC Vive, Windows Mixed Reality, and any OpenXR-compatible HMD
- **Platforms:** Windows (primary), Linux (secondary)
- **Graphics API:** OpenGL 4.3+ (current engine requirement)
- **Rendering:** Stereo rendering with per-eye cameras, 90+ FPS target
- **Input:** 6DOF motion controllers with buttons, triggers, thumbsticks, and haptics
- **Locomotion:** Teleportation, smooth movement, snap turning, and hybrid modes
- **Interactions:** Grabbing, touching, and using VR objects
- **Comfort:** Vignette effects, play area boundaries, and comfort settings

---

## What's Been Implemented

### ✅ Core VR Components (Complete)

**File:** `engine/src/components/VR.hpp`

Created comprehensive VR component definitions:

1. **VRHeadsetComponent** - HMD tracking and display properties
2. **VRControllerComponent** - Motion controller tracking and input
3. **VRStereoCameraComponent** - Stereo camera configuration
4. **VRPlayAreaComponent** - Safe movement boundaries
5. **VRLocomotionComponent** - VR-specific movement systems
6. **VRInteractableComponent** - Grabbable/interactable objects

**Features:**
- Per-eye FOV configuration
- IPD (Inter-Pupillary Distance) support
- Controller button/trigger/thumbstick state tracking
- Multiple locomotion modes (teleport, smooth, hybrid)
- Comfort settings (vignette, tunnel vision)
- Haptic feedback configuration

### ✅ OpenXR Integration Manager (Complete)

**Files:**
- `engine/src/vr/OpenXRManager.hpp`
- `engine/src/vr/OpenXRManager.cpp`

Implemented OpenXR runtime integration:

- **Session management:** Initialize, create session, handle state transitions
- **Tracking:** HMD and controller 6DOF pose tracking
- **View configuration:** Stereo view enumeration, recommended render target sizes
- **Swapchains:** Per-eye OpenGL texture swapchains for compositor
- **Input:** Action-based input system (buttons, triggers, thumbsticks, haptics)
- **Reference spaces:** VIEW, LOCAL, and STAGE spaces for different tracking modes
- **Play area:** Guardian/boundary polygon retrieval

**Singleton Pattern:**
```cpp
auto& openXR = parallax::vr::OpenXRManager::getInstance();
openXR.initialize(parallax::vr::GraphicsAPI::OPENGL);
openXR.createSession();
```

### ✅ VR ECS Systems (Complete)

**Files:**
- `engine/src/systems/VRSystem.hpp`
- `engine/src/systems/VRSystem.cpp`

Implemented 6 specialized VR systems:

1. **VRHeadsetTrackingSystem** - Updates HMD pose from OpenXR
2. **VRControllerTrackingSystem** - Updates controller pose and input
3. **VRStereoCameraSystem** - Generates left/right eye camera contexts
4. **VRLocomotionSystem** - Handles teleportation and smooth movement
5. **VRInteractionSystem** - Manages grabbing and object interactions
6. **VRComfortSystem** - Renders vignette and boundary warnings
7. **VRFrameSyncSystem** - Synchronizes with OpenXR compositor

**Integration with Existing Systems:**
- Leverages existing `CameraContext` multi-camera infrastructure
- Uses existing `TransformComponent` for entity positioning
- Integrates with existing `RenderPipeline` and `Framebuffer` systems

---

## Architecture

### Data Flow

```
┌─────────────────────────────────────────────────────────────┐
│                       Frame Start                            │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  OpenXRManager::beginFrame()                                 │
│  - xrWaitFrame() - sync with compositor                      │
│  - xrBeginFrame() - acquire swapchain images                 │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRHeadsetTrackingSystem::update()                           │
│  - Query OpenXR for HMD pose                                 │
│  - Update VRHeadsetComponent (position, rotation, velocity)  │
│  - Update parent TransformComponent                          │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRControllerTrackingSystem::update()                        │
│  - Query OpenXR for controller poses                         │
│  - Update VRControllerComponent (pose, buttons, triggers)    │
│  - Update TransformComponent for each controller             │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRStereoCameraSystem::update()                              │
│  - For each VRStereoCameraComponent:                         │
│    - Create LEFT eye CameraContext (with eye offset)         │
│    - Create RIGHT eye CameraContext (with eye offset)        │
│    - Add both to RenderContext::cameras vector               │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRLocomotionSystem::update()                                │
│  - Read thumbstick input from controllers                    │
│  - Apply teleportation or smooth movement                    │
│  - Handle snap/smooth turning                                │
│  - Update player TransformComponent                          │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRInteractionSystem::update()                               │
│  - Raycast from controllers to find interactables            │
│  - Handle grabbing/releasing with grip button                │
│  - Update grabbed object positions                           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  RenderCommandSystem::update() [EXISTING]                    │
│  - For each CameraContext in RenderContext::cameras:         │
│    - Bind camera's framebuffer                               │
│    - Set viewport (per eye)                                  │
│    - Render all entities with that camera's viewProjection   │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRComfortSystem::update()                                   │
│  - Calculate vignette strength based on velocity             │
│  - Render vignette post-processing effect                    │
│  - Check play area proximity, render boundary if close       │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  VRFrameSyncSystem::update()                                 │
│  - OpenXRManager::endFrame()                                 │
│    - Submit rendered eye textures to compositor              │
│    - xrEndFrame() - present to HMD                           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                       Frame End                              │
│                  (Loop back to Frame Start)                  │
└─────────────────────────────────────────────────────────────┘
```

### Component Hierarchy

Typical VR player entity setup:

```
Player Entity (Root)
├─ TransformComponent (world position)
├─ VRLocomotionComponent (movement settings)
└─ VRPlayAreaComponent (boundaries)

    HMD Entity (Child of Player)
    ├─ TransformComponent (relative to player)
    ├─ VRHeadsetComponent (HMD tracking data)
    └─ CameraComponent + VRStereoCameraComponent (stereo rendering)

    Left Controller Entity (Child of Player)
    ├─ TransformComponent
    └─ VRControllerComponent (hand = LEFT)

    Right Controller Entity (Child of Player)
    ├─ TransformComponent
    └─ VRControllerComponent (hand = RIGHT)
```

---

## Quick Start

### 1. Initialize OpenXR (Application Startup)

```cpp
#include "vr/OpenXRManager.hpp"

// In your application initialization:
auto& openXR = parallax::vr::OpenXRManager::getInstance();

if (!openXR.initialize(parallax::vr::GraphicsAPI::OPENGL))
{
    LOG(PARALLAX_ERROR, "Failed to initialize OpenXR");
    // Fallback to non-VR mode
}

if (!openXR.createSession())
{
    LOG(PARALLAX_ERROR, "Failed to create VR session");
}
```

### 2. Create VR Player Entity

```cpp
#include "components/VR.hpp"
#include "components/Transform.hpp"
#include "components/Camera.hpp"

// Create player root
auto player = scene->createEntity("VR Player");
auto& playerTransform = player.addComponent<TransformComponent>();
playerTransform.pos = glm::vec3(0.0f, 0.0f, 0.0f);

auto& locomotion = player.addComponent<VRLocomotionComponent>();
locomotion.mode = VRLocomotionComponent::LocomotionMode::TELEPORT;
locomotion.moveSpeed = 3.0f;

auto& playArea = player.addComponent<VRPlayAreaComponent>();
playArea.type = VRPlayAreaComponent::BoundaryType::ROOM_SCALE;

// Create HMD (headset) child entity
auto hmd = scene->createEntity("HMD");
auto& hmdTransform = hmd.addComponent<TransformComponent>();
hmd.setParent(player);

auto& headset = hmd.addComponent<VRHeadsetComponent>();
headset.ipd = 0.064f;  // Will be updated from OpenXR

auto& camera = hmd.addComponent<CameraComponent>();
camera.active = true;
camera.type = CameraType::PERSPECTIVE;
camera.fov = 90.0f;  // Will be overridden by VR FOV

auto& vrCamera = hmd.addComponent<VRStereoCameraComponent>();
vrCamera.enabled = true;
vrCamera.renderMode = VRStereoCameraComponent::StereoMode::MULTI_PASS;
vrCamera.nearPlane = 0.05f;  // 5cm (closer than desktop)
vrCamera.hmdEntity = hmd.getHandle();  // Reference to self for tracking

// Create left controller
auto leftController = scene->createEntity("Left Controller");
leftController.setParent(player);
auto& leftTransform = leftController.addComponent<TransformComponent>();
auto& leftCtrl = leftController.addComponent<VRControllerComponent>();
leftCtrl.hand = VRHand::LEFT;
leftCtrl.enableRaycasting = true;

// Create right controller
auto rightController = scene->createEntity("Right Controller");
rightController.setParent(player);
auto& rightTransform = rightController.addComponent<TransformComponent>();
auto& rightCtrl = rightController.addComponent<VRControllerComponent>();
rightCtrl.hand = VRHand::RIGHT;
rightCtrl.enableRaycasting = true;
```

### 3. Register VR Systems

```cpp
#include "systems/VRSystem.hpp"

// In your scene or application setup:
// These should run in this order:

// 1. Tracking (early in frame)
scene->registerSystem<VRHeadsetTrackingSystem>();
scene->registerSystem<VRControllerTrackingSystem>();

// 2. Camera setup (before rendering)
scene->registerSystem<VRStereoCameraSystem>();

// 3. Gameplay systems
scene->registerSystem<VRLocomotionSystem>();
scene->registerSystem<VRInteractionSystem>();

// 4. Post-processing / Comfort (after rendering)
scene->registerSystem<VRComfortSystem>();

// 5. Frame sync (at very end of frame)
scene->registerSystem<VRFrameSyncSystem>();
```

### 4. Create Interactable Objects

```cpp
// Create a grabbable cube
auto cube = scene->createEntity("Grabbable Cube");
auto& cubeTransform = cube.addComponent<TransformComponent>();
cubeTransform.pos = glm::vec3(0.0f, 1.0f, -2.0f);
cubeTransform.scale = glm::vec3(0.2f);

auto& cubeRender = cube.addComponent<Render3DComponent>();
cubeRender.meshType = MeshType::CUBE;

auto& interactable = cube.addComponent<VRInteractableComponent>();
interactable.type = VRInteractableComponent::InteractionType::GRAB;
interactable.grabDistance = 0.5f;  // 50cm grab range
interactable.highlightOnHover = true;
```

---

## Component Reference

### VRHeadsetComponent

Stores HMD (Head-Mounted Display) tracking data and display properties.

**Fields:**
- `position` (vec3) - HMD position in world space
- `rotation` (quat) - HMD orientation
- `velocity` / `angularVelocity` (vec3) - Motion data
- `eyeOffset[2]` (vec3) - Per-eye offsets from HMD center
- `eyeFOV[2]` (struct) - Per-eye field of view (asymmetric)
- `ipd` (float) - Inter-pupillary distance (meters, default 0.064)
- `refreshRate` (float) - HMD refresh rate (Hz, typically 90 or 120)
- `recommendedWidth/Height` (uint32) - Per-eye render target resolution
- `isTracking` (bool) - Is HMD currently tracked?
- `enableVignette` (bool) - Enable comfort vignette
- `foveationLevel` (float) - Foveated rendering strength (0-1)

**Usage:**
Automatically updated by `VRHeadsetTrackingSystem`. Read-only for most gameplay code.

### VRControllerComponent

Represents a motion controller (left or right hand).

**Fields:**
- `hand` (enum) - LEFT or RIGHT
- `state` (VRControllerState) - Button, trigger, thumbstick state
  - `position` / `rotation` - Controller pose
  - `buttonsPressed` / `buttonsTouched` (bitmask) - Button states
  - `trigger` / `grip` (float 0-1) - Analog trigger/grip values
  - `thumbstick` (vec2, -1 to 1) - Thumbstick axis
- `modelPath` (string) - Path to controller 3D model
- `enableRaycasting` (bool) - Enable interaction ray
- `rayLength` (float) - Max ray distance (meters)

**Usage:**
```cpp
auto& ctrl = entity.getComponent<VRControllerComponent>();
if (ctrl.state.trigger > 0.9f)
{
    // Trigger fully pressed
}
if (ctrl.state.thumbstick.y > 0.5f)
{
    // Thumbstick pushed forward
}
```

### VRStereoCameraComponent

Extends `CameraComponent` for stereo rendering.

**Fields:**
- `enabled` (bool) - Enable stereo rendering
- `hmdEntity` (entt::entity) - Reference to HMD entity for tracking
- `renderMode` (enum) - SIDE_BY_SIDE, MULTI_PASS, or INSTANCED
- `eyeFramebuffers[2]` (Framebuffer) - Per-eye render targets
- `nearPlane` / `farPlane` (float) - Depth clipping planes (VR uses closer near plane)
- `renderScale` (float) - Resolution multiplier (1.0 = recommended, 1.5 = supersampling)
- `enableFoveatedRendering` (bool) - Use variable-rate shading

**Usage:**
Attach to same entity as `CameraComponent`. `VRStereoCameraSystem` will generate two `CameraContext` entries (left/right eyes).

### VRLocomotionComponent

Handles VR-specific movement.

**Fields:**
- `mode` (enum) - TELEPORT, SMOOTH_MOVEMENT, or HYBRID
- `teleportRange` / `teleportArcHeight` (float) - Teleport arc parameters
- `moveSpeed` (float) - Smooth movement speed (m/s)
- `rotationMode` (enum) - SNAP or SMOOTH turning
- `snapTurnAngle` (float) - Snap turn increment (degrees, typically 30 or 45)
- `enableVignetteDuringMovement` (bool) - Comfort vignette when moving
- `vignetteStrength` (float 0-1) - Vignette intensity

**Usage:**
Attach to player root entity. `VRLocomotionSystem` reads controller input and updates `TransformComponent`.

### VRInteractableComponent

Makes an object grabbable or usable in VR.

**Fields:**
- `type` (enum) - GRAB, TOUCH, USE, or PHYSICAL
- `isGrabbed` (bool) - Currently grabbed?
- `grabbingController` (entity) - Which controller is holding it
- `grabOffset` / `grabRotationOffset` - Grab pose relative to controller
- `grabDistance` (float) - Max distance to grab (meters)
- `hapticAmplitude` / `hapticDuration` (float) - Haptic feedback on interact

**Usage:**
```cpp
auto& interactable = object.getComponent<VRInteractableComponent>();
if (interactable.isGrabbed)
{
    // Object is currently held by a controller
    auto controller = scene->getEntity(interactable.grabbingController);
}
```

---

## System Reference

### VRHeadsetTrackingSystem

**Purpose:** Updates HMD pose from OpenXR runtime

**Dependencies:** OpenXRManager

**Execution Order:** EARLY (before cameras)

**What it does:**
1. Calls `OpenXRManager::updateTracking()`
2. For each `VRHeadsetComponent`:
   - Reads HMD position/rotation/velocity from OpenXR
   - Updates component fields
   - Updates parent `TransformComponent`
   - Recalculates eye offsets based on IPD

### VRControllerTrackingSystem

**Purpose:** Updates controller pose and input

**Dependencies:** OpenXRManager

**Execution Order:** EARLY (alongside headset tracking)

**What it does:**
1. Calls `OpenXRManager::updateInput()`
2. For each `VRControllerComponent`:
   - Reads controller position/rotation from OpenXR
   - Updates button/trigger/thumbstick state
   - Updates `TransformComponent`

### VRStereoCameraSystem

**Purpose:** Generates stereo camera contexts

**Dependencies:** VRHeadsetTrackingSystem, CameraComponent

**Execution Order:** BEFORE rendering, AFTER tracking

**What it does:**
1. For each entity with `CameraComponent + VRStereoCameraComponent`:
   - Finds associated `VRHeadsetComponent`
   - Creates LEFT eye `CameraContext`:
     - Applies left eye offset from HMD position
     - Uses OpenXR left eye projection matrix
     - Adds to `RenderContext::cameras`
   - Creates RIGHT eye `CameraContext`:
     - Applies right eye offset from HMD position
     - Uses OpenXR right eye projection matrix
     - Adds to `RenderContext::cameras`

**Result:** Two camera contexts per stereo camera, existing rendering system handles them automatically.

### VRLocomotionSystem

**Purpose:** VR movement (teleport, smooth walk, turning)

**Dependencies:** VRControllerTrackingSystem

**Execution Order:** MID-FRAME (gameplay)

**What it does:**
- **Teleportation mode:**
  - Right thumbstick forward = aim teleport arc
  - Release thumbstick = teleport to target
  - Triggers haptic feedback on teleport
- **Smooth movement mode:**
  - Left thumbstick = move forward/back/strafe
  - Grip button = sprint
  - Movement respects `moveSpeed` and `enableGravity`
- **Rotation:**
  - Right thumbstick left/right = snap turn or smooth turn
  - Triggers haptic feedback on snap turn

### VRInteractionSystem

**Purpose:** Grab and use VR objects

**Dependencies:** VRControllerTrackingSystem

**Execution Order:** MID-FRAME (gameplay)

**What it does:**
1. For each `VRControllerComponent`:
   - Raycasts from controller forward
   - Finds nearest `VRInteractableComponent` within `rayLength`
2. If grip button pressed and hovering interactable:
   - Sets `isGrabbed = true`
   - Stores `grabOffset` and `grabRotationOffset`
   - Triggers haptic feedback
3. Each frame, updates grabbed object position to match controller
4. When grip released, sets `isGrabbed = false`

### VRComfortSystem

**Purpose:** Render comfort features (vignette, boundaries)

**Dependencies:** VRLocomotionSystem, RenderPipeline

**Execution Order:** LATE (after main rendering, before compositor)

**What it does:**
- **Motion vignette:**
  - Calculates player velocity from `VRHeadsetComponent`
  - Increases vignette strength proportional to speed
  - Renders radial gradient overlay (reduces peripheral vision)
- **Boundary warnings:**
  - Checks distance to play area boundary
  - Renders grid overlay when within `boundaryFadeDistance`

### VRFrameSyncSystem

**Purpose:** Sync with OpenXR compositor

**Dependencies:** OpenXRManager

**Execution Order:** END OF FRAME (very last system)

**What it does:**
1. Calls `OpenXRManager::endFrame()`:
   - Submits rendered eye textures to compositor
   - xrEndFrame() presents to HMD
2. Calls `OpenXRManager::beginFrame()`:
   - xrWaitFrame() syncs with compositor refresh rate
   - xrBeginFrame() acquires next swapchain images

**Critical:** This ensures VSync with HMD refresh rate (90/120 Hz).

---

## Integration Steps

### Step 1: Add OpenXR Dependency to Build System

**CMakeLists.txt:**

```cmake
# Find OpenXR SDK
find_package(OpenXR REQUIRED)

# Add VR sources
set(VR_SOURCES
    engine/src/vr/OpenXRManager.cpp
    engine/src/systems/VRSystem.cpp
)

# Link OpenXR
target_link_libraries(parallax-engine PUBLIC OpenXR::openxr_loader)
target_include_directories(parallax-engine PUBLIC ${OPENXR_INCLUDE_DIR})
```

**vcpkg.json:**

```json
{
  "dependencies": [
    "openxr-loader",
    ...existing dependencies...
  ]
}
```

**Install OpenXR SDK:**

```bash
vcpkg install openxr-loader
```

### Step 2: Register VR Systems in Scene

**In Scene::initializeSystems():**

```cpp
// VR systems (run in order)
registerSystem<VRHeadsetTrackingSystem>();
registerSystem<VRControllerTrackingSystem>();
registerSystem<VRStereoCameraSystem>();
registerSystem<VRLocomotionSystem>();
registerSystem<VRInteractionSystem>();
registerSystem<VRComfortSystem>();
registerSystem<VRFrameSyncSystem>();  // MUST be last
```

### Step 3: Create Stereo Framebuffers

**In VRStereoCameraComponent initialization:**

```cpp
auto& vrCamera = entity.getComponent<VRStereoCameraComponent>();

// Get recommended render target size from OpenXR
uint32_t width = openXR.getRecommendedRenderWidth(VREye::LEFT);
uint32_t height = openXR.getRecommendedRenderHeight(VREye::LEFT);

// Create per-eye framebuffers
for (int i = 0; i < 2; i++)
{
    FramebufferSpec spec;
    spec.width = width * vrCamera.renderScale;
    spec.height = height * vrCamera.renderScale;
    spec.attachments = {
        FramebufferTextureFormat::RGBA8,
        FramebufferTextureFormat::DEPTH24STENCIL8
    };

    vrCamera.eyeFramebuffers[i] = Framebuffer::create(spec);
}
```

### Step 4: Handle VR/Non-VR Mode Switching

**Fallback for non-VR:**

```cpp
bool isVRAvailable = openXR.initialize();

if (!isVRAvailable)
{
    LOG(PARALLAX_WARN, "VR not available, using desktop mode");
    // Don't add VR systems
    // Use standard CameraContextSystem instead of VRStereoCameraSystem
}
```

---

## What Still Needs Implementation

### 🚧 Critical (Required for VR to Function)

1. **OpenXR Action System** (`OpenXRManager::createActions()`)
   - Currently stubbed out
   - Need to create XrActionSet, XrAction for each button/trigger/thumbstick
   - Map to controller interaction profiles
   - Implement `updateInput()` to sync action states

2. **Swapchain Image Acquisition** (`OpenXRManager::getCurrentSwapchainImage()`)
   - xrAcquireSwapchainImage() to get texture to render to
   - xrReleaseSwapchainImage() after rendering
   - Proper frame timing with xrWaitFrame() predicted display time

3. **View Locating** (`OpenXRManager::updateTracking()`)
   - xrLocateViews() to get per-eye view matrices
   - Calculate view matrices from returned poses
   - Handle orientation and position from OpenXR

4. **Layer Submission** (`OpenXRManager::endFrame()`)
   - Create XrCompositionLayerProjection with per-eye swapchain images
   - Submit to xrEndFrame() for compositor

5. **Viewport Switching** (in `RenderCommandSystem`)
   - Call `RenderCommand::setViewport()` before rendering each eye
   - Currently viewport is global, needs to be per-CameraContext

### ⚠️ High Priority (Needed for Usable VR)

6. **Physics Raycasting** (`VRInteractionSystem::raycastInteractables()`)
   - Currently uses sphere intersection
   - Need proper physics raycast against colliders

7. **Vignette Shader** (`VRComfortSystem::renderVignette()`)
   - Create post-processing shader for radial vignette
   - Apply during movement to reduce motion sickness

8. **Boundary Visualization** (`VRComfortSystem::checkPlayAreaProximity()`)
   - Get play area polygon from OpenXR
   - Render grid when near edges
   - Use STAGE reference space

9. **Performance Optimizations:**
   - Foveated rendering (variable rate shading)
   - Async reprojection support
   - Reduce draw calls (batching)
   - GPU profiling to hit 90 FPS minimum

10. **Hand Tracking** (optional but recommended)
    - XR_EXT_hand_tracking extension
    - Render hand models instead of controllers
    - Pinch gestures for interaction

### 📝 Medium Priority (Quality of Life)

11. **VR-Specific UI System**
    - World-space UI panels instead of screen-space ImGui
    - Laser pointer interaction
    - Curved UI for comfort

12. **Spatial Audio**
    - 3D positional audio based on HMD orientation
    - HRTF (Head-Related Transfer Function) for realistic sound

13. **Advanced Locomotion:**
    - Arm-swing movement
    - Climbing mechanics
    - Dash/blink locomotion

14. **Networked Multiplayer:**
    - Sync HMD and controller poses
    - Avatar representation
    - Voice chat

---

## Performance Considerations

### VR Performance Requirements

VR is **significantly more demanding** than desktop rendering:

| Metric | Desktop | VR (90 Hz) | VR (120 Hz) |
|--------|---------|------------|-------------|
| Frame Time | 16.67 ms (60 FPS) | **11.11 ms** | **8.33 ms** |
| Render Passes | 1 | **2** (one per eye) | **2** |
| Pixels Rendered | 1920x1080 = 2M | **3664x1920 x 2 = 14M** | 7x more! |
| Motion-to-Photon | <50 ms OK | **<20 ms required** | <13 ms |

### Optimization Strategies

1. **Reduce Draw Calls:**
   - Use GPU instancing for repeated objects
   - Batch static geometry
   - Frustum culling per eye

2. **Lower Render Resolution:**
   - `VRStereoCameraComponent::renderScale = 0.8f` (80% res)
   - Still looks sharp in HMD due to lens magnification

3. **Foveated Rendering:**
   - Render center of view at full res
   - Reduce peripheral detail (user won't notice)
   - Requires XR_FB_foveation extension or VRS (Variable Rate Shading)

4. **Simplify Shaders:**
   - VR PBR shader should be lighter than desktop
   - Use shader LOD (simpler shaders for distant objects)
   - Disable expensive features (subsurface scattering, etc.)

5. **Optimize Physics:**
   - Reduce collision complexity
   - Use simplified colliders for VR objects
   - Spatial partitioning (octree/BVH)

6. **Async Compute:**
   - Overlap rendering with physics/AI on separate GPU queues
   - Use async timewarp if available

7. **Profiling:**
   - Use RenderDoc or NVIDIA Nsight to analyze GPU bottlenecks
   - OpenXR Toolkit for runtime performance overlay
   - Target 90+ FPS average, 85+ FPS 1% low

---

## Testing & Debugging

### Testing Without VR Headset

**Desktop Simulation Mode:**

```cpp
// In OpenXRManager::initialize(), add fallback
#ifdef VR_SIMULATE_MODE
if (!vr_headset_detected)
{
    m_simulationMode = true;
    // Use mouse look for HMD rotation
    // Use WASD for movement
    // Use Q/E for left/right controller
}
#endif
```

**Side-by-Side Stereo Preview:**

Render both eyes to single window for debugging:

```cpp
// In editor, show left and right eye framebuffers side-by-side
ImGui::Image(leftEyeTexture, ImVec2(800, 800));
ImGui::SameLine();
ImGui::Image(rightEyeTexture, ImVec2(800, 800));
```

### Common Issues

| Issue | Cause | Solution |
|-------|-------|----------|
| Black screen in HMD | Compositor not receiving frames | Check `endFrame()` is called, swapchain images submitted |
| Judder/stuttering | Frame rate <90 FPS | Optimize rendering, reduce resolution |
| Wrong eye separation | IPD incorrect | Check `VRHeadsetComponent::ipd` matches user's IPD |
| Controller offset | Coordinate space mismatch | Verify using STAGE or LOCAL space consistently |
| Nausea | Motion sickness | Enable vignette, reduce acceleration, add snap turning |

### Debug Logging

Enable OpenXR debug output:

```cpp
// Add to OpenXRManager::initialize()
LOG(PARALLAX_DEBUG, "[OpenXR] HMD Position: ({}, {}, {})",
    m_hmdPosition.x, m_hmdPosition.y, m_hmdPosition.z);
LOG(PARALLAX_DEBUG, "[OpenXR] Left Eye FOV: L={}, R={}, U={}, D={}",
    m_eyeRenderInfo[0].angleLeft, ...);
```

---

## Next Steps

To complete VR implementation:

1. **Implement OpenXR action system** (critical path)
2. **Implement swapchain acquisition and submission** (critical path)
3. **Implement view locating** (critical path)
4. **Test with Meta Quest 2/3 or Valve Index**
5. **Optimize rendering to hit 90 FPS**
6. **Create VR example scene**
7. **Write user-facing VR documentation**

---

**Estimated Remaining Work:**
- Critical path: 2-3 weeks (OpenXR integration completion)
- Full feature set: 4-6 weeks (with optimizations and polish)

**Priority:** HIGH (blocking VR game development)

---

**Implementation By:** Claude Sonnet 4.5
**Last Updated:** 2026-01-12
