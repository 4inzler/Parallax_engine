//// OpenXRManager.cpp /////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: OpenXR integration implementation
//
///////////////////////////////////////////////////////////////////////////////

#include "OpenXRManager.hpp"
#include "Logger.hpp"

// OpenXR headers
#define XR_USE_GRAPHICS_API_OPENGL
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <cstring>

namespace nexo::vr {

    //==========================================================================
    // Helper: Convert OpenXR types to GLM
    //==========================================================================

    glm::vec3 OpenXRManager::toGlmVec3(const void* xrVector3f)
    {
        const XrVector3f* v = static_cast<const XrVector3f*>(xrVector3f);
        return glm::vec3(v->x, v->y, v->z);
    }

    glm::quat OpenXRManager::toGlmQuat(const void* xrQuaternionf)
    {
        const XrQuaternionf* q = static_cast<const XrQuaternionf*>(xrQuaternionf);
        return glm::quat(q->w, q->x, q->y, q->z);
    }

    glm::mat4 OpenXRManager::createProjectionFov(float angleLeft, float angleRight,
                                                  float angleUp, float angleDown,
                                                  float nearZ, float farZ)
    {
        // OpenXR uses asymmetric FOV (each angle specified separately)
        // Standard perspective projection matrix for asymmetric frustum
        const float tanLeft = std::tan(angleLeft);
        const float tanRight = std::tan(angleRight);
        const float tanUp = std::tan(angleUp);
        const float tanDown = std::tan(angleDown);

        const float tanWidth = tanRight - tanLeft;
        const float tanHeight = tanUp - tanDown;

        glm::mat4 proj(0.0f);
        proj[0][0] = 2.0f / tanWidth;
        proj[1][1] = 2.0f / tanHeight;
        proj[2][0] = (tanRight + tanLeft) / tanWidth;
        proj[2][1] = (tanUp + tanDown) / tanHeight;
        proj[2][2] = -(farZ + nearZ) / (farZ - nearZ);
        proj[2][3] = -1.0f;
        proj[3][2] = -(2.0f * farZ * nearZ) / (farZ - nearZ);

        return proj;
    }

    //==========================================================================
    // Singleton
    //==========================================================================

    OpenXRManager& OpenXRManager::getInstance()
    {
        static OpenXRManager instance;
        return instance;
    }

    OpenXRManager::~OpenXRManager()
    {
        shutdown();
    }

    //==========================================================================
    // Initialization
    //==========================================================================

    bool OpenXRManager::initialize(GraphicsAPI api)
    {
        if (m_initialized)
        {
            LOG(NEXO_WARN, "[OpenXR] Already initialized");
            return true;
        }

        LOG(NEXO_INFO, "[OpenXR] Initializing OpenXR runtime...");
        m_graphicsAPI = api;

        if (!enumerateExtensions())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to enumerate extensions");
            return false;
        }

        if (!createInstanceInternal())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to create XrInstance");
            return false;
        }

        if (!getSystem())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to get XrSystem");
            return false;
        }

        if (!enumerateViewConfigurations())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to enumerate view configurations");
            return false;
        }

        m_initialized = true;
        LOG(NEXO_INFO, "[OpenXR] Initialized successfully. Runtime: {}, System: {}",
            m_runtimeName, m_systemName);

        return true;
    }

    void OpenXRManager::shutdown()
    {
        if (!m_initialized)
            return;

        LOG(NEXO_INFO, "[OpenXR] Shutting down...");

        destroySession();
        destroyActions();

        // Destroy spaces
        if (m_viewSpace)
            xrDestroySpace(m_viewSpace);
        if (m_localSpace)
            xrDestroySpace(m_localSpace);
        if (m_stageSpace)
            xrDestroySpace(m_stageSpace);

        // Destroy instance
        if (m_instance)
            xrDestroyInstance(m_instance);

        m_instance = nullptr;
        m_session = nullptr;
        m_viewSpace = nullptr;
        m_localSpace = nullptr;
        m_stageSpace = nullptr;
        m_initialized = false;

        LOG(NEXO_INFO, "[OpenXR] Shutdown complete");
    }

    bool OpenXRManager::enumerateExtensions()
    {
        uint32_t extensionCount = 0;
        xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr);

        std::vector<XrExtensionProperties> extensions(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
        xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensions.data());

        LOG(NEXO_INFO, "[OpenXR] Available extensions: {}", extensionCount);
        for (const auto& ext : extensions)
        {
            LOG(NEXO_TRACE, "[OpenXR]   - {}", ext.extensionName);
        }

        return true;
    }

    bool OpenXRManager::createInstanceInternal()
    {
        XrInstanceCreateInfo createInfo{XR_TYPE_INSTANCE_CREATE_INFO};
        std::strcpy(createInfo.applicationInfo.applicationName, "Parallax VR Game");
        createInfo.applicationInfo.applicationVersion = 1;
        std::strcpy(createInfo.applicationInfo.engineName, "Parallax Engine");
        createInfo.applicationInfo.engineVersion = 1;
        createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

        // Request OpenGL extension
        const char* extensions[] = {"XR_KHR_opengl_enable"};
        createInfo.enabledExtensionCount = 1;
        createInfo.enabledExtensionNames = extensions;

        XrResult result = xrCreateInstance(&createInfo, reinterpret_cast<XrInstance*>(&m_instance));
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrCreateInstance failed: {}", (int)result);
            return false;
        }

        // Get runtime name
        XrInstanceProperties instanceProps{XR_TYPE_INSTANCE_PROPERTIES};
        xrGetInstanceProperties(m_instance, &instanceProps);
        m_runtimeName = instanceProps.runtimeName;

        return true;
    }

    bool OpenXRManager::getSystem()
    {
        XrSystemGetInfo systemInfo{XR_TYPE_SYSTEM_GET_INFO};
        systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

        XrResult result = xrGetSystem(m_instance, &systemInfo, reinterpret_cast<XrSystemId*>(&m_systemId));
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrGetSystem failed: {}", (int)result);
            return false;
        }

        // Get system properties
        XrSystemProperties systemProps{XR_TYPE_SYSTEM_PROPERTIES};
        xrGetSystemProperties(m_instance, m_systemId, &systemProps);
        m_systemName = systemProps.systemName;

        LOG(NEXO_INFO, "[OpenXR] System: {}", m_systemName);
        LOG(NEXO_INFO, "[OpenXR]   Max layers: {}", systemProps.graphicsProperties.maxLayerCount);
        LOG(NEXO_INFO, "[OpenXR]   Max swapchain size: {}x{}",
            systemProps.graphicsProperties.maxSwapchainImageWidth,
            systemProps.graphicsProperties.maxSwapchainImageHeight);

        return true;
    }

    bool OpenXRManager::enumerateViewConfigurations()
    {
        uint32_t viewCount = 0;
        xrEnumerateViewConfigurationViews(m_instance, m_systemId,
                                          XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                                          0, &viewCount, nullptr);

        std::vector<XrViewConfigurationView> views(viewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
        xrEnumerateViewConfigurationViews(m_instance, m_systemId,
                                          XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
                                          viewCount, &viewCount, views.data());

        if (viewCount != 2)
        {
            LOG(NEXO_ERROR, "[OpenXR] Expected 2 views for stereo, got {}", viewCount);
            return false;
        }

        // Store recommended render target sizes per eye
        for (int i = 0; i < 2; i++)
        {
            m_eyeRenderInfo[i].width = views[i].recommendedImageRectWidth;
            m_eyeRenderInfo[i].height = views[i].recommendedImageRectHeight;

            LOG(NEXO_INFO, "[OpenXR] Eye {}: recommended {}x{}, max {}x{}",
                i, views[i].recommendedImageRectWidth, views[i].recommendedImageRectHeight,
                views[i].maxImageRectWidth, views[i].maxImageRectHeight);
        }

        return true;
    }

    //==========================================================================
    // Session Management
    //==========================================================================

    bool OpenXRManager::createSession()
    {
        if (m_session)
        {
            LOG(NEXO_WARN, "[OpenXR] Session already exists");
            return true;
        }

        LOG(NEXO_INFO, "[OpenXR] Creating session...");

        // OpenGL graphics binding (platform-specific)
        #ifdef _WIN32
        XrGraphicsBindingOpenGLWin32KHR graphicsBinding{XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR};
        graphicsBinding.hDC = wglGetCurrentDC();
        graphicsBinding.hGLRC = wglGetCurrentContext();
        #else
        XrGraphicsBindingOpenGLXlibKHR graphicsBinding{XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR};
        graphicsBinding.xDisplay = glXGetCurrentDisplay();
        graphicsBinding.glxContext = glXGetCurrentContext();
        graphicsBinding.glxDrawable = glXGetCurrentDrawable();
        #endif

        XrSessionCreateInfo sessionInfo{XR_TYPE_SESSION_CREATE_INFO};
        sessionInfo.next = &graphicsBinding;
        sessionInfo.systemId = m_systemId;

        XrResult result = xrCreateSession(m_instance, &sessionInfo, reinterpret_cast<XrSession*>(&m_session));
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrCreateSession failed: {}", (int)result);
            return false;
        }

        if (!createSpaces())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to create reference spaces");
            return false;
        }

        if (!createSwapchains())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to create swapchains");
            return false;
        }

        if (!createActions())
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to create action system");
            return false;
        }

        // Begin session
        XrSessionBeginInfo beginInfo{XR_TYPE_SESSION_BEGIN_INFO};
        beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;

        result = xrBeginSession(m_session, &beginInfo);
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrBeginSession failed: {}", (int)result);
            return false;
        }

        m_sessionState = SessionState::READY;
        LOG(NEXO_INFO, "[OpenXR] Session created and started");

        return true;
    }

    void OpenXRManager::destroySession()
    {
        if (!m_session)
            return;

        LOG(NEXO_INFO, "[OpenXR] Destroying session...");

        xrEndSession(m_session);
        xrDestroySession(m_session);
        m_session = nullptr;
        m_sessionState = SessionState::IDLE;
    }

    bool OpenXRManager::createSpaces()
    {
        XrReferenceSpaceCreateInfo spaceInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        spaceInfo.poseInReferenceSpace.orientation = {0, 0, 0, 1};
        spaceInfo.poseInReferenceSpace.position = {0, 0, 0};

        // VIEW space (relative to HMD)
        spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        if (XR_FAILED(xrCreateReferenceSpace(m_session, &spaceInfo, &m_viewSpace)))
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to create VIEW space");
            return false;
        }

        // LOCAL space (origin at session start)
        spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        if (XR_FAILED(xrCreateReferenceSpace(m_session, &spaceInfo, &m_localSpace)))
        {
            LOG(NEXO_ERROR, "[OpenXR] Failed to create LOCAL space");
            return false;
        }

        // STAGE space (room-scale origin at floor level)
        spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        XrResult result = xrCreateReferenceSpace(m_session, &spaceInfo, &m_stageSpace);
        if (XR_FAILED(result))
        {
            LOG(NEXO_WARN, "[OpenXR] STAGE space not supported, using LOCAL instead");
            m_stageSpace = m_localSpace;  // Fallback
        }

        LOG(NEXO_INFO, "[OpenXR] Reference spaces created");
        return true;
    }

    bool OpenXRManager::createSwapchains()
    {
        for (int eye = 0; eye < 2; eye++)
        {
            XrSwapchainCreateInfo swapchainInfo{XR_TYPE_SWAPCHAIN_CREATE_INFO};
            swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            swapchainInfo.format = GL_SRGB8_ALPHA8;  // sRGB for correct gamma
            swapchainInfo.sampleCount = 1;
            swapchainInfo.width = m_eyeRenderInfo[eye].width;
            swapchainInfo.height = m_eyeRenderInfo[eye].height;
            swapchainInfo.faceCount = 1;
            swapchainInfo.arraySize = 1;
            swapchainInfo.mipCount = 1;

            XrSwapchain swapchain;
            XrResult result = xrCreateSwapchain(m_session, &swapchainInfo, &swapchain);
            if (XR_FAILED(result))
            {
                LOG(NEXO_ERROR, "[OpenXR] Failed to create swapchain for eye {}", eye);
                return false;
            }

            m_eyeRenderInfo[eye].swapchain = swapchain;

            // Enumerate swapchain images (OpenGL textures)
            uint32_t imageCount = 0;
            xrEnumerateSwapchainImages(swapchain, 0, &imageCount, nullptr);

            std::vector<XrSwapchainImageOpenGLKHR> images(imageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR});
            xrEnumerateSwapchainImages(swapchain, imageCount, &imageCount,
                                       reinterpret_cast<XrSwapchainImageBaseHeader*>(images.data()));

            m_eyeRenderInfo[eye].swapchainImages.reserve(imageCount);
            for (const auto& img : images)
            {
                m_eyeRenderInfo[eye].swapchainImages.push_back(img.image);
            }

            LOG(NEXO_INFO, "[OpenXR] Swapchain for eye {}: {} images, {}x{}",
                eye, imageCount, swapchainInfo.width, swapchainInfo.height);
        }

        return true;
    }

    bool OpenXRManager::createActions()
    {
        // TODO: Implement action system for input
        // This would create XrActionSet, XrAction for buttons/triggers/thumbsticks
        // For now, just return true (input not yet implemented)
        LOG(NEXO_WARN, "[OpenXR] Action system not yet implemented");
        return true;
    }

    void OpenXRManager::destroyActions()
    {
        // TODO: Destroy action sets
    }

    //==========================================================================
    // Frame Rendering
    //==========================================================================

    bool OpenXRManager::beginFrame()
    {
        if (!m_session || m_frameInProgress)
            return false;

        XrFrameWaitInfo waitInfo{XR_TYPE_FRAME_WAIT_INFO};
        XrFrameState frameState{XR_TYPE_FRAME_STATE};

        XrResult result = xrWaitFrame(m_session, &waitInfo, &frameState);
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrWaitFrame failed: {}", (int)result);
            return false;
        }

        XrFrameBeginInfo beginInfo{XR_TYPE_FRAME_BEGIN_INFO};
        result = xrBeginFrame(m_session, &beginInfo);
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrBeginFrame failed: {}", (int)result);
            return false;
        }

        m_frameInProgress = true;
        return true;
    }

    bool OpenXRManager::endFrame()
    {
        if (!m_session || !m_frameInProgress)
            return false;

        // TODO: Submit layers to compositor
        XrFrameEndInfo endInfo{XR_TYPE_FRAME_END_INFO};
        endInfo.displayTime = 0;  // TODO: Use actual predicted display time
        endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
        endInfo.layerCount = 0;
        endInfo.layers = nullptr;

        XrResult result = xrEndFrame(m_session, &endInfo);
        if (XR_FAILED(result))
        {
            LOG(NEXO_ERROR, "[OpenXR] xrEndFrame failed: {}", (int)result);
            return false;
        }

        m_frameInProgress = false;
        return true;
    }

    uint32_t OpenXRManager::getCurrentSwapchainImage(VREye eye) const
    {
        // TODO: Implement swapchain image acquisition
        return 0;
    }

    //==========================================================================
    // Tracking
    //==========================================================================

    bool OpenXRManager::updateTracking()
    {
        if (!m_session)
            return false;

        // TODO: Implement xrLocateViews to get HMD pose and per-eye view matrices
        // For now, return placeholder data

        // This would call:
        // xrLocateViews(m_session, &locateInfo, &viewState, viewCount, &viewCountOutput, views);

        return true;
    }

    //==========================================================================
    // Getters
    //==========================================================================

    glm::vec3 OpenXRManager::getControllerPosition(VRHand hand) const
    {
        return m_controllers[(int)hand].position;
    }

    glm::quat OpenXRManager::getControllerRotation(VRHand hand) const
    {
        return m_controllers[(int)hand].rotation;
    }

    bool OpenXRManager::isControllerTracking(VRHand hand) const
    {
        return m_controllers[(int)hand].isTracking;
    }

    glm::mat4 OpenXRManager::getEyeViewMatrix(VREye eye) const
    {
        return m_eyeRenderInfo[(int)eye].viewMatrix;
    }

    glm::mat4 OpenXRManager::getEyeProjectionMatrix(VREye eye, float nearZ, float farZ) const
    {
        const auto& info = m_eyeRenderInfo[(int)eye];
        return createProjectionFov(info.angleLeft, info.angleRight,
                                   info.angleUp, info.angleDown,
                                   nearZ, farZ);
    }

    glm::vec3 OpenXRManager::getEyeOffset(VREye eye) const
    {
        // Left eye: negative X offset, Right eye: positive X offset
        float halfIPD = m_ipd * 0.5f;
        return (eye == VREye::LEFT) ? glm::vec3(-halfIPD, 0.0f, 0.0f)
                                     : glm::vec3(halfIPD, 0.0f, 0.0f);
    }

    uint32_t OpenXRManager::getRecommendedRenderWidth(VREye eye) const
    {
        return m_eyeRenderInfo[(int)eye].width;
    }

    uint32_t OpenXRManager::getRecommendedRenderHeight(VREye eye) const
    {
        return m_eyeRenderInfo[(int)eye].height;
    }

    //==========================================================================
    // Input
    //==========================================================================

    void OpenXRManager::updateInput()
    {
        // TODO: Implement action sync and input reading
        // This would call xrSyncActions() and xrGetActionStateFloat/Boolean()
    }

    bool OpenXRManager::isButtonPressed(VRHand hand, VRButton button) const
    {
        uint32_t mask = 1 << (uint32_t)button;
        return (m_controllerState[(int)hand].buttonsPressed & mask) != 0;
    }

    bool OpenXRManager::isButtonTouched(VRHand hand, VRButton button) const
    {
        uint32_t mask = 1 << (uint32_t)button;
        return (m_controllerState[(int)hand].buttonsTouched & mask) != 0;
    }

    float OpenXRManager::getTriggerValue(VRHand hand) const
    {
        return m_controllerState[(int)hand].trigger;
    }

    float OpenXRManager::getGripValue(VRHand hand) const
    {
        return m_controllerState[(int)hand].grip;
    }

    glm::vec2 OpenXRManager::getThumbstick(VRHand hand) const
    {
        return m_controllerState[(int)hand].thumbstick;
    }

    void OpenXRManager::triggerHaptic(VRHand hand, float amplitude, float frequency, float duration)
    {
        // TODO: Implement haptic feedback via xrApplyHapticFeedback()
        LOG(NEXO_TRACE, "[OpenXR] Haptic feedback: hand={}, amp={}, freq={}, dur={}",
            (int)hand, amplitude, frequency, duration);
    }

    //==========================================================================
    // Play Area
    //==========================================================================

    bool OpenXRManager::getPlayAreaBounds(std::vector<glm::vec2>& outPoints) const
    {
        outPoints = m_playAreaBounds;
        return !m_playAreaBounds.empty();
    }

} // namespace nexo::vr
