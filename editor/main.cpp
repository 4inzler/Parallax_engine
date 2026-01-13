//// main.cpp /////////////////////////////////////////////////////////////////
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
//  Date:        10/11/2024
//  Description: Main file for the parallax editor
//
///////////////////////////////////////////////////////////////////////////////

#include "src/Editor.hpp"
#include "src/DocumentWindows/ConsoleWindow/ConsoleWindow.hpp"
#include "src/DocumentWindows/EditorScene/EditorScene.hpp"
#include "src/DocumentWindows/SceneTreeWindow/SceneTreeWindow.hpp"
#include "src/DocumentWindows/InspectorWindow/InspectorWindow.hpp"
#include "src/DocumentWindows/AssetManager/AssetManagerWindow.hpp"
#include "src/DocumentWindows/MaterialInspector/MaterialInspector.hpp"
#include "src/DocumentWindows/PrimitiveWindow/PrimitiveWindow.hpp"
#include "src/DocumentWindows/GameWindow/GameWindow.hpp"

#include <thread>
#include <loguru.hpp>
#include <core/exceptions/Exceptions.hpp>

#include "scripting/native/ManagedTypedef.hpp"
#include "scripting/native/Scripting.hpp"

int main(int argc, char **argv)
try {
    loguru::init(argc, argv);
    loguru::g_stderr_verbosity = loguru::Verbosity_3;
    parallax::editor::Editor &editor = parallax::editor::Editor::getInstance();

    editor.registerWindow<parallax::editor::EditorScene>(
        std::format("Default Scene{}{}", PARALLAX_WND_USTRID_DEFAULT_SCENE, 0)
    );
    editor.registerWindow<parallax::editor::SceneTreeWindow>(PARALLAX_WND_USTRID_SCENE_TREE);
    editor.registerWindow<parallax::editor::InspectorWindow>(PARALLAX_WND_USTRID_INSPECTOR);
    editor.registerWindow<parallax::editor::ConsoleWindow>(PARALLAX_WND_USTRID_CONSOLE);
    editor.registerWindow<parallax::editor::MaterialInspector>(PARALLAX_WND_USTRID_MATERIAL_INSPECTOR);
    editor.registerWindow<parallax::editor::PrimitiveWindow>(PARALLAX_WND_USTRID_PRIMITIVE_WINDOW);
    editor.registerWindow<parallax::editor::AssetManagerWindow>(PARALLAX_WND_USTRID_ASSET_MANAGER);

    if (const auto defaultScene = editor.getWindow<parallax::editor::EditorScene>(std::format("Default Scene{}{}", PARALLAX_WND_USTRID_DEFAULT_SCENE, 0)).lock())
        defaultScene->setDefault();

    editor.init();

    while (editor.isOpen())
    {
        auto start = std::chrono::high_resolution_clock::now();
        editor.render();
        editor.update();

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::this_thread::sleep_for(std::chrono::milliseconds(16) - elapsed);
    }

    editor.shutdown();
    return 0;
} catch (const parallax::Exception &e) {
    LOG_EXCEPTION(e);
    return 1;
} catch (const std::exception &e) {
    LOG(PARALLAX_ERROR, "Unhandled exception: {}", e.what());
    return 1;
} catch (...) {
    LOG(PARALLAX_ERROR, "Unhandled unknown exception");
    return 1;
}
