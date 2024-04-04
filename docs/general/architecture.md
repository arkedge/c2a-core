# Architecture

## 概要
C2A が想定する，アーキテクチャについてまとめる (TBW)．


## OBC
想定するアーキテクチャでは，通信機と接続され衛星の全体を司る MOBC (Main On-Board Computer) と，MOBC の配下に接続される sub OBC (AOCS OBC など) のそれぞれ向けの機能が存在する．
そして，コンポーネントの中心に MOBC があり， sub OBC を含む各コンポーネントは MOBC と接続される．

`C2A_MOBC_FEATURES` を `ON` にすることで，MOBC 向けの機能を有効化する．
デフォルトは `OFF` であり， sub OBC 向けの実装となる．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L24
