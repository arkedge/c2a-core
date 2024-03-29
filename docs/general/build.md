# Build

## 概要
ビルドに関連することをまとめる (TBW)．


## Build Options
C2A User から利用するビルドオプションは， [`/CMakeLists.txt`](/CMakeLists.txt) で定義される．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L5-L31


### ターゲット OBC
c2a-core には，通信機と接続され衛星の全体を司る MOBC (Main On-Board Computer) と，MOBC の配下に接続される sub OBC (AOCS OBC など) のそれぞれ向けの機能が存在する．
`C2A_MOBC_FEATURES` を `ON` にすることで，MOBC 向けの機能を有効化する．デフォルトは `OFF` であり， sub OBC 向けの実装となる．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L24


### `stdint.h`
C89 ターゲットでビルドする際に `C2A_USE_STDINT_WRAPPER` を `ON` にすることで，コンパイラが提供するものではなく，ユーザ定義の `stdint.h` を使うことができる．  
その場合， `src_user/library/stdint_impl.h` にユーザー定義の `stdint.h` を配置すること．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L14
