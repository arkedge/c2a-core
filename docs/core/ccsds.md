# CCSDS

## 概要
C2A Core に実装された CCSDS (Consultative Committee for Space Data Systems) によって規定される宇宙データシステムの標準規格  Space Communications Protocols についてまとめる．
CCSDS 技術仕様文書は [ブルーブック（Blue Books）：全85文書](https://stage.tksc.jaxa.jp/ccsds/docs/doc_blue.html) を参照すること．


## Space Packet Protocol
[`tlm_cmd/ccsds/space_packet_protocol`](/tlm_cmd/ccsds/space_packet_protocol/) に配置される．

[Core/Communication.md#c2a-標準-space-packet-定義](./communication.md#c2a-標準-space-packet-定義) などを参照のこと．


## AOS Space Data Link Protocol / TC Space Data Link Protocol
[`tlm_cmd/ccsds/aos_space_data_link_protocol`](/tlm_cmd/ccsds/aos_space_data_link_protocol/), [`tlm_cmd/ccsds/tc_space_data_link_protocol`](/tlm_cmd/ccsds/tc_space_data_link_protocol/) に配置される．

基本的には，地上局と通信する無線機 (STX や SRX) に接続される OBC (MOBC など) 向けのコードとなる．

AOS Space Data Link Protocol や TC Space Data Link Protocol は，様々な設計パラメタがあるが，ここでのコードは，ある特定の設計パラメタにおける実装であることに注意すること．
したがって，現時点では汎用性の低いコードが配置されている．
（今後，適切にリファクタリングする予定．）

また，本コードをコンパイル対象に加えると，少なくない RAM を static に確保してしまうため，デフォルトではコンパイル対象には含まれない．


## ビルドオプション
AOS Space Data Link Protocol と TC Space Data Link Protocol の各実装をコンパイル対象に含める場合は，以下の `C2A_USE_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL`, `C2A_USE_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL` オプションをそれぞれ `ON` に設定すること．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L26-L29
