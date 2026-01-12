//// OpenXRManager.hpp /////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: OpenXR integration manager for VR support
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../components/VR.hpp"
#include "../renderer/Framebuffer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <string>
#include <vector>

// Forward declare OpenXR types to avoid including full SDK in header
typedef struct XrInstance_T* XrInstance;
typedef struct XrSession_T* XrSession;
typedef struct XrSpace_T* XrSpace;
typedef struct XrSwapchain_T* XrSwapchain;
typedef uint64_t XrSystemId;

namespace nexo::vr {

    /**
     * @brief OpenXR rendering API binding (OpenGL in our case)
     */
    enum class GraphicsAPI {
        OPENGL,
        VULKAN,
        DIRECTX11,
        DIRECTX12
    };

    /**
     * @brief OpenXR session state
     */
    enum class SessionState {
        UNKNOWN,
        IDLE,
        READY,
        SYNCHRONIZED,
        VISIBLE,
        FOCUSED,
        STOPPING,
        LOSS_PENDING,
        EXITING
    };

    /**
     * @brief Per-eye swapchain and rendering info
     */
    struct EyeRenderInfo {
        XrSwapchain swapchain = nullptr;
        std::vector<uint32_t> swapchainImages;  // OpenGL texture IDs
        uint32_t width = 0;
        uint32_t height = 0;

        // FOV for this eye
        float angleLeft = -0.785f;   // ~45 degrees
        float angleRight = 0.785f;
        float angleUp = 0.785f;
        float angleDown = -0.785f;

        // View matrix for this eye
        glm::mat4 viewMatrix = glm::mat4(1.0f);
        glm::mat4 projectionMatrix = glm::mat4(1.0f);
    };

    /**
     * @brief OpenXR Manager - Singleton managing VR runtime integration
     */
    class OpenXRManager {
    public:
        static OpenXRManager& getInstance();

        // Lifecycle
        bool initialize(GraphicsAPI api = GraphicsAPI::OPENGL);
        void shutdown();

        // Session management
        bool createSession();
        void destroySession();
        bool isSessionRunning() const { return m_sessionState == SessionState::FOCUSED; }
        SessionState getSessionState() const { return m_sessionState; }

        // Frame synchronization
        bool beginFrame();                    // Call at start of VR frame
        bool endFrame();                      // Call after rendering both eyes
        uint32_t getCurrentSwapchainImage(VREye eye) const;

        // Tracking
        bool updateTracking();                // Updates HMD and controller poses
        glm::vec3 getHMDPosition() const { return m_hmdPosition; }
        glm::quat getHMDRotation() const { return m_hmdRotation; }
        glm::vec3 getHMDVelocity() const { return m_hmdVelocity; }

        glm::vec3 getControllerPosition(VRHand hand) const;
        glm::quat getControllerRotation(VRHand hand) const;
        bool isControllerTracking(VRHand hand) const;

        // View matrices for stereo rendering
        glm::mat4 getEyeViewMatrix(VREye eye) const;
        glm::mat4 getEyeProjectionMatrix(VREye eye, float nearZ, float farZ) const;
        glm::vec3 getEyeOffset(VREye eye) const;

        // Display properties
        float getIPD() const { return m_ipd; }
        float getRefreshRate() const { return m_refreshRate; }
        uint32_t getRecommendedRenderWidth(VREye eye) const;
        uint32_t getRecommendedRenderHeight(VREye eye) const;

        // Input (button/trigger/thumbstick)
        void updateInput();
        bool isButtonPressed(VRHand hand, VRButton button) const;
        bool isButtonTouched(VRHand hand, VRButton button) const;
        float getTriggerValue(VRHand hand) const;
        float getGripValue(VRHand hand) const;
        glm::vec2 getThumbstick(VRHand hand) const;

        // Haptics
        void triggerHaptic(VRHand hand, float amplitude, float frequency, float duration);

        // Play area / Guardian
        bool getPlayAreaBounds(std::vector<glm::vec2>& outPoints) const;
        glm::vec3 getPlayAreaCenter() const { return m_playAreaCenter; }

        // Debug info
        std::string getRuntimeName() const { return m_runtimeName; }
        std::string getSystemName() const { return m_systemName; }
        bool isInitialized() const { return m_initialized; }

    private:
        OpenXRManager() = default;
        ~OpenXRManager();

        OpenXRManager(const OpenXRManager&) = delete;
        OpenXRManager& operator=(const OpenXRManager&) = delete;

        // OpenXR handles
        XrInstance m_instance = nullptr;
        XrSession m_session = nullptr;
        XrSystemId m_systemId = 0;
        XrSpace m_viewSpace = nullptr;         // VIEW reference space
        XrSpace m_localSpace = nullptr;        // LOCAL reference space
        XrSpace m_stageSpace = nullptr;        // STAGE reference space (room-scale)

        // Eye rendering
        EyeRenderInfo m_eyeRenderInfo[(int)VREye::COUNT];

        // Tracking state
        SessionState m_sessionState = SessionState::UNKNOWN;
        bool m_initialized = false;
        bool m_frameInProgress = false;

        // HMD pose
        glm::vec3 m_hmdPosition = glm::vec3(0.0f);
        glm::quat m_hmdRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 m_hmdVelocity = glm::vec3(0.0f);

        // Controller poses
        struct ControllerPose {
            glm::vec3 position = glm::vec3(0.0f);
            glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 velocity = glm::vec3(0.0f);
            bool isTracking = false;
        };
        ControllerPose m_controllers[(int)VRHand::COUNT];

        // Controller input state
        VRControllerState m_controllerState[(int)VRHand::COUNT];

        // Display properties
        float m_ipd = 0.064f;
        float m_refreshRate = 90.0f;

        // Play area
        glm::vec3 m_playAreaCenter = glm::vec3(0.0f);
        std::vector<glm::vec2> m_playAreaBounds;

        // Runtime info
        std::string m_runtimeName = "Unknown";
        std::string m_systemName = "Unknown";
        GraphicsAPI m_graphicsAPI = GraphicsAPI::OPENGL;

        // Internal helpers
        bool enumerateExtensions();
        bool createInstanceInternal();
        bool getSystem();
        bool enumerateViewConfigurations();
        bool createSpaces();
        bool createSwapchains();
        void pollEvents();
        void handleSessionStateChange(SessionState newState);

        // OpenXR action system (for input)
        bool createActions();
        void destroyActions();

        // Convert OpenXR types to GLM
        static glm::vec3 toGlmVec3(const void* xrVector3f);
        static glm::quat toGlmQuat(const void* xrQuaternionf);
        static glm::mat4 createProjectionFov(float angleLeft, float angleRight,
                                              float angleUp, float angleDown,
                                              float nearZ, float farZ);
    };

} // namespace nexo::vr
