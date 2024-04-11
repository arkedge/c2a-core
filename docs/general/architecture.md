# Architecture

## 概要
C2A が想定する，アーキテクチャについてまとめる (TBW)．


## OBC
通信機と接続され衛星の全体を司る MOBC (Main On-Board Computer) と，MOBC の配下に接続される sub OBC (AOCS OBC など) が存在する．
そして，コンポーネントの中心に MOBC があり， sub OBC を含む各コンポーネントは MOBC と接続される．

デフォルトは sub OBC 向けの実装となっており， MOBC を開発するときは，`C2A_MOBC_FEATURES` を `ON` にし有効化する．
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L24
