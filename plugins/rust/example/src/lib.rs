//! Minimal Rust plugin for the Parallax editor demonstrating the C ABI.

use std::ffi::CString;
use std::os::raw::{c_char, c_void};
use std::ptr;

const ABI_VERSION: u32 = 1;

#[repr(C)]
#[derive(Copy, Clone)]
pub struct ParallaxMenuItemC {
    menu_path: *const c_char,
    icon: *const c_char,
    shortcut: *const c_char,
    callback: Option<extern "C" fn(*mut c_void)>,
    user_data: *mut c_void,
    separator: bool,
}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct ParallaxPluginInfoC {
    name: *const c_char,
    version: *const c_char,
    author: *const c_char,
    description: *const c_char,
    dependencies: *const *const c_char,
    dependency_count: u32,
}

#[repr(C)]
pub struct ParallaxHostContextC {
    register_menu_item: Option<extern "C" fn(*const ParallaxMenuItemC, *mut c_void) -> bool>,
    log: Option<extern "C" fn(i32, *const c_char, *mut c_void)>,
    host_user_data: *mut c_void,
}

#[repr(C)]
pub struct ParallaxPluginApi {
    abi_version: u32,
    info: ParallaxPluginInfoC,
    on_load: Option<extern "C" fn(*const ParallaxHostContextC, *mut *mut c_void) -> bool>,
    on_unload: Option<extern "C" fn(*mut c_void)>,
    on_update: Option<extern "C" fn(*mut c_void, f32)>,
    on_gui: Option<extern "C" fn(*mut c_void)>,
}

unsafe impl Sync for ParallaxPluginInfoC {}
unsafe impl Sync for ParallaxPluginApi {}

struct PluginState {
    host: *const ParallaxHostContextC,
}

static MENU_PATH: &[u8] = b"Plugins/Rust Example/Hello from Rust\0";
static SHORTCUT: &[u8] = b"Ctrl+Shift+R\0";

unsafe fn host_log(host: *const ParallaxHostContextC, level: i32, message: &str) {
    if host.is_null() {
        return;
    }
    if let Some(log_fn) = (*host).log {
        if let Ok(c_msg) = CString::new(message) {
            log_fn(level, c_msg.as_ptr(), (*host).host_user_data);
        }
    }
}

extern "C" fn menu_callback(state_ptr: *mut c_void) {
    unsafe {
        if state_ptr.is_null() {
            return;
        }
        let state = &mut *(state_ptr as *mut PluginState);
        host_log(state.host, 0, "Rust plugin menu item clicked");
    }
}

extern "C" fn on_load(host: *const ParallaxHostContextC, out_state: *mut *mut c_void) -> bool {
    unsafe {
        if out_state.is_null() || host.is_null() {
            return false;
        }

        let state = Box::new(PluginState { host });
        let state_ptr = Box::into_raw(state) as *mut c_void;
        *out_state = state_ptr;

        // Register a menu item inside the editor (Plugins menu).
        if let Some(register_menu_item) = (*host).register_menu_item {
            let item = ParallaxMenuItemC {
                menu_path: MENU_PATH.as_ptr() as *const c_char,
                icon: ptr::null(),
                shortcut: SHORTCUT.as_ptr() as *const c_char,
                callback: Some(menu_callback),
                user_data: state_ptr,
                separator: false,
            };
            register_menu_item(&item, (*host).host_user_data);
        }

        host_log(host, 0, "Rust plugin loaded");
        true
    }
}

extern "C" fn on_unload(state_ptr: *mut c_void) {
    unsafe {
        if !state_ptr.is_null() {
            drop(Box::from_raw(state_ptr as *mut PluginState));
        }
    }
}

extern "C" fn on_update(state_ptr: *mut c_void, _delta: f32) {
    unsafe {
        // No-op for this sample, but state_ptr is preserved if needed.
        if state_ptr.is_null() {
            return;
        }
        let state = &mut *(state_ptr as *mut PluginState);
        let _ = state;
    }
}

static INFO: ParallaxPluginInfoC = ParallaxPluginInfoC {
    name: b"Rust Example\0".as_ptr() as *const c_char,
    version: b"0.1.0\0".as_ptr() as *const c_char,
    author: b"Parallax Rust SDK\0".as_ptr() as *const c_char,
    description: b"Demonstrates how to write a Rust plugin for Parallax\0".as_ptr() as *const c_char,
    dependencies: ptr::null(),
    dependency_count: 0,
};

static API: ParallaxPluginApi = ParallaxPluginApi {
    abi_version: ABI_VERSION,
    info: INFO,
    on_load: Some(on_load),
    on_unload: Some(on_unload),
    on_update: Some(on_update),
    on_gui: None,
};

#[no_mangle]
pub extern "C" fn ParallaxGetPluginApi() -> *const ParallaxPluginApi {
    &API
}
