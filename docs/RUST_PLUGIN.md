# Parallax Rust Plugin Guide

## How it works
- The editor now loads plugins from a `plugins` folder next to the executable.
- Plugins can expose a C ABI entrypoint named `ParallaxGetPluginApi` that returns a `ParallaxPluginApi` struct (`editor/src/plugins/PluginAbi.hpp`).
- The host passes a small callback surface so plugins can register menu items and log messages. Lifecycle callbacks (`on_load`, `on_unload`, `on_update`, `on_gui`) are optional.

## Build the sample Rust plugin
```powershell
cd plugins\rust\example
cargo build --release
# Copy the built DLL next to Parallax.exe
copy target\release\parallax_rust_example.dll ..\..\..\build-msvc2-rel\plugins\
```
On Linux/macOS use the analogous `.so`/`.dylib` output and place it in the same `plugins` folder next to the editor binary.

## Writing your own plugin (Rust)
1. Create a `cdylib` crate.
2. Define the FFI structs from `PluginAbi.hpp` in Rust (`plugins/rust/example/src/lib.rs` is a reference).
3. Export `#[no_mangle] extern "C" fn ParallaxGetPluginApi() -> *const ParallaxPluginApi`.
4. In `on_load`, call the provided `register_menu_item` to expose actions inside the editor.
5. Use the provided `log` callback for diagnostics; levels: 0=info, 1=warn, 2=error.

## Plugin search path
- On startup the editor scans `<exe-dir>/plugins` for `.dll` (Windows) or `.so` (Linux) files.
- Use the Plugins → Reload Plugins menu to rescan at runtime.
