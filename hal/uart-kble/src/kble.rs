use std::{net::SocketAddr, pin::pin, sync::Arc, thread};

use anyhow::Result;
use axum::{
    extract::{ws::WebSocket, Path, State, WebSocketUpgrade},
    http::StatusCode,
    response::Response,
    routing::get,
    Router,
};
use futures::{SinkExt, StreamExt};
use kble_socket::from_axum;
use tokio::sync::OwnedMutexGuard;

use crate::{Mux, OuterChannel};

pub struct Server {
    mux: Arc<Mux>,
}

impl Server {
    pub fn new(mux: Arc<Mux>) -> Self {
        Self { mux }
    }

    pub async fn serve(self, addr: SocketAddr) -> Result<()> {
        let app = Router::new()
            .route("/channels/:ch", get(handle_channel))
            .with_state(self.mux);
        axum::Server::bind(&addr)
            .serve(app.into_make_service())
            .await?;
        Ok(())
    }

    pub fn serve_in_background(self, addr: SocketAddr) {
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

async fn handle_channel(
    upgrade: WebSocketUpgrade,
    State(mux): State<Arc<Mux>>,
    Path(ch): Path<u8>,
) -> Result<Response, StatusCode> {
    let Some(channel) = mux.try_get_outer(ch) else {
        return Err(StatusCode::CONFLICT);
    };
    Ok(upgrade.on_upgrade(|ws| handle_ws(ws, channel)))
}

async fn handle_ws(ws: WebSocket, channel: OwnedMutexGuard<OuterChannel>) {
    let (mut sink, mut stream) = from_axum(ws);
    let tx = channel.tx.clone();
    let rx = channel.rx.clone();
    let tx_fut = async {
        loop {
            let mut buf = vec![0u8; 2048];
            let len = tx.read(&mut buf).await;
            buf.truncate(len);
            sink.send(buf.into()).await?;
        }
        #[allow(unreachable_code)]
        anyhow::Ok(())
    };
    let rx_fut = async {
        loop {
            let Some(chunk) = stream.next().await else {
                break;
            };
            rx.write(&chunk?).await;
        }
        anyhow::Ok(())
    };
    // どちらか一方が終了したら channel guard を解放して戻る。try_join だと
    // クライアント切断で rx 側が正常終了しても、tx 側が次の下り送出を待って
    // 永久に完了せず guard を掴み続け、再接続が 409 Conflict になり続ける。
    let tx_fut = pin!(tx_fut);
    let rx_fut = pin!(rx_fut);
    futures::future::select(tx_fut, rx_fut).await;
}
