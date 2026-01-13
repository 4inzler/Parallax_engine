//// CAbiPluginAdapter.cpp /////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: Adapter to allow C ABI plugins (Rust/C) to run inside the editor
//
///////////////////////////////////////////////////////////////////////////////

#include "CAbiPluginAdapter.hpp"
#include "PluginManager.hpp"
#include "Logger.hpp"

namespace parallax::editor::plugins {
namespace {

    // Simple severity mapper for host logging
    void logFromPlugin(const int level, const char* message, void* userData)
    {
        const char* text = message ? message : "";
        const auto* state = static_cast<CAbiPluginAdapter::HostState*>(userData);
        const std::string pluginName = state ? state->pluginName : "unknown";

        switch (level) {
            case 2:
                LOG(PARALLAX_ERROR, "[Plugin:{}] {}", pluginName, text);
                break;
            case 1:
                LOG(PARALLAX_WARN, "[Plugin:{}] {}", pluginName, text);
                break;
            default:
                LOG(PARALLAX_INFO, "[Plugin:{}] {}", pluginName, text);
                break;
        }
    }

    bool registerMenuItem(const ParallaxMenuItemC* item, void* userData)
    {
        if (!item)
            return false;

        auto* state = static_cast<CAbiPluginAdapter::HostState*>(userData);
        if (state == nullptr || state->manager == nullptr) {
            LOG(PARALLAX_ERROR, "Plugin requested menu registration but host state is missing");
            return false;
        }

        MenuItemRegistration registration;
        registration.menuPath = item->menu_path ? item->menu_path : "";
        registration.icon = item->icon ? item->icon : "";
        registration.shortcut = item->shortcut ? item->shortcut : "";
        registration.separator = item->separator;

        if (item->callback) {
            const auto cb = item->callback;
            void* cbUserData = item->user_data;
            registration.callback = [cb, cbUserData]() {
                cb(cbUserData);
            };
        } else {
            registration.callback = []() {};
        }

        state->manager->registerMenuItem(registration);
        return true;
    }

    PluginInfo toPluginInfo(const ParallaxPluginApi* api)
    {
        PluginInfo info{};
        if (api == nullptr)
            return info;

        const auto& src = api->info;
        if (src.name) info.name = src.name;
        if (src.version) info.version = src.version;
        if (src.author) info.author = src.author;
        if (src.description) info.description = src.description;

        if (src.dependencies && src.dependency_count > 0) {
            info.dependencies.reserve(src.dependency_count);
            for (uint32_t i = 0; i < src.dependency_count; ++i) {
                if (src.dependencies[i])
                    info.dependencies.emplace_back(src.dependencies[i]);
            }
        }

        return info;
    }

} // namespace

CAbiPluginAdapter::CAbiPluginAdapter(const ParallaxPluginApi* api, PluginManager& manager)
    : m_api(api), m_manager(manager), m_info(toPluginInfo(api))
{
    m_hostContext.register_menu_item = &registerMenuItem;
    m_hostContext.log = &logFromPlugin;
    m_hostContext.host_user_data = &m_hostState;
}

PluginInfo CAbiPluginAdapter::getInfo() const
{
    return m_info;
}

bool CAbiPluginAdapter::onLoad(PluginManager& manager)
{
    if (m_api == nullptr) {
        LOG(PARALLAX_ERROR, "Cannot load plugin: API is null");
        return false;
    }

    m_hostState.manager = &manager;
    m_hostState.pluginName = m_info.name;

    if (!m_api->on_load) {
        LOG(PARALLAX_WARN, "[Plugin:{}] on_load not provided, skipping initialization", m_info.name);
        return true;
    }

    return m_api->on_load(&m_hostContext, &m_state);
}

void CAbiPluginAdapter::onUnload()
{
    if (m_api && m_api->on_unload) {
        m_api->on_unload(m_state);
    }
    m_state = nullptr;
}

void CAbiPluginAdapter::onUpdate(const float deltaTime)
{
    if (m_api && m_api->on_update) {
        m_api->on_update(m_state, deltaTime);
    }
}

void CAbiPluginAdapter::onGui()
{
    if (m_api && m_api->on_gui) {
        m_api->on_gui(m_state);
    }
}

} // namespace parallax::editor::plugins
