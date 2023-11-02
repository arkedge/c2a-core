use std::os::raw::c_void;

use c2a_core::hal::ccsds::CCSDS_Config;

use crate::ccsds_user_bind::{CCSDS_ERR_CODE, CCSDS_ERR_CODE_CCSDS_ERR_OK};

#[no_mangle]
pub extern "C" fn CCSDS_read_sequence(_select: u32, _uip_stat: *const u32) -> CCSDS_ERR_CODE {
    CCSDS_ERR_CODE_CCSDS_ERR_OK
}

#[no_mangle]
pub extern "C" fn CCSDS_get_buffer_num() -> u8 {
    8
}

#[no_mangle]
pub extern "C" fn CCSDS_set_rate(ui_rate: u32, config: *mut c_void) {
    let ui_rate = if ui_rate > 0xFF { 0xFF } else { ui_rate };

    let config = config as *mut CCSDS_Config;
    let config = unsafe { &mut *config };
    config.bitrate = 40000000 / ui_rate; // [bps]
}
