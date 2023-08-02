mod buffer;
mod kble;

use std::env;
use std::ffi::c_int;
use std::net::{Ipv4Addr, SocketAddr};
use std::{collections::HashMap, sync::Arc};

use c2a_core::hal::uart::{UART_Config, UART_ERR_CODE, UART_ERR_CODE_UART_OK};
use kble::Server;
use once_cell::sync::Lazy;
use tokio::sync::{Mutex, OwnedMutexGuard, RwLock};

use buffer::Buffer;

pub struct OuterChannel {
    pub tx: Arc<Buffer>,
    pub rx: Arc<Buffer>,
}

struct InnerChannel {
    tx: Arc<Buffer>,
    rx: Arc<Buffer>,
}

struct ChannelPair {
    inner: InnerChannel,
    outer: Arc<Mutex<OuterChannel>>,
}

impl ChannelPair {
    fn with_capacity(capacity: usize) -> Self {
        let tx = Arc::new(Buffer::with_capacity(capacity));
        let rx = Arc::new(Buffer::with_capacity(capacity));
        let inner = InnerChannel {
            tx: tx.clone(),
            rx: rx.clone(),
        };
        let outer = OuterChannel { tx, rx };

        Self {
            inner,
            outer: Arc::new(Mutex::new(outer)),
        }
    }

    fn reinitialize(&mut self, capacity: usize) {
        self.inner.tx.reinitialize(capacity);
        self.inner.rx.reinitialize(capacity);
    }
}

#[derive(Default)]
pub struct Mux {
    channels: RwLock<HashMap<u8, ChannelPair>>,
}

impl Mux {
    fn init_channel(&self, ch: u8) {
        const BUFFER_SIZE: usize = 2048; // FIXME: make configurable
        let mut channels = self.channels.blocking_write();
        channels
            .entry(ch)
            .and_modify(|channel| channel.reinitialize(BUFFER_SIZE))
            .or_insert_with(|| ChannelPair::with_capacity(BUFFER_SIZE));
    }

    fn read(&self, ch: u8, buf: &mut [u8]) -> usize {
        let channels = self.channels.blocking_read();
        let Some(pair) = channels.get(&ch) else {
            // TODO: return propery error
            return 0;
        };
        pair.inner.rx.nonblocking_read(buf)
    }

    fn write(&self, ch: u8, data: &[u8]) {
        let channels = self.channels.blocking_read();
        let Some(pair) = channels.get(&ch) else {
            return;
        };
        pair.inner.tx.blocking_write(data)
    }

    pub fn try_get_outer(&self, ch: u8) -> Option<OwnedMutexGuard<OuterChannel>> {
        let channels = self.channels.try_read().ok()?;
        let Some(pair) = channels.get(&ch) else {
            return None;
        };
        pair.outer.clone().try_lock_owned().ok()
    }
}

static MUX: Lazy<Arc<Mux>> = Lazy::new(|| {
    let mux = Arc::new(Mux::default());
    let port = env::var("UART_KBLE_PORT").unwrap_or_else(|_| "9696".to_string());
    let port = port.parse().unwrap();
    Server::new(mux.clone())
        .serve_in_background(SocketAddr::new(Ipv4Addr::UNSPECIFIED.into(), port));
    mux
});

#[no_mangle]
#[allow(clippy::missing_safety_doc)]
pub unsafe extern "C" fn UART_init(uart_config: *mut UART_Config) -> c_int {
    let ch = unsafe { (*uart_config).ch };
    MUX.init_channel(ch);
    UART_ERR_CODE_UART_OK.0
}

#[no_mangle]
#[allow(clippy::missing_safety_doc)]
pub unsafe extern "C" fn UART_rx(
    uart_config: *mut UART_Config,
    buf: *mut u8,
    buf_size: c_int,
) -> c_int {
    let ch = unsafe { (*uart_config).ch };
    let buf = unsafe { std::slice::from_raw_parts_mut(buf, buf_size as usize) };
    let ret = MUX.read(ch, buf);
    UART_ERR_CODE(ret as i32).0
}

#[no_mangle]
#[allow(clippy::missing_safety_doc)]
pub unsafe extern "C" fn UART_tx(
    uart_config: *mut UART_Config,
    data: *mut u8,
    data_size: c_int,
) -> c_int {
    let ch = unsafe { (*uart_config).ch };
    let data = unsafe { std::slice::from_raw_parts_mut(data, data_size as usize) };
    MUX.write(ch, data);
    UART_ERR_CODE_UART_OK.0
}

#[no_mangle]
pub extern "C" fn UART_reopen(_uart_config: *mut UART_Config, _reason: c_int) -> c_int {
    UART_ERR_CODE_UART_OK.0
}
