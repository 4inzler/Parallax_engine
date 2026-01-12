//// IPlugin.hpp ///////////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: Interface for Parallax Engine plugins
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace nexo::editor::plugins {

    // Forward declarations
    class PluginManager;
    struct MenuItemRegistration;
    class IImporter;
    class IDocumentWindow;

    /**
     * @brief Plugin metadata
     */
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string author;
        std::string description;
        std::vector<std::string> dependencies;
    };

    /**
     * @brief Base interface for all plugins
     *
     * Plugins extend the editor with custom functionality:
     * - Custom importers (FBX, Collada, etc.)
     * - Custom panels/windows
     * - Menu items and tools
     * - Asset processors
     */
    class IPlugin {
    public:
        virtual ~IPlugin() = default;

        /**
         * @brief Get plugin metadata
         */
        virtual PluginInfo getInfo() const = 0;

        /**
         * @brief Called when plugin is loaded
         * @param manager Reference to plugin manager for registration
         * @return true if initialization succeeded
         */
        virtual bool onLoad(PluginManager& manager) = 0;

        /**
         * @brief Called when plugin is unloaded
         */
        virtual void onUnload() = 0;

        /**
         * @brief Called every frame for active plugins
         * @param deltaTime Time since last frame in seconds
         */
        virtual void onUpdate(float deltaTime) {}

        /**
         * @brief Called when plugin UI should be rendered
         */
        virtual void onGui() {}
    };

    /**
     * @brief Menu item callback signature
     */
    using MenuCallback = std::function<void()>;

    /**
     * @brief Menu item registration data
     */
    struct MenuItemRegistration {
        std::string menuPath;        // e.g., "Tools/My Plugin/Do Something"
        std::string icon;            // FontAwesome icon
        std::string shortcut;        // e.g., "Ctrl+Alt+P"
        MenuCallback callback;
        bool separator = false;      // Add separator before this item
    };

    /**
     * @brief Factory function signature for creating plugins
     * Plugins must export a function with this signature named "CreatePlugin"
     */
    using PluginFactoryFunc = IPlugin*(*)();

} // namespace nexo::editor::plugins

/**
 * @brief Macro to export plugin factory function
 *
 * Usage in plugin .cpp:
 * EXPORT_PLUGIN(MyPluginClass)
 */
#if defined(_WIN32) || defined(_WIN64)
    #define PLUGIN_EXPORT __declspec(dllexport)
#else
    #define PLUGIN_EXPORT __attribute__((visibility("default")))
#endif

#define EXPORT_PLUGIN(PluginClass) \
    extern "C" PLUGIN_EXPORT nexo::editor::plugins::IPlugin* CreatePlugin() { \
        return new PluginClass(); \
    }
