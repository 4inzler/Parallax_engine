//// PluginManager.cpp //////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: Implementation of Plugin Manager
//
///////////////////////////////////////////////////////////////////////////////

#include "PluginManager.hpp"
#include "Logger.hpp"
#include <filesystem>

namespace parallax::editor::plugins {

    PluginManager& PluginManager::getInstance()
    {
        static PluginManager instance;
        return instance;
    }

    PluginManager::~PluginManager()
    {
        unloadAllPlugins();
    }

    PluginHandle PluginManager::loadLibrary(const std::string& path)
    {
#ifdef _WIN32
        return LoadLibraryA(path.c_str());
#else
        return dlopen(path.c_str(), RTLD_LAZY);
#endif
    }

    void PluginManager::unloadLibrary(PluginHandle handle)
    {
        if (!handle) return;

#ifdef _WIN32
        FreeLibrary(handle);
#else
        dlclose(handle);
#endif
    }

    PluginFactoryFunc PluginManager::getFactoryFunction(PluginHandle handle)
    {
        if (!handle) return nullptr;

#ifdef _WIN32
        return reinterpret_cast<PluginFactoryFunc>(
            GetProcAddress(handle, "CreatePlugin"));
#else
        return reinterpret_cast<PluginFactoryFunc>(
            dlsym(handle, "CreatePlugin"));
#endif
    }

    bool PluginManager::loadPlugin(const std::string& path)
    {
        if (!std::filesystem::exists(path))
        {
            LOG(PARALLAX_ERROR, "Plugin file not found: {}", path);
            return false;
        }

        // Load the library
        PluginHandle handle = loadLibrary(path);
        if (!handle)
        {
#ifdef _WIN32
            LOG(PARALLAX_ERROR, "Failed to load plugin: {} (Error: {})", path, GetLastError());
#else
            LOG(PARALLAX_ERROR, "Failed to load plugin: {} (Error: {})", path, dlerror());
#endif
            return false;
        }

        // Get the factory function
        PluginFactoryFunc createPlugin = getFactoryFunction(handle);
        if (!createPlugin)
        {
            LOG(PARALLAX_ERROR, "Plugin {} does not export CreatePlugin function", path);
            unloadLibrary(handle);
            return false;
        }

        // Create plugin instance
        std::unique_ptr<IPlugin> plugin(createPlugin());
        if (!plugin)
        {
            LOG(PARALLAX_ERROR, "Failed to create plugin instance from {}", path);
            unloadLibrary(handle);
            return false;
        }

        // Get plugin info
        PluginInfo info = plugin->getInfo();

        // Check if plugin with same name is already loaded
        if (isPluginLoaded(info.name))
        {
            LOG(PARALLAX_WARN, "Plugin {} is already loaded", info.name);
            unloadLibrary(handle);
            return false;
        }

        // Initialize the plugin
        if (!plugin->onLoad(*this))
        {
            LOG(PARALLAX_ERROR, "Plugin {} failed to initialize", info.name);
            unloadLibrary(handle);
            return false;
        }

        // Store the loaded plugin
        LoadedPlugin loadedPlugin;
        loadedPlugin.instance = std::move(plugin);
        loadedPlugin.handle = handle;
        loadedPlugin.info = info;
        loadedPlugin.path = path;

        m_plugins[info.name] = std::move(loadedPlugin);

        LOG(PARALLAX_INFO, "Plugin loaded: {} v{} by {}", info.name, info.version, info.author);
        return true;
    }

    bool PluginManager::unloadPlugin(const std::string& pluginName)
    {
        auto it = m_plugins.find(pluginName);
        if (it == m_plugins.end())
        {
            LOG(PARALLAX_WARN, "Plugin {} is not loaded", pluginName);
            return false;
        }

        // Call plugin cleanup
        it->second.instance->onUnload();

        // Unload the library
        unloadLibrary(it->second.handle);

        // Remove from map
        m_plugins.erase(it);

        LOG(PARALLAX_INFO, "Plugin unloaded: {}", pluginName);
        return true;
    }

    void PluginManager::unloadAllPlugins()
    {
        for (auto& [name, plugin] : m_plugins)
        {
            plugin.instance->onUnload();
            unloadLibrary(plugin.handle);
        }
        m_plugins.clear();
        m_menuItems.clear();
        m_importers.clear();
        m_panels.clear();

        LOG(PARALLAX_INFO, "All plugins unloaded");
    }

    bool PluginManager::isPluginLoaded(const std::string& pluginName) const
    {
        return m_plugins.find(pluginName) != m_plugins.end();
    }

    std::vector<std::string> PluginManager::getLoadedPlugins() const
    {
        std::vector<std::string> names;
        names.reserve(m_plugins.size());
        for (const auto& [name, _] : m_plugins)
        {
            names.push_back(name);
        }
        return names;
    }

    PluginInfo PluginManager::getPluginInfo(const std::string& pluginName) const
    {
        auto it = m_plugins.find(pluginName);
        if (it != m_plugins.end())
        {
            return it->second.info;
        }
        return {};
    }

    void PluginManager::updatePlugins(float deltaTime)
    {
        for (auto& [name, plugin] : m_plugins)
        {
            plugin.instance->onUpdate(deltaTime);
        }
    }

    void PluginManager::renderPluginGuis()
    {
        for (auto& [name, plugin] : m_plugins)
        {
            plugin.instance->onGui();
        }
    }

    void PluginManager::registerMenuItem(const MenuItemRegistration& menuItem)
    {
        m_menuItems.push_back(menuItem);
        LOG(PARALLAX_DEBUG, "Menu item registered: {}", menuItem.menuPath);
    }

    void PluginManager::registerImporter(std::shared_ptr<IImporter> importer)
    {
        m_importers.push_back(importer);
        LOG(PARALLAX_DEBUG, "Importer registered");
    }

    void PluginManager::registerPanel(std::shared_ptr<IDocumentWindow> panel)
    {
        m_panels.push_back(panel);
        LOG(PARALLAX_DEBUG, "Panel registered");
    }

} // namespace parallax::editor::plugins
