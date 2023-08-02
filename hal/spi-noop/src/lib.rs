#![no_std]

use core::ffi::{c_int, c_void};

use c2a_core::hal::spi::*;

#[no_mangle]
pub extern "C" fn SPI_init(_spi_config: *mut SPI_Config) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn SPI_rx(
    _spi_config: *mut SPI_Config,
    _data: *mut c_void,
    _buffer_size: c_int,
) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn SPI_tx(
    _spi_config: *mut SPI_Config,
    _data: *mut c_void,
    _data_size: c_int,
) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn SPI_reopen(_spi_config: *mut SPI_Config, _reason: c_int) -> c_int {
    0
}

#[no_mangle]
pub extern "C" fn SPI_set_rx_length(_spi_config: *mut SPI_Config, _rx_length: u16) {}

#[no_mangle]
pub extern "C" fn SPI_set_cs_state_after_tx(
    _spi_config: *mut SPI_Config,
    _cs_state_after_tx: SPI_CS_STATE_AFTER_TX,
) {
}
