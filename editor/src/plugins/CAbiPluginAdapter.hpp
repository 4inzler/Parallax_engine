//// CAbiPluginAdapter.hpp /////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: Adapter to expose C/Rust plugins through the IPlugin interface
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPlugin.hpp"
#include "PluginAbi.hpp"

namespace parallax::editor::plugins {

    class PluginManager;

    class CAbiPluginAdapter final : public IPlugin {
    public:
        CAbiPluginAdapter(const ParallaxPluginApi* api, PluginManager& manager);
        ~CAbiPluginAdapter() override = default;

        [[nodiscard]] PluginInfo getInfo() const override;
        bool onLoad(PluginManager& manager) override;
        void onUnload() override;
        void onUpdate(float deltaTime) override;
        void onGui() override;

    public:
        struct HostState {
            PluginManager* manager = nullptr;
            std::string pluginName;
        };

    private:
        const ParallaxPluginApi* m_api;
        PluginManager& m_manager;
        PluginInfo m_info{};
        void* m_state { nullptr };

        HostState m_hostState{};
        ParallaxHostContextC m_hostContext{};
    };

} // namespace parallax::editor::plugins
