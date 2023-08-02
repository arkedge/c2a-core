#![no_std]
#![allow(clippy::missing_safety_doc)]

use core::ffi::c_int;

use c2a_core::hal::wdt::*;

#[no_mangle]
pub unsafe extern "C" fn WDT_initialize(config: *mut WDT_Config) -> c_int {
    let cfg = unsafe { &mut (*config) };

    WDT_set_timer(config, 7);

    cfg.is_clear_enable = 1;
    cfg.is_wdt_enable = 1;

    0
}

#[no_mangle]
pub extern "C" fn WDT_clear(_wdt_config: *mut WDT_Config) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn WDT_enable(_wdt_config: *mut WDT_Config) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn WDT_disable(_wdt_config: *mut WDT_Config) -> c_int {
    0
}

#[no_mangle]
pub unsafe extern "C" fn WDT_set_timer(config: *mut WDT_Config, time: c_int) -> c_int {
    let cfg = unsafe { &mut (*config) };

    cfg.timer_setting = time;

    0
}
