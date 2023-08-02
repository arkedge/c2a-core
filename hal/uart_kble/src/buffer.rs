use std::{
    collections::VecDeque,
    ops::{Deref, DerefMut},
};

use tokio::sync::{Mutex, Notify};

pub struct Buffer {
    deque: Mutex<VecDeque<u8>>,
    notify: Notify,
}

fn write_internal<D>(mut deque: D, data: &[u8])
where
    D: Deref<Target = VecDeque<u8>> + DerefMut,
{
    let data = if deque.capacity() < data.len() {
        &data[data.len() - deque.capacity()..]
    } else {
        data
    };
    if deque.capacity() < deque.len() + data.len() {
        let overflow = deque.len() + data.len() - deque.capacity();
        deque.drain(..overflow);
    }
    deque.extend(data);
}
fn read_internal<D>(mut deque: D, buf: &mut [u8]) -> usize
where
    D: Deref<Target = VecDeque<u8>> + DerefMut,
{
    let read = deque.len().min(buf.len());
    for (s, d) in deque.drain(..).zip(buf.iter_mut()) {
        *d = s;
    }
    read
}
impl Buffer {
    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            deque: Mutex::new(VecDeque::with_capacity(capacity)),
            notify: Notify::new(),
        }
    }

    pub fn reinitialize(&self, capacity: usize) {
        let mut deque = self.deque.blocking_lock();
        *deque = VecDeque::with_capacity(capacity);
    }

    pub fn blocking_write(&self, data: &[u8]) {
        let deque = self.deque.blocking_lock();
        write_internal(deque, data);
        self.notify.notify_waiters();
    }

    pub async fn write(&self, data: &[u8]) {
        let deque = self.deque.lock().await;
        write_internal(deque, data);
        self.notify.notify_waiters();
    }

    pub fn nonblocking_read(&self, buf: &mut [u8]) -> usize {
        let deque = self.deque.blocking_lock();
        read_internal(deque, buf)
    }

    pub async fn read(&self, buf: &mut [u8]) -> usize {
        loop {
            let deque = self.deque.lock().await;
            if deque.is_empty() {
                drop(deque);
                self.notify.notified().await;
                continue;
            }
            return read_internal(deque, buf);
        }
    }
}
