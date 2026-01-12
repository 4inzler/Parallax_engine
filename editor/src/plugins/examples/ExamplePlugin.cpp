//// ExamplePlugin.cpp /////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: Example plugin demonstrating the plugin API
//
///////////////////////////////////////////////////////////////////////////////

#include "../IPlugin.hpp"
#include "../PluginManager.hpp"
#include "Logger.hpp"
#include <imgui.h>

namespace nexo::editor::plugins {

    /**
     * @brief Example plugin that adds a custom menu item and window
     */
    class ExamplePlugin : public IPlugin {
    public:
        PluginInfo getInfo() const override
        {
            return {
                "Example Plugin",      // name
                "1.0.0",              // version
                "Parallax Team",      // author
                "Demonstrates plugin capabilities",  // description
                {}                    // dependencies
            };
        }

        bool onLoad(PluginManager& manager) override
        {
            LOG(NEXO_INFO, "Example Plugin: onLoad() called");

            // Register a menu item
            MenuItemRegistration menuItem;
            menuItem.menuPath = "Tools/Example Plugin/Open Example Window";
            menuItem.icon = "";  // No icon
            menuItem.shortcut = "Ctrl+Alt+E";
            menuItem.callback = [this]() {
                m_windowOpen = !m_windowOpen;
                LOG(NEXO_INFO, "Example window toggled: {}", m_windowOpen ? "open" : "closed");
            };

            manager.registerMenuItem(menuItem);

            // Register a separator and another menu item
            MenuItemRegistration separator;
            separator.menuPath = "Tools/Example Plugin/";
            separator.separator = true;
            manager.registerMenuItem(separator);

            MenuItemRegistration aboutItem;
            aboutItem.menuPath = "Tools/Example Plugin/About";
            aboutItem.callback = [this]() {
                m_aboutOpen = true;
            };
            manager.registerMenuItem(aboutItem);

            return true;
        }

        void onUnload() override
        {
            LOG(NEXO_INFO, "Example Plugin: onUnload() called");
        }

        void onUpdate(float deltaTime) override
        {
            // Plugin update logic here
            // This is called every frame
        }

        void onGui() override
        {
            // Render plugin UI
            if (m_windowOpen)
            {
                if (ImGui::Begin("Example Plugin Window", &m_windowOpen))
                {
                    ImGui::Text("This is an example plugin window!");
                    ImGui::Separator();

                    ImGui::Text("Plugin features:");
                    ImGui::BulletText("Custom menu items");
                    ImGui::BulletText("Custom windows/panels");
                    ImGui::BulletText("Custom importers");
                    ImGui::BulletText("Asset processors");

                    ImGui::Separator();

                    if (ImGui::Button("Click me!"))
                    {
                        m_clickCount++;
                        LOG(NEXO_INFO, "Example button clicked {} times", m_clickCount);
                    }

                    ImGui::SameLine();
                    ImGui::Text("Clicks: %d", m_clickCount);

                    ImGui::Separator();

                    ImGui::TextWrapped(
                        "This example demonstrates how to create a plugin for the "
                        "Parallax Engine editor. Plugins can extend the editor with "
                        "custom functionality, tools, and workflows."
                    );
                }
                ImGui::End();
            }

            if (m_aboutOpen)
            {
                if (ImGui::Begin("About Example Plugin", &m_aboutOpen, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("Example Plugin");
                    ImGui::Text("Version: 1.0.0");
                    ImGui::Text("Author: Parallax Team");
                    ImGui::Separator();
                    ImGui::TextWrapped("This is a demonstration plugin showing how to use the plugin API.");

                    if (ImGui::Button("Close"))
                    {
                        m_aboutOpen = false;
                    }
                }
                ImGui::End();
            }
        }

    private:
        bool m_windowOpen = false;
        bool m_aboutOpen = false;
        int m_clickCount = 0;
    };

} // namespace nexo::editor::plugins

// Export the plugin
EXPORT_PLUGIN(nexo::editor::plugins::ExamplePlugin)
