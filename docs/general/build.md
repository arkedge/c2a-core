# Build

## 概要
ビルドに関連することをまとめる (TBW)．



## Build Options
C2A User から利用するビルドオプションは， [`/CMakeLists.txt`](/CMakeLists.txt) で定義される．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt



## [`/CMakeLists.txt`](/CMakeLists.txt)
### コンパイルオプション
TBA

### C2A Core 機能
#### `C2A_USE_ALL_CORE_APPS`
`ON` にすることで，全ての [Core Application](/applications/) をビルドする．  
その場合， `/src_user/applications/CMakeLists.txt` にビルドしたい Core Application を適宜追加すること．

https://github.com/arkedge/c2a-core/blob/459dbbf84d9587730f248ccbadb5a5745b28903d/examples/subobc/src/src_user/applications/CMakeLists.txt#L9-L13


#### `C2A_USE_ALL_CORE_TEST_APPS`
`ON` にすることで，全ての [Core Test Application](/applications/test_app/) をビルドする．


#### `C2A_USE_ALL_CORE_LIB`
`ON` にすることで，全ての [Core Library](/library/) をビルドする．


#### `C2A_MOBC_FEATURES`
`ON` にすることで，MOBC 向けの機能を有効化する．デフォルトは `OFF` であり， sub OBC 向けの実装となる．




### その他
#### `C2A_USE_STDINT_WRAPPER`
C89 ターゲットでビルドする際に `ON` にすることで，コンパイラが提供するものではなく，ユーザ定義の `stdint.h` を使う．  
その場合， `src_user/library/stdint_impl.h` にユーザー定義の `stdint.h` を配置すること．
