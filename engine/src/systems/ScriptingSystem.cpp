//// ScriptingSystem.cpp //////////////////////////////////////////////////////
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
//  Author:      Guillaume HEIN
//  Date:        21/06/2025
//  Description: Source file for the scripting system
//
///////////////////////////////////////////////////////////////////////////////

#include "ScriptingSystem.hpp"

#include "scripting/native/Scripting.hpp"

namespace parallax::system {

    ScriptingSystem::ScriptingSystem()
    {
        scripting::HostHandler::Parameters params;
        params.errorCallback = [this](const scripting::HostString& message) {
            LOG(PARALLAX_ERROR, "Scripting host error: {}", message.to_utf8());
            m_latestScriptingError = message.to_utf8();
        };

        scripting::HostHandler& host = scripting::HostHandler::getInstance();

        // Initialize the host
        if (host.initialize(params) != scripting::HostHandler::SUCCESS) {
            LOG(PARALLAX_ERROR, "Failed to initialize host");
            THROW_EXCEPTION(scripting::ScriptingBackendInitFailed, m_latestScriptingError);
        }
    }

    int ScriptingSystem::init()
    {

        const auto &scriptHost = scripting::HostHandler::getInstance();

        updateWorldState();
        if (auto ret = scriptHost.getManagedApi().SystemBase.InitializeComponents(); ret != 0) {
            LOG(PARALLAX_ERROR, "Failed to initialize scripting components, returned: {}", ret);
            return ret;
        }
        LOG(PARALLAX_INFO, "Scripting components initialized successfully");
        if (auto ret = scriptHost.getManagedApi().SystemBase.InitializeSystems(&m_worldState, sizeof(m_worldState)); ret != 0) {
            LOG(PARALLAX_ERROR, "Failed to initialize scripting systems, returned: {}", ret);
            return ret;
        }
        LOG(PARALLAX_INFO, "Scripting systems initialized successfully");

        if (scriptHost.runScriptExample() == EXIT_FAILURE) {
            LOG(PARALLAX_ERROR, "Error in runScriptExample");
            return 1;
        }

        LOG(PARALLAX_INFO, "Successfully ran runScriptExample");
        return 0;
    }

    int ScriptingSystem::update()
    {
        const auto &scriptHost = scripting::HostHandler::getInstance();
        const auto &api = scriptHost.getManagedApi();

        updateWorldState();
        if (const auto ret = api.SystemBase.UpdateSystems(&m_worldState, sizeof(m_worldState)); ret != 0) {
            LOG_ONCE(PARALLAX_ERROR, "Failed to update scripting systems");
            return ret;
        }
        Logger::resetOnce(PARALLAX_LOG_ONCE_KEY("Failed to update scripting systems"));
        return 0;
    }

    int ScriptingSystem::shutdown()
    {
        const auto &scriptHost = scripting::HostHandler::getInstance();
        const auto &api = scriptHost.getManagedApi();

        updateWorldState();
        if (auto ret = api.SystemBase.ShutdownSystems(&m_worldState, sizeof(m_worldState)); ret != 0) {
            LOG(PARALLAX_ERROR, "Failed to shutdown scripting systems: {}", ret);
            return ret;
        }
        LOG(PARALLAX_INFO, "Scripting systems shutdown successfully");
        return 0;
    }

    void ScriptingSystem::updateWorldState()
    {
        Application& app = Application::getInstance();
        m_worldState.update(app.getWorldState());
    }

} // namespace parallax::system