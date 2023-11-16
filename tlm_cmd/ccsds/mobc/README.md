# CCSDS / MOBC

本ディレクトリに配置されているコードは，地上局と通信する無線機 (STX や SRX) に接続される OBC (MOBC など) が，CCSDS Space Communications Protocols における，TC Space Data Link Protocol と AOS Space Data Link Protocol に関連するコードである．

CSDS Space Communications Protocols は，様々な設計パラメタがあるが，ここでのコードは，ある特定の設計パラメタにおける実装であることに注意すること．
したがって，現時点では汎用性の低いコードが配置されている．
（今後，適切にリファクタリンスする予定．）

また，本コードをコンパイル対象に加えると，少なくない RAM を static に確保してしまうため，デフォルトではコンパイル対象には含まれない．
コンパイル対象に含める場合は， FIXME: で設定すること．
