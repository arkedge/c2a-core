#![no_std]

use core::ffi::{c_int, c_void};

use c2a_core::hal::i2c::*;

#[no_mangle]
pub extern "C" fn I2C_init(_i2c_config: *mut I2C_Config) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn I2C_rx(
    _i2c_config: *mut I2C_Config,
    _data: *mut c_void,
    _buffer_size: c_int,
) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn I2C_tx(
    _i2c_config: *mut I2C_Config,
    _data: *mut c_void,
    _data_size: c_int,
) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn I2C_reopen(_i2c_config: *mut I2C_Config, _reason: c_int) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn I2C_set_stop_flag(_i2c_config: *mut I2C_Config, _stop_flag: u8) {}

#[no_mangle]
pub extern "C" fn I2C_set_rx_length(_i2c_config: *mut I2C_Config, _rx_length: u32) {}
