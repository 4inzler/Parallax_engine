# VR Game Development - Critical Requirements & Implementation Roadmap

**Engine:** Parallax Engine (formerly Parallax)
**Target:** VR Game Development
**Status:** VR Support Not Yet Implemented
**Priority:** 🔴 CRITICAL - Core VR features required before game development

---

## 🚨 Critical VR Requirements

### 1. **Stereo Rendering System** 🔴 CRITICAL
**Status:** ❌ NOT IMPLEMENTED
**Priority:** HIGHEST
**Est. Time:** 2-3 weeks

**Requirements:**
- Dual camera rendering (left/right eye)
- Separate view matrices per eye
- IPD (Interpupillary Distance) adjustment (58-72mm typical)
- Viewport offset for each eye
- Synchronized rendering pipeline
- Distortion correction for lens warping

**Implementation Needs:**
```cpp
// VR Camera Component
struct VRCameraComponent {
    float ipd = 0.064f;  // 64mm default
    glm::mat4 leftEyeView;
    glm::mat4 rightEyeView;
    glm::mat4 leftEyeProjection;
    glm::mat4 rightEyeProjection;
    RenderTarget* leftEyeTarget;
    RenderTarget* rightEyeTarget;
};
```

**Files to Create:**
- `engine/src/components/VRCamera.hpp`
- `engine/src/systems/VRRenderingSystem.hpp`
- `engine/src/vr/VRManager.hpp`

---

### 2. **VR SDK Integration** 🔴 CRITICAL
**Status:** ❌ NOT IMPLEMENTED
**Priority:** HIGHEST
**Est. Time:** 3-4 weeks

**Recommended SDK:** **OpenXR** (cross-platform standard)

**Why OpenXR:**
- ✅ Cross-platform (Quest, Vive, Index, WMR, PSVR2)
- ✅ Industry standard (Khronos Group)
- ✅ Future-proof
- ✅ Single codebase for all headsets
- ✅ Open source

**Alternative SDKs:**
- SteamVR (Valve - PC VR only)
- Oculus SDK (Meta - Quest only)
- OpenVR (deprecated, use OpenXR)

**Required Features:**
- HMD tracking (position + rotation)
- Controller tracking (6DOF per controller)
- Play space bounds
- Haptic feedback
- Input binding system
- Render texture submission
- Frame timing/prediction

**Implementation:**
```cpp
class VRManager {
public:
    bool initializeVR();  // Initialize OpenXR
    void shutdown();

    // Tracking
    glm::mat4 getHMDPose();
    glm::mat4 getControllerPose(VRHand hand);

    // Rendering
    void beginFrame();
    void submitFrame(VREye eye, Texture* texture);
    void endFrame();

    // Input
    bool getButtonState(VRController controller, VRButton button);
    glm::vec2 getJoystickAxis(VRController controller);
    void triggerHaptic(VRController controller, float intensity, float duration);
};
```

**Dependencies to Add:**
- OpenXR SDK (via vcpkg or manual)
- Platform-specific VR runtime

---

### 3. **VR Performance Optimization** 🔴 CRITICAL
**Status:** ⚠️ NEEDS ASSESSMENT
**Priority:** HIGHEST
**Target:** 90 FPS minimum (120 FPS preferred)

**Critical for VR:**
- VR requires 90+ FPS (vs 60 FPS for desktop)
- Each frame rendered TWICE (stereo)
- Low latency essential (< 20ms motion-to-photon)
- Dropped frames cause motion sickness

**Required Optimizations:**

#### a) **Rendering Performance:**
- ✅ Frustum culling per eye
- ✅ Occlusion culling
- ✅ Level-of-Detail (LOD) system
- ✅ Instanced rendering
- ✅ Forward+ or deferred rendering
- ✅ Single-pass stereo rendering (if GPU supports)
- ✅ Foveated rendering (if hardware supports)

#### b) **CPU Optimizations:**
- ✅ Multithreaded rendering
- ✅ Job system for game logic
- ✅ Async asset loading
- ✅ Object pooling
- ✅ Spatial partitioning (octree/BVH)

#### c) **GPU Optimizations:**
- ✅ Reduce draw calls (< 500 recommended)
- ✅ Batch similar materials
- ✅ Texture atlasing
- ✅ Shader complexity reduction
- ✅ Dynamic resolution scaling

**Performance Targets:**
| Metric | Minimum | Recommended | Ideal |
|--------|---------|-------------|-------|
| Frame Rate | 90 FPS | 120 FPS | 144 FPS |
| Frame Time | 11.1ms | 8.3ms | 6.9ms |
| Draw Calls | < 1000 | < 500 | < 300 |
| Triangles/Frame | < 1M | < 500K | < 300K |
| Texture Memory | < 4GB | < 2GB | < 1GB |

---

### 4. **VR Input System** 🔴 CRITICAL
**Status:** ❌ NOT IMPLEMENTED
**Priority:** HIGH
**Est. Time:** 2 weeks

**Current Input System:** Desktop (Keyboard/Mouse)
**VR Requires:** Motion controllers, hand tracking, gaze input

**Controller Features Needed:**
```cpp
enum class VRButton {
    Trigger,        // Index finger trigger
    Grip,           // Hand grip button
    Joystick,       // Thumbstick click
    ButtonA,        // Face button A
    ButtonB,        // Face button B
    Menu,           // Menu/system button
};

enum class VRHand {
    Left,
    Right
};

struct VRControllerState {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 velocity;
    glm::vec3 angularVelocity;

    bool triggerPressed;
    float triggerValue;  // 0.0 - 1.0
    bool gripPressed;
    float gripValue;
    glm::vec2 joystickAxis;

    bool buttonA;
    bool buttonB;
};
```

**Input Actions Needed:**
- Grab/Release objects
- Teleport locomotion
- Smooth locomotion (joystick)
- Menu interaction (laser pointer)
- Gesture recognition (optional)
- Hand tracking (optional, Quest 3+)

---

### 5. **VR Locomotion System** 🔴 CRITICAL
**Status:** ❌ NOT IMPLEMENTED
**Priority:** HIGH
**Est. Time:** 1-2 weeks

**VR Locomotion Modes:**

#### a) **Teleportation** (Comfort: High)
- Arc-based teleport preview
- Valid/invalid surface indication
- Fade transition (prevent motion sickness)
- Rotation during teleport

#### b) **Smooth Locomotion** (Comfort: Medium)
- Joystick-based movement
- Head-relative or controller-relative
- Optional vignette effect (reduces motion sickness)
- Adjustable movement speed

#### c) **Snap Turning** (Comfort: High)
- 30° or 45° rotation increments
- Fade during rotation
- Prevents smooth rotation sickness

#### d) **Room-Scale** (Comfort: Highest)
- Physical walking in play space
- Guardian/chaperone bounds
- Center recentering

**Implementation:**
```cpp
class VRLocomotionManager {
public:
    enum class Mode {
        Teleport,
        SmoothWalk,
        Dash,
        Climbing,
        Flying
    };

    void setMode(Mode mode);
    void update(float deltaTime);

    // Teleport
    bool getTeleportTarget(glm::vec3& outPosition);
    void executeTeleport(glm::vec3 position, float rotation);

    // Smooth locomotion
    void setSmoothSpeed(float speed);
    void setVignetteIntensity(float intensity);  // 0-1

    // Snap turn
    void snapTurn(float degrees);
};
```

---

### 6. **VR Comfort Features** 🟡 HIGH PRIORITY
**Status:** ❌ NOT IMPLEMENTED
**Priority:** HIGH
**Est. Time:** 1 week

**Essential for Preventing Motion Sickness:**

#### a) **Vignette Effect:**
```glsl
// Fragment shader vignette
float vignette = 1.0 - smoothstep(0.5, 1.0, length(uv - 0.5));
color.rgb *= mix(0.3, 1.0, vignette);
```

#### b) **Field of View Reduction:**
- Reduce FOV during fast movement
- Tunnel vision effect
- Helps prevent motion sickness

#### c) **Smooth Camera Transitions:**
- No instant rotation
- Fade to black for teleports
- Slow acceleration/deceleration

#### d) **Fixed Reference Frame:**
- Cockpit mode (stationary reference)
- Virtual nose (subtle grounding)
- Grid floor overlay

#### e) **Height Adjustment:**
```cpp
struct VRPlayerSettings {
    float playerHeight = 1.75f;  // Meters
    float eyeHeight = 1.65f;
    glm::vec3 floorOffset;
};
```

---

### 7. **VR UI System** 🟡 HIGH PRIORITY
**Status:** ⚠️ CURRENT ImGui NOT VR-COMPATIBLE
**Priority:** HIGH
**Est. Time:** 2-3 weeks

**Problem:** ImGui is screen-space, not suitable for VR

**VR UI Solutions:**

#### a) **World-Space UI:**
```cpp
class VRCanvas {
public:
    void setWorldPosition(glm::vec3 pos);
    void setWorldRotation(glm::quat rot);
    void setSize(float width, float height);

    // Render UI to texture
    void beginUI();
    void endUI();

    // Interaction
    bool raycastHit(glm::vec3 origin, glm::vec3 direction, glm::vec2& uvHit);
};
```

#### b) **Laser Pointer Interaction:**
```cpp
class VRLaserPointer {
public:
    void update(glm::vec3 controllerPos, glm::quat controllerRot);
    bool isHitting(VRCanvas* canvas, glm::vec2& uvCoord);
    void render();  // Draw laser beam
};
```

#### c) **Hand-Based Interaction:**
- Direct touch for close UI
- Poke interaction
- Grab and manipulate

#### d) **Diegetic UI:**
- In-world menus (e.g., wrist watch)
- Holographic displays
- Physical buttons/switches

**Implementation Approach:**
1. Render ImGui to texture
2. Display texture on 3D quad in world
3. Raycast from controller for interaction
4. Transform 2D clicks to 3D raycast hits

---

### 8. **VR-Specific Camera System** 🔴 CRITICAL
**Status:** ⚠️ CURRENT CAMERA NOT VR-READY
**Priority:** HIGHEST
**Est. Time:** 2 weeks

**Current Issue:** RMB-gated camera not suitable for VR

**VR Camera Requirements:**
- Head tracking controls camera (no mouse)
- No manual camera control in game mode
- Smooth interpolation (prevents judder)
- Prediction/extrapolation for low latency

**VR Camera Implementation:**
```cpp
class VRCamera {
public:
    void update(glm::mat4 hmdPose);

    // Per-eye rendering
    glm::mat4 getLeftEyeView() const;
    glm::mat4 getRightEyeView() const;
    glm::mat4 getLeftEyeProjection() const;
    glm::mat4 getRightEyeProjection() const;

    // Settings
    void setIPD(float ipd);
    void setNearClip(float near);
    void setFarClip(float far);

    // Prediction for low latency
    glm::mat4 getPredictedPose(float timeOffset);
};
```

**Editor vs Runtime:**
- **Editor Mode:** Keep RMB camera (debugging)
- **VR Game Mode:** Use HMD tracking
- **Toggle:** Switch between modes

---

### 9. **VR Audio** 🟡 MEDIUM PRIORITY
**Status:** ⚠️ NEEDS SPATIAL AUDIO
**Priority:** MEDIUM
**Est. Time:** 1-2 weeks

**Spatial Audio Requirements:**
- HRTF (Head-Related Transfer Function)
- 3D positional audio
- Reverb based on room size
- Occlusion/obstruction
- Doppler effect

**Recommended Libraries:**
- **Steam Audio** (free, excellent quality)
- **Resonance Audio** (Google, cross-platform)
- **OpenAL Soft** (open source)
- **FMOD** or **Wwise** (professional, paid)

---

### 10. **VR Physics & Interaction** 🟡 MEDIUM PRIORITY
**Status:** ⚠️ NEEDS VR PHYSICS
**Priority:** MEDIUM
**Est. Time:** 2-3 weeks

**VR Physics Needs:**
- Hand collision detection
- Grab mechanics (physics-based or kinematic)
- Throwable objects with velocity tracking
- Climbing mechanics
- Force feedback simulation

**Implementation:**
```cpp
class VRPhysicsHand {
public:
    void setControllerPose(glm::vec3 pos, glm::quat rot);
    void setVelocity(glm::vec3 velocity);

    // Grabbing
    void grab(PhysicsObject* object);
    void release();
    PhysicsObject* getGrabbedObject();

    // Throwing
    glm::vec3 getReleaseVelocity();  // For throwing
};
```

---

## 📋 Implementation Priority Order

### Phase 1: Core VR Rendering (4-6 weeks) 🔴
**Blockers for VR development - must complete first:**

1. ✅ **OpenXR Integration**
   - Initialize OpenXR session
   - Get HMD tracking
   - Get controller tracking
   - Frame timing

2. ✅ **Stereo Rendering Pipeline**
   - Dual render targets
   - Per-eye view matrices
   - Lens distortion correction
   - Frame submission to VR compositor

3. ✅ **VR Camera System**
   - Replace mouse/keyboard with HMD tracking
   - Per-eye projection matrices
   - IPD adjustment

4. ✅ **Basic VR Input**
   - Controller button reading
   - Trigger/grip values
   - Joystick axes

**Deliverable:** Ability to view scene in VR headset with head tracking

---

### Phase 2: VR Interaction (3-4 weeks) 🟡
**Required for playable VR game:**

5. ✅ **VR Locomotion**
   - Teleportation system
   - Smooth locomotion
   - Snap turning
   - Room-scale support

6. ✅ **VR UI System**
   - World-space canvases
   - Laser pointer interaction
   - Menu system

7. ✅ **VR Comfort Features**
   - Vignette effect
   - FOV reduction
   - Smooth transitions

**Deliverable:** Player can move and interact in VR

---

### Phase 3: VR Polish (2-3 weeks) 🟢
**Quality of life improvements:**

8. ✅ **VR Physics Interaction**
   - Grab/throw mechanics
   - Hand collision
   - Haptic feedback

9. ✅ **Spatial Audio**
   - 3D positional sound
   - HRTF implementation

10. ✅ **VR Performance Optimization**
    - Single-pass stereo
    - Foveated rendering (if supported)
    - Dynamic resolution scaling

**Deliverable:** Polished VR experience

---

### Phase 4: Advanced Features (4-6 weeks) 🔵
**Optional enhancements:**

11. ☐ **Hand Tracking** (Quest 3+)
12. ☐ **Passthrough Mixed Reality**
13. ☐ **Multiplayer VR**
14. ☐ **VR Replay System**
15. ☐ **Advanced Climbing Mechanics**

---

## 🛠️ Immediate Action Items

### Week 1: Setup & Planning
- [ ] Install OpenXR SDK
- [ ] Set up VR development environment
- [ ] Test headset connection
- [ ] Create VR feature branch in git
- [ ] Update CMakeLists.txt for OpenXR

### Week 2-3: OpenXR Integration
- [ ] Initialize OpenXR session
- [ ] Get HMD tracking data
- [ ] Get controller tracking data
- [ ] Render to VR compositor

### Week 4-5: Stereo Rendering
- [ ] Implement dual camera rendering
- [ ] Per-eye view/projection matrices
- [ ] Lens distortion correction
- [ ] Test in headset

### Week 6: VR Input
- [ ] Controller button mapping
- [ ] Trigger/grip analog values
- [ ] Haptic feedback

---

## 📦 Dependencies to Add

### vcpkg packages:
```bash
vcpkg install openxr-loader
vcpkg install openvr  # Optional, if supporting SteamVR
```

### Manual installs:
- **OpenXR SDK** (if not via vcpkg)
- **SteamVR** or **Oculus** runtime (for testing)

### Optional:
- **Steam Audio** (spatial audio)
- **XR Interaction Toolkit** (Unity-like interaction helpers)

---

## 🎮 Target VR Platforms

### Recommended Primary Targets:
1. **Meta Quest 3** (standalone + PC VR)
   - Most popular consumer headset
   - Standalone Android or PC tethered
   - Hand tracking, passthrough, high res

2. **Valve Index** (PC VR)
   - High-end PC VR
   - Finger tracking
   - High refresh rate (144Hz)

3. **SteamVR** (PC VR generic)
   - Works with Vive, Index, etc.
   - Through OpenXR

### Secondary Targets:
4. **PSVR2** (PlayStation VR)
5. **Windows Mixed Reality**
6. **HTC Vive**

**Recommendation:** Use **OpenXR** for cross-platform support

---

## ⚠️ Critical VR Gotchas

### 1. **Performance is Non-Negotiable**
- Desktop: 60 FPS okay → VR: 90 FPS minimum
- Dropped frames = instant motion sickness
- Budget: 11ms per frame (for 90 FPS)
- Rendering happens TWICE (stereo)

### 2. **Motion Sickness is Real**
- Bad frame rate → nausea
- Acceleration → nausea
- Smooth rotation → nausea
- Small FOV → nausea
- **Test with multiple people!**

### 3. **VR Scale Matters**
- 1 unit MUST equal 1 meter
- Incorrect scale = discomfort
- Player height must match real height

### 4. **Input is Different**
- No keyboard in VR
- Everything is spatial
- Menus must be in 3D space
- Text is hard to read

### 5. **Testing Requires VR Headset**
- Cannot develop VR without headset
- Simulator modes exist but insufficient
- Need real testing throughout development

---

## 📊 VR Performance Budgets

### Rendering Budget (11ms @ 90fps):
| System | Budget | Notes |
|--------|--------|-------|
| Game Logic | 2ms | Physics, AI, scripts |
| Rendering (CPU) | 3ms | Culling, draw calls |
| Rendering (GPU) | 5ms | Rasterization, shading |
| VR Compositor | 1ms | Distortion, submission |

### Memory Budget:
- **VRAM:** < 2GB texture memory
- **RAM:** < 4GB for game logic
- **Vertex Buffer:** < 100MB
- **Index Buffer:** < 50MB

---

## 🎯 Success Metrics

### Minimum Viable VR Product:
- ✅ 90 FPS stable
- ✅ < 20ms motion-to-photon latency
- ✅ Stereo rendering working
- ✅ Head tracking working
- ✅ Controller tracking working
- ✅ Basic locomotion (teleport OR smooth)
- ✅ Can interact with objects
- ✅ No motion sickness in 10min session

### Polished VR Product:
- ✅ 120 FPS stable (if hardware supports)
- ✅ Multiple locomotion modes
- ✅ Comfortable for 30+ min sessions
- ✅ Intuitive UI
- ✅ Spatial audio
- ✅ Hand presence (virtual hands)
- ✅ Haptic feedback

---

## 📚 Learning Resources

### OpenXR:
- [Khronos OpenXR Specification](https://www.khronos.org/openxr/)
- [OpenXR Tutorials](https://github.com/KhronosGroup/OpenXR-SDK)

### VR Best Practices:
- [Oculus VR Best Practices](https://developer.oculus.com/documentation/native/pc/dg-performance-guidelines/)
- [Valve VR Best Practices](https://steamcommunity.com/sharedfiles/filedetails/?id=1678978061)

### Performance:
- [VR Performance Optimization Guide](https://docs.unity3d.com/Manual/VRPerformance.html)

---

## 🚀 Getting Started (This Week)

1. **Test Current Engine in VR:**
   ```bash
   # Install SteamVR or Oculus software
   # Connect headset
   # Try running current engine (will NOT work, but good baseline)
   ```

2. **Install OpenXR SDK:**
   ```bash
   cd Parallax_engine
   vcpkg install openxr-loader
   ```

3. **Create VR Branch:**
   ```bash
   git checkout -b feature/vr-support
   ```

4. **Begin Implementation:**
   - Start with OpenXR integration
   - Get HMD tracking working
   - Render simple scene in headset

---

**Next Steps:** I'll implement the core VR rendering system. Should I start with:
1. OpenXR integration framework?
2. Stereo rendering pipeline?
3. VR-specific components?

Let me know and I'll begin implementation! 🎮🥽
