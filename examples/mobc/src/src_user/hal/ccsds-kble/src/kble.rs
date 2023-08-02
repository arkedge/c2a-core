use std::thread;

use anyhow::{anyhow, Result};
use futures::{future, SinkExt, TryStreamExt};
use tokio::{
    net::{TcpListener, ToSocketAddrs},
    sync::mpsc,
};

pub fn new() -> (mpsc::Sender<Vec<u8>>, mpsc::Receiver<Vec<u8>>, Socket) {
    let (tlm_tx, tlm_rx) = mpsc::channel(5);
    let (cmd_tx, cmd_rx) = mpsc::channel(5);
    let socket = Socket { tlm_rx, cmd_tx };
    (tlm_tx, cmd_rx, socket)
}

pub struct Socket {
    tlm_rx: mpsc::Receiver<Vec<u8>>,
    cmd_tx: mpsc::Sender<Vec<u8>>,
}

impl Socket {
    pub async fn serve(mut self, addr: impl ToSocketAddrs) -> Result<()> {
        let listener = TcpListener::bind(addr).await?;
        loop {
            let (incoming, _addr) = listener.accept().await?;
            let wss = tokio_tungstenite::accept_async(incoming).await?;
            let (mut sink, mut stream) = kble_socket::from_tungstenite(wss);
            let uplink = async {
                loop {
                    let tlm_bytes = self
                        .tlm_rx
                        .recv()
                        .await
                        .ok_or_else(|| anyhow!("telemetry producer has gone"))?;
                    sink.send(tlm_bytes.into()).await?;
                }
            };
            let downlink = async {
                loop {
                    let Some(cmd_bytes) = stream.try_next().await? else {
                        break;
                    };
                    self.cmd_tx.send(cmd_bytes.into()).await?;
                }
                anyhow::Ok(())
            };
            let _: Option<((), ())> = future::try_join(uplink, downlink).await.ok();
        }
    }

    pub fn serve_in_background(self, addr: impl ToSocketAddrs + Send + 'static) {
        let rt = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();
        thread::spawn(move || {
            let fut = self.serve(addr);
            if let Err(e) = rt.block_on(fut) {
                eprintln!("kble server has exited: {e}");
            }
        });
    }
}
