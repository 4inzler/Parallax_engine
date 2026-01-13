//// PluginAbi.hpp ////////////////////////////////////////////////////////////
//
//  Author:      Parallax Engine Team
//  Date:        12/01/2026
//  Description: C ABI for external plugins (C/Rust, etc.)
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Increment if the ABI changes in an incompatible way
#define PARALLAX_PLUGIN_ABI_VERSION 1U

typedef struct ParallaxMenuItemC {
    const char *menu_path;     // e.g., "Tools/My Plugin/Do Stuff"
    const char *icon;          // FontAwesome icon text (optional, can be null)
    const char *shortcut;      // e.g., "Ctrl+Alt+P" (optional, can be null)
    void (*callback)(void *user_data);
    void *user_data;
    bool separator;            // Add separator before this item
} ParallaxMenuItemC;

typedef struct ParallaxPluginInfoC {
    const char *name;
    const char *version;
    const char *author;
    const char *description;
    const char **dependencies;   // optional array of null-terminated strings
    uint32_t dependency_count;
} ParallaxPluginInfoC;

// Host-facing functions exposed to plugins
typedef struct ParallaxHostContextC {
    // Returns true on success
    bool (*register_menu_item)(const ParallaxMenuItemC *item, void *host_user_data);
    // level: 0=info, 1=warn, 2=error
    void (*log)(int level, const char *message, void *host_user_data);
    void *host_user_data;
} ParallaxHostContextC;

typedef struct ParallaxPluginApi {
    uint32_t abi_version;  // must equal PARALLAX_PLUGIN_ABI_VERSION
    ParallaxPluginInfoC info;

    // Lifecycle
    bool (*on_load)(const ParallaxHostContextC *host, void **plugin_state);
    void (*on_unload)(void *plugin_state);
    void (*on_update)(void *plugin_state, float delta_time);
    void (*on_gui)(void *plugin_state);
} ParallaxPluginApi;

// Function name exported by plugins
typedef const ParallaxPluginApi *(*ParallaxGetPluginApiFn)();

#ifdef __cplusplus
}
#endif
