//// Panels.cpp ///////////////////////////////////////////////////////////////
//
// ⢀⢀⢀⣤⣤⣤⡀⢀⢀⢀⢀⢀⢀⢠⣤⡄⢀⢀⢀⢀⣠⣤⣤⣤⣤⣤⣤⣤⣤⣤⡀⢀⢀⢀⢠⣤⣄⢀⢀⢀⢀⢀⢀⢀⣤⣤⢀⢀⢀⢀⢀⢀⢀⢀⣀⣄⢀⢀⢠⣄⣀⢀⢀⢀⢀⢀⢀⢀
// ⢀⢀⢀⣿⣿⣿⣷⡀⢀⢀⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⡟⡛⡛⡛⡛⡛⡛⡛⢁⢀⢀⢀⢀⢻⣿⣦⢀⢀⢀⢀⢠⣾⡿⢃⢀⢀⢀⢀⢀⣠⣾⣿⢿⡟⢀⢀⡙⢿⢿⣿⣦⡀⢀⢀⢀⢀
// ⢀⢀⢀⣿⣿⡛⣿⣷⡀⢀⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⡇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⡙⣿⡷⢀⢀⣰⣿⡟⢁⢀⢀⢀⢀⢀⣾⣿⡟⢁⢀⢀⢀⢀⢀⢀⢀⡙⢿⣿⡆⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⡈⢿⣷⡄⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⣇⣀⣀⣀⣀⣀⣀⣀⢀⢀⢀⢀⢀⢀⢀⡈⢀⢀⣼⣿⢏⢀⢀⢀⢀⢀⢀⣼⣿⡏⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⡘⣿⣿⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⡈⢿⣿⡄⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⣿⢿⢿⢿⢿⢿⢿⢿⢇⢀⢀⢀⢀⢀⢀⢀⢠⣾⣿⣧⡀⢀⢀⢀⢀⢀⢀⣿⣿⡇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣿⣿⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⢀⡈⢿⣿⢀⢀⢸⣿⡇⢀⢀⢀⢀⣿⣿⡇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣰⣿⡟⡛⣿⣷⡄⢀⢀⢀⢀⢀⢿⣿⣇⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣿⣿⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⢀⢀⡈⢿⢀⢀⢸⣿⡇⢀⢀⢀⢀⡛⡟⢁⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⢀⣼⣿⡟⢀⢀⡈⢿⣿⣄⢀⢀⢀⢀⡘⣿⣿⣄⢀⢀⢀⢀⢀⢀⢀⢀⢀⣼⣿⢏⢀⢀⢀
// ⢀⢀⢀⣿⣿⢀⢀⢀⢀⢀⢀⢀⢀⢸⣿⡇⢀⢀⢀⢀⢀⣀⣀⣀⣀⣀⣀⣀⣀⣀⡀⢀⢀⢀⣠⣾⡿⢃⢀⢀⢀⢀⢀⢻⣿⣧⡀⢀⢀⢀⡈⢻⣿⣷⣦⣄⢀⢀⣠⣤⣶⣿⡿⢋⢀⢀⢀⢀
// ⢀⢀⢀⢿⢿⢀⢀⢀⢀⢀⢀⢀⢀⢸⢿⢃⢀⢀⢀⢀⢻⢿⢿⢿⢿⢿⢿⢿⢿⢿⢃⢀⢀⢀⢿⡟⢁⢀⢀⢀⢀⢀⢀⢀⡙⢿⡗⢀⢀⢀⢀⢀⡈⡉⡛⡛⢀⢀⢹⡛⢋⢁⢀⢀⢀⢀⢀⢀
//
//  Author:      Mehdy MORVAN
//  Date:        18/04/2025
//  Description: Source file for the ui panels
//
///////////////////////////////////////////////////////////////////////////////

#include "ImParallax/ImParallax.hpp"
#include "Parallax.hpp"
#include "Panels.hpp"
#include "Elements.hpp"
#include "Widgets.hpp"
#include "EntityProperties.hpp"
#include "CameraFactory.hpp"
#include "Path.hpp"
#include "IconsFontAwesome.h"
#include "assets/AssetCatalog.hpp"
#include "components/Camera.hpp"
#include "components/Transform.hpp"
#include "components/Uuid.hpp"
#include "context/Selector.hpp"
#include "context/actions/EntityActions.hpp"
#include "utils/EditorProps.hpp"
#include "context/ActionManager.hpp"

namespace ImParallax {
    bool MaterialInspector(parallax::components::Material &material)
	{
		bool modified = false;
		// --- Shader Selection ---
		ImGui::BeginGroup();
		{
			ImGui::Text("Shader:");
			ImGui::SameLine();

			static int currentShaderIndex = 0;
			const char* shaderOptions[] = { "Standard", "Unlit", "CustomPBR" };
			const float availableWidth = ImGui::GetContentRegionAvail().x;
			ImGui::SetNextItemWidth(availableWidth);

			if (ImGui::Combo("##ShaderCombo", &currentShaderIndex, shaderOptions, IM_ARRAYSIZE(shaderOptions)))
			{
				//TODO: implement shader selection
			}
		}
		ImGui::EndGroup();
		ImGui::Spacing();

		// --- Rendering mode selection ---
		ImGui::Text("Rendering mode:");
		ImGui::SameLine();
		static int currentRenderingModeIndex = 0;
		const char* renderingModeOptions[] = { "Opaque", "Transparent", "Refraction" };
		const float availableWidth = ImGui::GetContentRegionAvail().x;

		ImGui::SetNextItemWidth(availableWidth);
		if (ImGui::Combo("##RenderingModeCombo", &currentRenderingModeIndex, renderingModeOptions, IM_ARRAYSIZE(renderingModeOptions)))
		{
			//TODO: implement rendering mode
		}

    	auto& catalog = parallax::assets::AssetCatalog::getInstance();
	    // --- Albedo texture ---
        {
            static ImGuiColorEditFlags colorPickerModeAlbedo = ImGuiColorEditFlags_PickerHueBar;
		    static bool showColorPickerAlbedo = false;
		    const auto asset = material.albedoTexture.lock();
		    const auto albedoTexture = asset && asset->isLoaded() ? asset->getData()->texture : nullptr;

			std::filesystem::path newTexturePath;
		    if (TextureButton("Albedo texture", albedoTexture, newTexturePath)
		    	&& !newTexturePath.empty()) {
		    	// TODO: /!\ This is not futureproof, this would modify the texture for every asset that use this material
		    	const auto newTexture = catalog.createAsset<parallax::assets::Texture>(
					parallax::assets::AssetLocation(newTexturePath.filename().string()),
					newTexturePath
				);
		    	if (newTexture)
		    		material.albedoTexture = newTexture;
		    }
		    ImGui::SameLine();
		    modified = ColorEditor("##ColorEditor Albedo texture", &material.albedoColor, &colorPickerModeAlbedo, &showColorPickerAlbedo) || modified;
        }

		// --- Specular texture ---
        {
            static ImGuiColorEditFlags colorPickerModeSpecular = ImGuiColorEditFlags_PickerHueBar;
		    static bool showColorPickerSpecular = false;
		    const auto asset = material.metallicMap.lock();
		    const auto metallicTexture = asset && asset->isLoaded() ? asset->getData()->texture : nullptr;

			std::filesystem::path newTexturePath;
		    if (TextureButton("Specular texture", metallicTexture, newTexturePath)
		    	&& !newTexturePath.empty()) {
		    	// TODO: /!\ This is not futureproof, this would modify the texture for every asset that use this material
				const auto newTexture = catalog.createAsset<parallax::assets::Texture>(
					parallax::assets::AssetLocation(newTexturePath.filename().string()),
					newTexturePath
				);
		    	if (newTexture)
		    		material.metallicMap = newTexture;
		    }
		    ImGui::SameLine();
		    modified = ColorEditor("##ColorEditor Specular texture", &material.specularColor, &colorPickerModeSpecular, &showColorPickerSpecular) || modified;
		}
        return modified;
	}

    /**
    * @brief Creates a default perspective camera for the camera inspector preview.
    *
    * Sets up a perspective camera with a framebuffer for rendering the preview view.
    * Also adds a billboard with a camera icon for visualization in the scene.
    *
    * @param sceneId The ID of the scene where the camera should be created
    * @param sceneViewportSize The dimensions to use for the camera's framebuffer
    * @return The entity ID of the created camera
    */
	static parallax::ecs::Entity createDefaultPerspectiveCamera(const parallax::scene::SceneId sceneId, const ImVec2 sceneViewportSize)
	{
        auto &app = parallax::getApp();
        parallax::renderer::NxFramebufferSpecs framebufferSpecs;
        framebufferSpecs.attachments = {
            parallax::renderer::NxFrameBufferTextureFormats::RGBA8, parallax::renderer::NxFrameBufferTextureFormats::RED_INTEGER, parallax::renderer::NxFrameBufferTextureFormats::Depth
        };

        // Define layout: 60% for inspector, 40% for preview
        framebufferSpecs.width = static_cast<unsigned int>(sceneViewportSize.x);
        framebufferSpecs.height = static_cast<unsigned int>(sceneViewportSize.y);
        const auto renderTarget = parallax::renderer::NxFramebuffer::create(framebufferSpecs);
        const parallax::ecs::Entity defaultCamera = parallax::CameraFactory::createPerspectiveCamera({0.0f, 0.0f, -5.0f}, static_cast<unsigned int>(sceneViewportSize.x), static_cast<unsigned int>(sceneViewportSize.y), renderTarget);
        app.getSceneManager().getScene(sceneId).addEntity(defaultCamera);
        parallax::editor::utils::addPropsTo(defaultCamera, parallax::editor::utils::PropsType::CAMERA);
        return defaultCamera;
	}

	bool CameraInspector(const parallax::scene::SceneId sceneId)
	{
	    auto &app = parallax::getApp();
		static int undoStackSize = -1;
		// We store the current undo stack size so that when finalizing the camera creation,
		// we can remove the correct number of actions from the undo stack in order to only keep the camera creation action
		if (undoStackSize == -1)
		    undoStackSize = static_cast<int>(parallax::editor::ActionManager::get().getUndoStackSize());

        const ImVec2 availSize = ImGui::GetContentRegionAvail();
        const float totalWidth = availSize.x;
        float totalHeight = availSize.y - 40; // Reserve space for bottom buttons

        // Define layout: 60% for inspector, 40% for preview
        const float inspectorWidth = totalWidth * 0.4f;
        const float previewWidth = totalWidth - inspectorWidth - 8; // Subtract spacing between panels
        static parallax::ecs::Entity camera = parallax::ecs::MAX_ENTITIES;
        if (camera == parallax::ecs::MAX_ENTITIES)
        {
            camera = createDefaultPerspectiveCamera(sceneId, ImVec2(previewWidth, totalHeight));
        }

        static char cameraName[128] = "";
        static bool nameIsEmpty = false;
        static bool closingPopup = false;

        // We do this since imgui seems to render once more the popup, so we need to wait one frame before deleting
        // the render target
        if (closingPopup) {
            // Now it's safe to delete the entity
            app.deleteEntity(camera);

            camera = parallax::ecs::MAX_ENTITIES;
            cameraName[0] = '\0';
            nameIsEmpty = false;
            undoStackSize = -1;
            closingPopup = false;
            ImGui::CloseCurrentPopup();
            return true;
        }
        ImGui::Columns(2, "CameraCreatorColumns", false);

        ImGui::SetColumnWidth(0, inspectorWidth);
        // --- Left Side: Camera Inspector ---
        {
            ImGui::BeginChild("CameraInspector", ImVec2(inspectorWidth - 4, totalHeight), true);
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Name");
            ImGui::SameLine();
            if (nameIsEmpty) {
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            }
            ImGui::InputText("##CameraName", cameraName, IM_ARRAYSIZE(cameraName));
            if (nameIsEmpty) {
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();

                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
                ImGui::TextWrapped("Name is empty");
                ImGui::PopStyleColor();
                ImGui::Spacing();
            } else {
                ImGui::Spacing();
            }
            if (nameIsEmpty && cameraName[0] != '\0')
                nameIsEmpty = false;
            ImGui::Spacing();

            if (Header("##CameraNode", "Camera"))
            {
                auto &cameraComponent = parallax::Application::m_coordinator->getComponent<parallax::components::CameraComponent>(camera);
                cameraComponent.render = true;
                static parallax::components::CameraComponent::Memento beforeState;
                auto cameraComponentCopy = cameraComponent;
                resetItemStates();
                Camera(cameraComponent);
                if (isItemActivated()) {
                    beforeState = cameraComponentCopy.save();
                } else if (isItemDeactivated()) {
                    auto afterState = cameraComponent.save();
                    auto action = std::make_unique<parallax::editor::ComponentChangeAction<parallax::components::CameraComponent>>(camera, beforeState, afterState);
                    parallax::editor::ActionManager::get().recordAction(std::move(action));
                }
                ImGui::TreePop();
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            if (Header("##TransformNode", "Transform Component"))
            {
                static glm::vec3 lastDisplayedEuler(0.0f);
                auto &transformComponent = parallax::Application::m_coordinator->getComponent<parallax::components::TransformComponent>(camera);
                static parallax::components::TransformComponent::Memento beforeState;
                resetItemStates();
                auto transformComponentCopy = transformComponent;
                Transform(transformComponent, lastDisplayedEuler);
                if (isItemActivated()) {
                    beforeState = transformComponentCopy.save();
                } else if (isItemDeactivated()) {
                    auto afterState = transformComponent.save();
                    auto action = std::make_unique<parallax::editor::ComponentChangeAction<parallax::components::TransformComponent>>(camera, beforeState, afterState);
                    parallax::editor::ActionManager::get().recordAction(std::move(action));
                }
                ImGui::TreePop();
            }

            if (parallax::Application::m_coordinator->entityHasComponent<parallax::components::PerspectiveCameraTarget>(camera) &&
                Header("##PerspectiveCameraTarget", "Camera Target Component"))
            {
                auto &cameraTargetComponent = parallax::Application::m_coordinator->getComponent<parallax::components::PerspectiveCameraTarget>(camera);
                parallax::components::PerspectiveCameraTarget::Memento beforeState{};
                resetItemStates();
                auto cameraTargetComponentCopy = cameraTargetComponent;
                CameraTarget(cameraTargetComponent);
                if (isItemActivated()) {
                    beforeState = cameraTargetComponentCopy.save();
                } else if (isItemDeactivated()) {
                    auto afterState = cameraTargetComponent.save();
                    auto action = std::make_unique<parallax::editor::ComponentChangeAction<parallax::components::PerspectiveCameraTarget>>(camera, beforeState, afterState);
                    parallax::editor::ActionManager::get().recordAction(std::move(action));
                }
                ImGui::TreePop();
            }

            if (parallax::Application::m_coordinator->entityHasComponent<parallax::components::PerspectiveCameraController>(camera) &&
                Header("##PerspectiveCameraController", "Camera Controller Component"))
            {
                auto &cameraControllerComponent = parallax::Application::m_coordinator->getComponent<parallax::components::PerspectiveCameraController>(camera);
                parallax::components::PerspectiveCameraController::Memento beforeState{};
                auto cameraControllerComponentCopy = cameraControllerComponent;
                resetItemStates();
                CameraController(cameraControllerComponent);
                if (isItemActivated()) {
                    beforeState = cameraControllerComponentCopy.save();
                } else if (isItemDeactivated()) {
                    auto afterState = cameraControllerComponent.save();
                    auto action = std::make_unique<parallax::editor::ComponentChangeAction<parallax::components::PerspectiveCameraController>>(camera, beforeState, afterState);
                    parallax::editor::ActionManager::get().recordAction(std::move(action));
                }
                ImGui::TreePop();
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            // Add Component button
            const float buttonWidth = inspectorWidth - 16;
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
            float centeredX = (inspectorWidth - buttonWidth) * 0.5f;
            ImGui::SetCursorPosX(centeredX);

            // Static variables for state tracking
            static bool showComponentSelector = false;
            static float animProgress = 0.0f;
            static double lastClickTime = 0.0f;

            // Button with arrow indicating state
            const std::string buttonText = "Add Component " + std::string(showComponentSelector ? ICON_FA_CHEVRON_UP : ICON_FA_CHEVRON_DOWN);

            if (Button(buttonText, ImVec2(buttonWidth, 0)))
            {
                showComponentSelector = !showComponentSelector;
                if (showComponentSelector) {
                    lastClickTime = ImGui::GetTime();
                    animProgress = 0.0f;
                }
            }
            ImGui::PopStyleVar();

            // Component selector with just two options
            if (showComponentSelector)
            {
                // Animation calculation
                constexpr float animDuration = 0.25f;
                auto timeSinceClick = static_cast<float>(ImGui::GetTime() - lastClickTime);
                animProgress = std::min(timeSinceClick / animDuration, 1.0f);

                // Simplified component grid with compact layout
                constexpr float maxGridHeight = 90.0f;
                const float currentHeight = maxGridHeight * animProgress;

                // Create child window for components with animated height
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 3.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // Reduce spacing between items
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8)); // Better padding inside items

                ImGui::BeginChild("ComponentSelector", ImVec2(buttonWidth, currentHeight), 0, ImGuiWindowFlags_NoScrollbar);

                if (animProgress > 0.5f)
                {

                    // Draw component buttons side-by-side with controlled spacing
                    ImGui::BeginGroup();

                    if (!parallax::Application::m_coordinator->entityHasComponent<parallax::components::PerspectiveCameraTarget>(camera) &&
                        !parallax::Application::m_coordinator->entityHasComponent<parallax::components::PerspectiveCameraController>(camera) &&
                        ButtonWithIconAndText("camera_target", ICON_FA_CAMERA, "Camera target", ImVec2(75.0f, 75.0f)))
                    {
                        auto action = std::make_unique<parallax::editor::ComponentAddAction<parallax::components::PerspectiveCameraTarget>>(camera);
                        parallax::editor::ActionManager::get().recordAction(std::move(action));
                        parallax::components::PerspectiveCameraTarget cameraTarget{};
                        parallax::Application::m_coordinator->addComponent(camera, cameraTarget);
                        showComponentSelector = false;
                    }
                    ImGui::SameLine();
                    if (!parallax::Application::m_coordinator->entityHasComponent<parallax::components::PerspectiveCameraTarget>(camera) &&
                        !parallax::Application::m_coordinator->entityHasComponent<parallax::components::PerspectiveCameraController>(camera) &&
                        ButtonWithIconAndText("camera_controller", ICON_FA_GAMEPAD, "Camera Controller", ImVec2(75.0f, 75.0f)))
                    {
                        auto action = std::make_unique<parallax::editor::ComponentAddAction<parallax::components::PerspectiveCameraController>>(camera);
                        parallax::editor::ActionManager::get().recordAction(std::move(action));
                        parallax::components::PerspectiveCameraController cameraController{};
                        parallax::Application::m_coordinator->addComponent(camera, cameraController);
                        showComponentSelector = false;
                    }
                    ImGui::EndGroup();
                }

                ImGui::EndChild();
                ImGui::PopStyleVar(3);

                // Reset animation if needed
                if (!showComponentSelector && animProgress >= 1.0f) {
                    animProgress = 0.0f;
                }
            }

            ImGui::EndChild(); // End CameraInspector
        }
        ImGui::NextColumn();
        // --- Right Side: Camera Preview ---
        {
            ImGui::BeginChild("CameraPreview", ImVec2(previewWidth - 4, totalHeight), true);

            parallax::Application::SceneInfo sceneInfo{sceneId, parallax::RenderingType::FRAMEBUFFER};
            app.run(sceneInfo);
            auto const &cameraComponent = parallax::Application::m_coordinator->getComponent<parallax::components::CameraComponent>(camera);
            const unsigned int textureId = cameraComponent.m_renderTarget->getColorAttachmentId(0);

            const float displayHeight = totalHeight - 20;
            const float displayWidth = displayHeight;

            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 4, ImGui::GetCursorPosY() + 4));
            Image(static_cast<ImTextureID>(static_cast<intptr_t>(textureId)),
                        ImVec2(displayWidth, displayHeight));

            ImGui::EndChild();
        }

        ImGui::Columns(1);
        ImGui::Spacing();

        // Bottom buttons - centered
        constexpr float buttonWidth = 120.0f;

        if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
        {
            if (cameraName[0] == '\0') {
                nameIsEmpty = true;
                return false;
            }
            nameIsEmpty = false;
            auto &selector = parallax::editor::Selector::get();
            const auto &uuid = parallax::Application::m_coordinator->getComponent<parallax::components::UuidComponent>(camera);
            auto &cameraComponent = parallax::Application::m_coordinator->getComponent<parallax::components::CameraComponent>(camera);
            cameraComponent.active = false;
            selector.setUiHandle(uuid.uuid, std::string(ICON_FA_CAMERA "  ") + cameraName);
            unsigned int stackSize = parallax::editor::ActionManager::get().getUndoStackSize() - undoStackSize;
            parallax::editor::ActionManager::get().clearHistory(stackSize);
            auto action = std::make_unique<parallax::editor::EntityCreationAction>(camera);
            parallax::editor::ActionManager::get().recordAction(std::move(action));
            camera = parallax::ecs::MAX_ENTITIES;
            cameraName[0] = '\0';
            undoStackSize = -1;
            ImGui::CloseCurrentPopup();
            return true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            unsigned int stackSize = parallax::editor::ActionManager::get().getUndoStackSize() - undoStackSize;
            parallax::editor::ActionManager::get().clearHistory(stackSize);
            closingPopup = true;
            return false;
        }
        return false;
	}
}
