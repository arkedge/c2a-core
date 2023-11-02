pub mod ccsds_user;
pub mod ccsds_user_bind;

mod kble;

use ccsds_user_bind::{
    CCSDS_ERR_CODE_CCSDS_ERR_OK, CCSDS_ERR_CODE_CCSDS_ERR_RX_4KBPS,
    CCSDS_ERR_CODE_CCSDS_ERR_TX_INVALID,
};

use once_cell::sync::Lazy;
use tokio::sync::mpsc::{self, error::TryRecvError};

use std::{
    os::raw::{c_int, c_void},
    sync::Mutex,
};

struct KblePair {
    tlm_tx: mpsc::Sender<Vec<u8>>,
    cmd_rx: Mutex<mpsc::Receiver<Vec<u8>>>,
}

static KBLE: Lazy<KblePair> = Lazy::new(|| {
    let (tlm_tx, cmd_rx, socket) = kble::new();
    let cmd_rx = Mutex::new(cmd_rx);
    let kble_addr = std::env::var("IF_CCSDS_KBLE_ADDR");
    let kble_addr = kble_addr.unwrap_or("0.0.0.0:22545".to_string());
    socket.serve_in_background(kble_addr);
    KblePair { tlm_tx, cmd_rx }
});

#[no_mangle]
pub extern "C" fn CCSDS_init(my_ccsds_v: *mut c_void) -> c_int {
    ccsds_user::CCSDS_set_rate(0xAD, my_ccsds_v); // 初期値 230.4 [kbps]
    Lazy::force(&KBLE);
    CCSDS_ERR_CODE_CCSDS_ERR_OK.0
}

#[no_mangle]
pub extern "C" fn CCSDS_rx(
    _my_ccsds_v: *mut c_void,
    data_v: *mut c_void,
    buffer_size: c_int,
) -> c_int {
    let cmd_bytes = match KBLE.cmd_rx.lock().unwrap().try_recv() {
        Ok(cmd_bytes) => cmd_bytes,
        Err(TryRecvError::Empty) => return 0,
        _ => return CCSDS_ERR_CODE_CCSDS_ERR_RX_4KBPS.0,
    };
    let buf = unsafe { std::slice::from_raw_parts_mut(data_v as *mut u8, buffer_size as usize) };
    if cmd_bytes.len() > buf.len() {
        // buffer is too short
        return CCSDS_ERR_CODE_CCSDS_ERR_RX_4KBPS.0;
    }
    let len = cmd_bytes.len();
    buf[..len].copy_from_slice(&cmd_bytes[..]);
    len as c_int
}

#[no_mangle]
pub extern "C" fn CCSDS_tx(
    _my_ccsds_v: *mut c_void,
    data_v: *mut c_void,
    data_size: c_int,
) -> c_int {
    let buf = unsafe { std::slice::from_raw_parts(data_v as *const u8, data_size as usize) };
    match KBLE.tlm_tx.try_send(buf.to_vec()) {
        Ok(_) => CCSDS_ERR_CODE_CCSDS_ERR_OK.0,
        Err(_) => CCSDS_ERR_CODE_CCSDS_ERR_TX_INVALID.0,
    }
}

#[no_mangle]
pub extern "C" fn CCSDS_reopen(_my_ccsds_v: *mut c_void, _reson: c_int) -> c_int {
    CCSDS_ERR_CODE_CCSDS_ERR_OK.0
}
