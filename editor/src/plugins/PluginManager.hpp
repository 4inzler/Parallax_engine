//// PluginManager.hpp //////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: Manages loading, unloading, and lifecycle of plugins
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPlugin.hpp"
#include <map>
#include <memory>
#include <vector>
#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
    typedef HMODULE PluginHandle;
#else
    #include <dlfcn.h>
    typedef void* PluginHandle;
#endif

namespace parallax::editor::plugins {

    /**
     * @brief Manages editor plugins
     *
     * Singleton class responsible for:
     * - Loading plugins from DLLs/SOs
     * - Managing plugin lifecycle
     * - Providing plugin registration services
     * - Coordinating plugin callbacks
     */
    class PluginManager {
    public:
        /**
         * @brief Get singleton instance
         */
        static PluginManager& getInstance();

        // Delete copy/move constructors and assignment operators
        PluginManager(const PluginManager&) = delete;
        PluginManager(PluginManager&&) = delete;
        PluginManager& operator=(const PluginManager&) = delete;
        PluginManager& operator=(PluginManager&&) = delete;

        /**
         * @brief Load a plugin from file
         * @param path Path to plugin DLL/SO
         * @return true if plugin loaded successfully
         */
        bool loadPlugin(const std::string& path);

        /**
         * @brief Load all plugins from a directory (non-recursive)
         * @return Number of successfully loaded plugins
         */
        size_t loadPluginsFromDirectory(const std::filesystem::path& directory);

        /**
         * @brief Unload a plugin by name
         * @param pluginName Name of plugin to unload
         * @return true if plugin was unloaded
         */
        bool unloadPlugin(const std::string& pluginName);

        /**
         * @brief Unload all plugins
         */
        void unloadAllPlugins();

        /**
         * @brief Check if a plugin is loaded
         */
        bool isPluginLoaded(const std::string& pluginName) const;

        /**
         * @brief Get list of loaded plugin names
         */
        std::vector<std::string> getLoadedPlugins() const;

        /**
         * @brief Get plugin info by name
         */
        PluginInfo getPluginInfo(const std::string& pluginName) const;

        /**
         * @brief Update all plugins
         * @param deltaTime Time since last frame
         */
        void updatePlugins(float deltaTime);

        /**
         * @brief Render all plugin UIs
         */
        void renderPluginGuis();

        // Registration functions for plugins

        /**
         * @brief Register a menu item from a plugin
         */
        void registerMenuItem(const MenuItemRegistration& menuItem);

        /**
         * @brief Register a custom importer
         */
        void registerImporter(std::shared_ptr<IImporter> importer);

        /**
         * @brief Register a custom panel/window
         */
        void registerPanel(std::shared_ptr<IDocumentWindow> panel);

        /**
         * @brief Get all registered menu items
         */
        const std::vector<MenuItemRegistration>& getMenuItems() const { return m_menuItems; }

    private:
        PluginManager() = default;
        ~PluginManager();

        struct LoadedPlugin {
            std::unique_ptr<IPlugin> instance;
            PluginHandle handle;
            PluginInfo info;
            std::string path;
        };

        std::map<std::string, LoadedPlugin> m_plugins;
        std::vector<MenuItemRegistration> m_menuItems;
        std::vector<std::shared_ptr<IImporter>> m_importers;
        std::vector<std::shared_ptr<IDocumentWindow>> m_panels;

        PluginHandle loadLibrary(const std::string& path);
        void unloadLibrary(PluginHandle handle);
        PluginFactoryFunc getFactoryFunction(PluginHandle handle);
    };

} // namespace parallax::editor::plugins
