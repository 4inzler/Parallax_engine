//// VRSystem.hpp //////////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: VR ECS systems for headset tracking, input, and rendering
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../components/VR.hpp"
#include "../components/Transform.hpp"
#include "../components/Camera.hpp"
#include "../vr/OpenXRManager.hpp"
#include "../ecs/System.hpp"
#include "Timestep.hpp"

namespace parallax {

    /**
     * @brief Updates VR headset tracking data from OpenXR runtime
     *
     * This system updates VRHeadsetComponent with latest pose data from the HMD.
     * Should run early in frame, before camera systems.
     */
    class VRHeadsetTrackingSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();
    };

    /**
     * @brief Updates VR controller tracking and input
     *
     * Updates VRControllerComponent with pose, button states, and analog input.
     * Should run early in frame alongside headset tracking.
     */
    class VRControllerTrackingSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();
    };

    /**
     * @brief Manages stereo camera rendering for VR
     *
     * Generates separate CameraContext entries for left/right eyes,
     * applying eye offsets and projection matrices from OpenXR.
     * Runs after VRHeadsetTrackingSystem, replaces standard CameraContextSystem
     * for entities with VRStereoCameraComponent.
     */
    class VRStereoCameraSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();

        // Helper: Create camera context for a single eye
        void createEyeCameraContext(
            entt::entity entity,
            const TransformComponent& transform,
            const CameraComponent& camera,
            const VRStereoCameraComponent& vrCamera,
            const VRHeadsetComponent& headset,
            VREye eye
        );
    };

    /**
     * @brief Handles VR locomotion (teleportation, smooth movement, snap turning)
     *
     * Processes VRLocomotionComponent to move the player entity based on
     * controller input and configured locomotion mode.
     */
    class VRLocomotionSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();

        // Locomotion modes
        void handleTeleportation(entt::entity entity, VRLocomotionComponent& locomotion,
                                  TransformComponent& transform, const Timestep ts);
        void handleSmoothMovement(entt::entity entity, VRLocomotionComponent& locomotion,
                                   TransformComponent& transform, const Timestep ts);
        void handleRotation(entt::entity entity, VRLocomotionComponent& locomotion,
                             TransformComponent& transform, const Timestep ts);

        // Teleport state
        struct TeleportState {
            bool isAiming = false;
            glm::vec3 targetPosition = glm::vec3(0.0f);
            bool hasValidTarget = false;
        };
        std::map<entt::entity, TeleportState> m_teleportStates;
    };

    /**
     * @brief Manages VR interactions (grabbing, touching, using objects)
     *
     * Handles VRInteractableComponent, detecting collisions between controller
     * raycasts and interactable objects, managing grab state.
     */
    class VRInteractionSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();

        // Raycasting
        struct RaycastHit {
            entt::entity entity = entt::null;
            glm::vec3 hitPoint = glm::vec3(0.0f);
            glm::vec3 hitNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            float distance = 0.0f;
        };

        bool raycastInteractables(const glm::vec3& origin, const glm::vec3& direction,
                                   float maxDistance, RaycastHit& outHit);

        void handleGrabbing(entt::entity controllerEntity, VRControllerComponent& controller,
                             const TransformComponent& controllerTransform);
        void handleUse(entt::entity controllerEntity, VRControllerComponent& controller);
    };

    /**
     * @brief Renders VR comfort features (vignette, boundary warnings)
     *
     * Renders post-processing effects for comfort:
     * - Vignette during movement (reduce motion sickness)
     * - Guardian/boundary visualization when approaching play area edge
     * - Tunnel vision FOV reduction (optional)
     */
    class VRComfortSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();

        // Vignette rendering
        void renderVignette(entt::entity entity, const VRLocomotionComponent& locomotion,
                             float strength);

        // Boundary warnings
        void checkPlayAreaProximity(entt::entity entity, const TransformComponent& transform,
                                     const VRPlayAreaComponent& playArea);
    };

    /**
     * @brief Synchronizes VR frame rendering with OpenXR compositor
     *
     * Calls OpenXR beginFrame/endFrame at appropriate times,
     * submits rendered eye textures to compositor.
     * Should run at the very end of frame, after all rendering.
     */
    class VRFrameSyncSystem : public System {
    public:
        void update(const Timestep ts) override;

    private:
        vr::OpenXRManager& m_openXR = vr::OpenXRManager::getInstance();
    };

} // namespace parallax
