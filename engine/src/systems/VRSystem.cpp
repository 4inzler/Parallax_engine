//// VRSystem.cpp //////////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: VR ECS systems implementation
//
///////////////////////////////////////////////////////////////////////////////

#include "VRSystem.hpp"
#include "../components/RenderContext.hpp"
#include "../scene/Scene.hpp"
#include "Logger.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace nexo {

    //==========================================================================
    // VRHeadsetTrackingSystem
    //==========================================================================

    void VRHeadsetTrackingSystem::update(const Timestep ts)
    {
        if (!m_openXR.isInitialized() || !m_openXR.isSessionRunning())
            return;

        // Update OpenXR tracking
        m_openXR.updateTracking();

        // Update all VRHeadsetComponent entities
        auto view = getActiveScene()->registry.view<VRHeadsetComponent, TransformComponent>();

        for (auto entity : view)
        {
            auto& headset = view.get<VRHeadsetComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            // Get latest HMD pose from OpenXR
            headset.position = m_openXR.getHMDPosition();
            headset.rotation = m_openXR.getHMDRotation();
            headset.velocity = m_openXR.getHMDVelocity();
            headset.isTracking = m_openXR.isSessionRunning();

            // Update eye offsets based on IPD
            float halfIPD = headset.ipd * 0.5f;
            headset.eyeOffset[(int)VREye::LEFT] = glm::vec3(-halfIPD, 0.0f, 0.0f);
            headset.eyeOffset[(int)VREye::RIGHT] = glm::vec3(halfIPD, 0.0f, 0.0f);

            // Update transform to match HMD (for child entities like cameras)
            transform.pos = headset.position;
            transform.quat = headset.rotation;
        }
    }

    //==========================================================================
    // VRControllerTrackingSystem
    //==========================================================================

    void VRControllerTrackingSystem::update(const Timestep ts)
    {
        if (!m_openXR.isInitialized() || !m_openXR.isSessionRunning())
            return;

        // Update input state
        m_openXR.updateInput();

        // Update all VRControllerComponent entities
        auto view = getActiveScene()->registry.view<VRControllerComponent, TransformComponent>();

        for (auto entity : view)
        {
            auto& controller = view.get<VRControllerComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            VRHand hand = controller.hand;

            // Update pose
            controller.state.position = m_openXR.getControllerPosition(hand);
            controller.state.rotation = m_openXR.getControllerRotation(hand);
            controller.state.isTracking = m_openXR.isControllerTracking(hand);

            // Update transform
            transform.pos = controller.state.position;
            transform.quat = controller.state.rotation;

            // Update input state
            controller.state.trigger = m_openXR.getTriggerValue(hand);
            controller.state.grip = m_openXR.getGripValue(hand);
            controller.state.thumbstick = m_openXR.getThumbstick(hand);

            // Update button states
            controller.state.buttonsPressed = 0;
            controller.state.buttonsTouched = 0;

            for (int i = 0; i < (int)VRButton::COUNT; i++)
            {
                VRButton button = (VRButton)i;
                if (m_openXR.isButtonPressed(hand, button))
                    controller.state.buttonsPressed |= (1 << i);
                if (m_openXR.isButtonTouched(hand, button))
                    controller.state.buttonsTouched |= (1 << i);
            }
        }
    }

    //==========================================================================
    // VRStereoCameraSystem
    //==========================================================================

    void VRStereoCameraSystem::update(const Timestep ts)
    {
        if (!m_openXR.isInitialized())
            return;

        auto view = getActiveScene()->registry.view<CameraComponent, VRStereoCameraComponent, TransformComponent>();

        for (auto entity : view)
        {
            auto& camera = view.get<CameraComponent>(entity);
            auto& vrCamera = view.get<VRStereoCameraComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            if (!vrCamera.enabled || !camera.active)
                continue;

            // Get HMD entity reference
            if (vrCamera.hmdEntity == entt::null)
            {
                // Try to find first VRHeadsetComponent in scene
                auto hmdView = getActiveScene()->registry.view<VRHeadsetComponent>();
                if (!hmdView.empty())
                    vrCamera.hmdEntity = hmdView.front();
                else
                    continue;  // No HMD found
            }

            auto* headset = getActiveScene()->registry.try_get<VRHeadsetComponent>(vrCamera.hmdEntity);
            if (!headset || !headset->isTracking)
                continue;

            // Generate separate camera contexts for left and right eyes
            createEyeCameraContext(entity, transform, camera, vrCamera, *headset, VREye::LEFT);
            createEyeCameraContext(entity, transform, camera, vrCamera, *headset, VREye::RIGHT);
        }
    }

    void VRStereoCameraSystem::createEyeCameraContext(
        entt::entity entity,
        const TransformComponent& transform,
        const CameraComponent& camera,
        const VRStereoCameraComponent& vrCamera,
        const VRHeadsetComponent& headset,
        VREye eye)
    {
        CameraContext context;

        // Get eye offset in world space
        glm::vec3 eyeOffset = headset.rotation * headset.eyeOffset[(int)eye];
        glm::vec3 eyePosition = headset.position + eyeOffset;

        // View matrix (camera looking forward with HMD rotation)
        glm::mat4 viewMatrix = glm::mat4_cast(glm::conjugate(headset.rotation));
        viewMatrix = glm::translate(viewMatrix, -eyePosition);

        // Projection matrix from OpenXR
        glm::mat4 projectionMatrix = m_openXR.getEyeProjectionMatrix(
            eye, vrCamera.nearPlane, vrCamera.farPlane
        );

        // Combined view-projection matrix
        context.viewProjectionMatrix = projectionMatrix * viewMatrix;
        context.cameraPosition = eyePosition;
        context.clearColor = camera.clearColor;

        // Use per-eye framebuffer if available
        if (vrCamera.eyeFramebuffers[(int)eye])
        {
            context.renderTarget = vrCamera.eyeFramebuffers[(int)eye];
        }
        else
        {
            context.renderTarget = camera.m_renderTarget;
        }

        context.pipeline = camera.m_pipeline;

        // Add to RenderContext
        RenderContext::get().cameras.push_back(context);
    }

    //==========================================================================
    // VRLocomotionSystem
    //==========================================================================

    void VRLocomotionSystem::update(const Timestep ts)
    {
        if (!m_openXR.isInitialized())
            return;

        auto view = getActiveScene()->registry.view<VRLocomotionComponent, TransformComponent>();

        for (auto entity : view)
        {
            auto& locomotion = view.get<VRLocomotionComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            // Handle movement based on mode
            if (locomotion.mode == VRLocomotionComponent::LocomotionMode::TELEPORT ||
                locomotion.mode == VRLocomotionComponent::LocomotionMode::HYBRID)
            {
                handleTeleportation(entity, locomotion, transform, ts);
            }

            if (locomotion.mode == VRLocomotionComponent::LocomotionMode::SMOOTH_MOVEMENT ||
                locomotion.mode == VRLocomotionComponent::LocomotionMode::HYBRID)
            {
                handleSmoothMovement(entity, locomotion, transform, ts);
            }

            // Handle rotation (snap turn or smooth turn)
            handleRotation(entity, locomotion, transform, ts);
        }
    }

    void VRLocomotionSystem::handleTeleportation(entt::entity entity, VRLocomotionComponent& locomotion,
                                                  TransformComponent& transform, const Timestep ts)
    {
        // Get right controller (typically used for teleportation)
        glm::vec2 thumbstick = m_openXR.getThumbstick(VRHand::RIGHT);
        bool triggerPressed = m_openXR.isButtonPressed(VRHand::RIGHT, VRButton::TRIGGER);

        auto& teleportState = m_teleportStates[entity];

        // Activate teleport aiming when thumbstick pushed forward
        if (thumbstick.y > 0.5f && !teleportState.isAiming)
        {
            teleportState.isAiming = true;
            LOG(NEXO_TRACE, "[VR] Teleport aiming started");
        }

        if (teleportState.isAiming)
        {
            // TODO: Raycast to find valid teleport target
            // For now, just use a position 5 meters forward
            glm::vec3 forward = transform.quat * glm::vec3(0.0f, 0.0f, -1.0f);
            teleportState.targetPosition = transform.pos + forward * 5.0f;
            teleportState.hasValidTarget = true;

            // Release to teleport
            if (thumbstick.y < 0.2f)
            {
                if (teleportState.hasValidTarget)
                {
                    transform.pos = teleportState.targetPosition;
                    LOG(NEXO_INFO, "[VR] Teleported to ({}, {}, {})",
                        transform.pos.x, transform.pos.y, transform.pos.z);

                    // Haptic feedback
                    m_openXR.triggerHaptic(VRHand::RIGHT, 0.8f, 100.0f, 0.1f);
                }

                teleportState.isAiming = false;
                teleportState.hasValidTarget = false;
            }
        }
    }

    void VRLocomotionSystem::handleSmoothMovement(entt::entity entity, VRLocomotionComponent& locomotion,
                                                   TransformComponent& transform, const Timestep ts)
    {
        // Get left controller thumbstick for movement
        glm::vec2 thumbstick = m_openXR.getThumbstick(VRHand::LEFT);

        if (glm::length(thumbstick) < 0.1f)
            return;  // Dead zone

        // Get forward and right directions (from HMD orientation, not controller)
        // TODO: Get actual HMD forward direction
        glm::vec3 forward = transform.quat * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 right = transform.quat * glm::vec3(1.0f, 0.0f, 0.0f);

        // Flatten to XZ plane if gravity is enabled
        if (locomotion.enableGravity)
        {
            forward.y = 0.0f;
            forward = glm::normalize(forward);
            right.y = 0.0f;
            right = glm::normalize(right);
        }

        // Calculate movement vector
        glm::vec3 movement = forward * thumbstick.y + right * thumbstick.x;
        movement *= locomotion.moveSpeed * ts.getDelta();

        // Check for sprint (grip button)
        if (m_openXR.getGripValue(VRHand::LEFT) > 0.8f)
            movement *= locomotion.sprintMultiplier;

        // Apply movement
        transform.pos += movement;
    }

    void VRLocomotionSystem::handleRotation(entt::entity entity, VRLocomotionComponent& locomotion,
                                             TransformComponent& transform, const Timestep ts)
    {
        glm::vec2 thumbstick = m_openXR.getThumbstick(VRHand::RIGHT);

        if (locomotion.rotationMode == VRLocomotionComponent::RotationMode::SNAP)
        {
            // Snap turn on thumbstick left/right
            static bool snapCooldown = false;

            if (std::abs(thumbstick.x) > 0.7f && !snapCooldown)
            {
                float angle = (thumbstick.x > 0) ? -locomotion.snapTurnAngle : locomotion.snapTurnAngle;
                glm::quat rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
                transform.quat = rotation * transform.quat;

                snapCooldown = true;

                // Haptic feedback
                m_openXR.triggerHaptic(VRHand::RIGHT, 0.3f, 80.0f, 0.05f);

                LOG(NEXO_TRACE, "[VR] Snap turn: {} degrees", angle);
            }

            if (std::abs(thumbstick.x) < 0.3f)
                snapCooldown = false;
        }
        else  // SMOOTH rotation
        {
            if (std::abs(thumbstick.x) > 0.1f)
            {
                float rotationSpeed = locomotion.smoothTurnSpeed * thumbstick.x * ts.getDelta();
                glm::quat rotation = glm::angleAxis(glm::radians(-rotationSpeed), glm::vec3(0.0f, 1.0f, 0.0f));
                transform.quat = rotation * transform.quat;
            }
        }
    }

    //==========================================================================
    // VRInteractionSystem
    //==========================================================================

    void VRInteractionSystem::update(const Timestep ts)
    {
        if (!m_openXR.isInitialized())
            return;

        // Update each controller's interactions
        auto controllerView = getActiveScene()->registry.view<VRControllerComponent, TransformComponent>();

        for (auto entity : controllerView)
        {
            auto& controller = controllerView.get<VRControllerComponent>(entity);
            auto& transform = controllerView.get<TransformComponent>(entity);

            if (!controller.state.isTracking)
                continue;

            // Handle grabbing/releasing
            handleGrabbing(entity, controller, transform);

            // Handle use button
            handleUse(entity, controller);
        }
    }

    bool VRInteractionSystem::raycastInteractables(const glm::vec3& origin, const glm::vec3& direction,
                                                     float maxDistance, RaycastHit& outHit)
    {
        // TODO: Implement proper raycasting against VRInteractableComponent entities
        // This would check collision with physics colliders or bounding boxes

        auto view = getActiveScene()->registry.view<VRInteractableComponent, TransformComponent>();

        float closestDistance = maxDistance;
        bool hit = false;

        for (auto entity : view)
        {
            auto& interactable = view.get<VRInteractableComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            // Simple sphere intersection for now (grab distance = sphere radius)
            glm::vec3 toObject = transform.pos - origin;
            float t = glm::dot(toObject, direction);

            if (t < 0.0f || t > maxDistance)
                continue;

            glm::vec3 closestPoint = origin + direction * t;
            float dist = glm::distance(closestPoint, transform.pos);

            if (dist < interactable.grabDistance && t < closestDistance)
            {
                outHit.entity = entity;
                outHit.hitPoint = closestPoint;
                outHit.distance = t;
                closestDistance = t;
                hit = true;
            }
        }

        return hit;
    }

    void VRInteractionSystem::handleGrabbing(entt::entity controllerEntity, VRControllerComponent& controller,
                                              const TransformComponent& controllerTransform)
    {
        if (!controller.enableRaycasting)
            return;

        glm::vec3 origin = controllerTransform.pos;
        glm::vec3 direction = controllerTransform.quat * glm::vec3(0.0f, 0.0f, -1.0f);

        RaycastHit hit;
        bool hitInteractable = raycastInteractables(origin, direction, controller.rayLength, hit);

        // Check grip button to grab
        bool gripPressed = controller.state.grip > 0.9f;

        if (gripPressed && hitInteractable && hit.entity != entt::null)
        {
            auto* interactable = getActiveScene()->registry.try_get<VRInteractableComponent>(hit.entity);
            if (interactable && !interactable->isGrabbed)
            {
                // Grab the object
                interactable->isGrabbed = true;
                interactable->grabbingController = controllerEntity;

                // Calculate grab offset
                auto* objectTransform = getActiveScene()->registry.try_get<TransformComponent>(hit.entity);
                if (objectTransform)
                {
                    interactable->grabOffset = objectTransform->pos - controllerTransform.pos;
                    interactable->grabRotationOffset = glm::conjugate(controllerTransform.quat) * objectTransform->quat;
                }

                // Haptic feedback
                m_openXR.triggerHaptic(controller.hand, interactable->hapticAmplitude, 150.0f,
                                        interactable->hapticDuration);

                LOG(NEXO_INFO, "[VR] Grabbed object with {} hand", (int)controller.hand);
            }
        }
        else if (!gripPressed)
        {
            // Release any grabbed objects
            auto interactableView = getActiveScene()->registry.view<VRInteractableComponent>();
            for (auto entity : interactableView)
            {
                auto& interactable = interactableView.get<VRInteractableComponent>(entity);
                if (interactable.isGrabbed && interactable.grabbingController == controllerEntity)
                {
                    interactable.isGrabbed = false;
                    interactable.grabbingController = entt::null;
                    LOG(NEXO_INFO, "[VR] Released object");
                }
            }
        }

        // Update grabbed object position
        auto interactableView = getActiveScene()->registry.view<VRInteractableComponent, TransformComponent>();
        for (auto entity : interactableView)
        {
            auto [interactable, objectTransform] = interactableView.get<VRInteractableComponent, TransformComponent>(entity);
            if (interactable.isGrabbed && interactable.grabbingController == controllerEntity)
            {
                objectTransform.pos = controllerTransform.pos + interactable.grabOffset;
                objectTransform.quat = controllerTransform.quat * interactable.grabRotationOffset;
            }
        }
    }

    void VRInteractionSystem::handleUse(entt:entity controllerEntity, VRControllerComponent& controller)
    {
        // TODO: Implement "use" interaction (trigger button)
        // This would trigger callbacks on VRInteractableComponent when trigger is pressed
    }

    //==========================================================================
    // VRComfortSystem
    //==========================================================================

    void VRComfortSystem::update(const Timestep ts)
    {
        // TODO: Implement vignette rendering and boundary warnings
        // This would render post-processing effects when:
        // - Player is moving (vignette to reduce motion sickness)
        // - Player is near play area boundary (boundary grid visualization)

        auto view = getActiveScene()->registry.view<VRLocomotionComponent, VRHeadsetComponent, TransformComponent>();

        for (auto entity : view)
        {
            auto& locomotion = view.get<VRLocomotionComponent>(entity);
            auto& headset = view.get<VRHeadsetComponent>(entity);
            auto& transform = view.get<TransformComponent>(entity);

            // Calculate movement speed to determine vignette strength
            float speed = glm::length(headset.velocity);
            float movementVignetteStrength = 0.0f;

            if (speed > 0.1f && locomotion.enableVignetteDuringMovement)
            {
                // Increase vignette with speed
                movementVignetteStrength = glm::clamp(speed / 5.0f, 0.0f, 1.0f) * locomotion.vignetteStrength;
            }

            // Render vignette if needed
            if (movementVignetteStrength > 0.01f || headset.enableVignette)
            {
                float finalStrength = glm::max(movementVignetteStrength, headset.vignetteStrength);
                renderVignette(entity, locomotion, finalStrength);
            }
        }
    }

    void VRComfortSystem::renderVignette(entt::entity entity, const VRLocomotionComponent& locomotion,
                                          float strength)
    {
        // TODO: Implement vignette shader rendering
        // This would render a radial gradient overlay to reduce peripheral vision
        LOG(NEXO_TRACE, "[VR] Rendering vignette: strength={}", strength);
    }

    void VRComfortSystem::checkPlayAreaProximity(entt::entity entity, const TransformComponent& transform,
                                                   const VRPlayAreaComponent& playArea)
    {
        // TODO: Check distance to play area boundary and render warning grid
    }

    //==========================================================================
    // VRFrameSyncSystem
    //==========================================================================

    void VRFrameSyncSystem::update(const Timestep ts)
    {
        if (!m_openXR.isInitialized() || !m_openXR.isSessionRunning())
            return;

        // This should be called at the END of the frame, after all rendering
        m_openXR.endFrame();

        // Begin next frame
        m_openXR.beginFrame();
    }

} // namespace nexo
