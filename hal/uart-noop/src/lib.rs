#![no_std]

use core::ffi::{c_int, c_void};

use c2a_core::hal::uart::*;

#[no_mangle]
pub extern "C" fn UART_init(_uart_config: *mut UART_Config) -> c_int {
    UART_ERR_CODE_UART_OK.0
}

#[no_mangle]
pub extern "C" fn UART_rx(
    _uart_config: *mut UART_Config,
    _data: *mut c_void,
    _buffer_size: c_int,
) -> c_int {
    UART_ERR_CODE_UART_OK.0
}

#[no_mangle]
pub extern "C" fn UART_tx(
    _uart_config: *mut UART_Config,
    _data: *mut c_void,
    _data_size: c_int,
) -> c_int {
    UART_ERR_CODE_UART_OK.0
}

#[no_mangle]
pub extern "C" fn UART_reopen(_uart_config: *mut UART_Config, _reason: c_int) -> c_int {
    UART_ERR_CODE_UART_OK.0
}
