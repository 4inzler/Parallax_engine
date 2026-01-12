//// VR.hpp ////////////////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: VR headset and controller components for OpenXR integration
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

namespace nexo {

    /**
     * @brief Eye type for stereo rendering
     */
    enum class VREye : uint8_t {
        LEFT = 0,
        RIGHT = 1,
        COUNT = 2
    };

    /**
     * @brief VR hand type
     */
    enum class VRHand : uint8_t {
        LEFT = 0,
        RIGHT = 1,
        COUNT = 2
    };

    /**
     * @brief VR button identifiers matching OpenXR action bindings
     */
    enum class VRButton : uint32_t {
        TRIGGER = 0,
        GRIP,
        THUMBSTICK,
        BUTTON_A,      // Right controller
        BUTTON_B,      // Right controller
        BUTTON_X,      // Left controller
        BUTTON_Y,      // Left controller
        MENU,
        COUNT
    };

    /**
     * @brief VR controller state
     */
    struct VRControllerState {
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);

        // Button states (bit flags)
        uint32_t buttonsPressed = 0;
        uint32_t buttonsTouched = 0;

        // Analog inputs
        float trigger = 0.0f;           // 0.0 - 1.0
        float grip = 0.0f;              // 0.0 - 1.0
        glm::vec2 thumbstick = glm::vec2(0.0f);  // -1.0 to 1.0

        // Haptics
        float hapticAmplitude = 0.0f;   // 0.0 - 1.0
        float hapticFrequency = 0.0f;   // Hz
        float hapticDuration = 0.0f;    // seconds

        bool isTracking = false;
    };

    /**
     * @brief VR headset component - tracks HMD pose and properties
     */
    struct VRHeadsetComponent {
        // HMD pose (in world space)
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 angularVelocity = glm::vec3(0.0f);

        // Per-eye view offsets (from HMD center)
        glm::vec3 eyeOffset[(int)VREye::COUNT] = {
            glm::vec3(-0.032f, 0.0f, 0.0f),  // Left eye (half IPD)
            glm::vec3(0.032f, 0.0f, 0.0f)    // Right eye (half IPD)
        };

        // Per-eye field of view (radians)
        struct EyeFOV {
            float angleLeft;
            float angleRight;
            float angleUp;
            float angleDown;
        };
        EyeFOV eyeFOV[(int)VREye::COUNT];

        // Display properties
        float ipd = 0.064f;              // Inter-pupillary distance (meters)
        float refreshRate = 90.0f;       // Hz
        uint32_t recommendedWidth = 1832;  // Per-eye render target width
        uint32_t recommendedHeight = 1920; // Per-eye render target height

        // Tracking state
        bool isTracking = false;
        bool isSessionRunning = false;

        // Comfort settings
        bool enableVignette = true;
        float vignetteStrength = 0.5f;  // 0.0 - 1.0

        // Performance
        float foveationLevel = 1.0f;    // 0.0 = off, 1.0 = max foveated rendering
    };

    /**
     * @brief VR controller component - represents a motion controller
     */
    struct VRControllerComponent {
        VRHand hand = VRHand::RIGHT;
        VRControllerState state;

        // Visual representation
        std::string modelPath = "";  // Path to controller 3D model
        bool renderController = true;

        // Interaction
        bool enableRaycasting = true;
        float rayLength = 10.0f;     // meters
        glm::vec3 rayColor = glm::vec3(0.0f, 0.8f, 1.0f);
    };

    /**
     * @brief VR stereo camera component - extends standard camera for VR
     * Attach to entity with CameraComponent to enable stereo rendering
     */
    struct VRStereoCameraComponent {
        bool enabled = true;

        // Reference to HMD entity for tracking
        entt::entity hmdEntity = entt::null;

        // Stereo rendering mode
        enum class StereoMode : uint8_t {
            SIDE_BY_SIDE,        // Left/Right in single framebuffer
            MULTI_PASS,          // Separate render passes per eye
            INSTANCED            // Single-pass stereo (GPU instancing)
        };
        StereoMode renderMode = StereoMode::MULTI_PASS;

        // Per-eye framebuffers (for MULTI_PASS mode)
        std::shared_ptr<NxFramebuffer> eyeFramebuffers[(int)VREye::COUNT];

        // Near/far override for VR (usually closer near plane)
        float nearPlane = 0.05f;   // 5cm (closer than desktop 0.1m)
        float farPlane = 1000.0f;

        // Performance
        float renderScale = 1.0f;  // Render resolution multiplier (1.0 = recommended, 1.5 = supersampling)
        bool enableFoveatedRendering = false;
    };

    /**
     * @brief VR play area component - defines safe movement boundaries
     */
    struct VRPlayAreaComponent {
        enum class BoundaryType : uint8_t {
            SEATED,              // No boundary (seated experience)
            STANDING,            // Small area (arm's length)
            ROOM_SCALE           // Large tracked area
        };
        BoundaryType type = BoundaryType::STANDING;

        // Boundary polygon (in play space, Y=0 plane)
        std::vector<glm::vec2> boundaryPoints;

        // Center of play area (origin in world space)
        glm::vec3 centerPosition = glm::vec3(0.0f);

        // Visualization
        bool showBoundary = true;
        float boundaryFadeDistance = 0.3f;  // Start showing boundary when within 30cm
        glm::vec3 boundaryColor = glm::vec3(0.0f, 1.0f, 0.0f);
    };

    /**
     * @brief VR locomotion component - handles VR-specific movement
     */
    struct VRLocomotionComponent {
        enum class LocomotionMode : uint8_t {
            TELEPORT,            // Point and teleport
            SMOOTH_MOVEMENT,     // Joystick-based smooth movement
            HYBRID               // Both teleport and smooth
        };
        LocomotionMode mode = LocomotionMode::TELEPORT;

        // Teleport settings
        float teleportRange = 10.0f;             // meters
        float teleportArcHeight = 2.0f;          // meters
        bool requireValidSurface = true;
        glm::vec3 teleportIndicatorColor = glm::vec3(0.0f, 0.8f, 1.0f);

        // Smooth movement settings
        float moveSpeed = 3.0f;                  // m/s
        float sprintMultiplier = 2.0f;
        bool enablePhysics = true;               // Respect collisions
        bool enableGravity = true;

        // Rotation (snap turn vs smooth)
        enum class RotationMode : uint8_t {
            SNAP,                // 30/45 degree snap turns
            SMOOTH               // Continuous rotation
        };
        RotationMode rotationMode = RotationMode::SNAP;
        float snapTurnAngle = 30.0f;             // degrees
        float smoothTurnSpeed = 90.0f;           // degrees/second

        // Comfort options
        bool enableVignetteDuringMovement = true;
        float vignetteStrength = 0.6f;           // 0.0 - 1.0
        bool enableTunnelVision = false;         // Reduce FOV during movement
        float tunnelVisionFOVReduction = 0.3f;   // 0.0 - 1.0 (30% reduction)
    };

    /**
     * @brief VR interaction component - for grabbable/interactable objects
     */
    struct VRInteractableComponent {
        enum class InteractionType : uint8_t {
            GRAB,                // Can be picked up
            TOUCH,               // Trigger on touch
            USE,                 // Trigger button to use
            PHYSICAL             // Physics-based interaction
        };
        InteractionType type = InteractionType::GRAB;

        // Grab settings
        bool isGrabbed = false;
        entt::entity grabbingController = entt::null;
        glm::vec3 grabOffset = glm::vec3(0.0f);
        glm::quat grabRotationOffset = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        // Interaction constraints
        bool twoHandedGrab = false;
        float grabDistance = 0.5f;               // Max distance to grab (meters)

        // Haptic feedback on interaction
        float hapticAmplitude = 0.5f;            // 0.0 - 1.0
        float hapticDuration = 0.1f;             // seconds

        // Highlight when hovered
        bool highlightOnHover = true;
        glm::vec3 highlightColor = glm::vec3(1.0f, 1.0f, 0.0f);
    };

} // namespace nexo
