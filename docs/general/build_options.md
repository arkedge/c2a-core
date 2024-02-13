# Build Options

## 概要
主なビルドオプション (Cmake Option) について説明する．  
なお，ここに含まれているオプションは利用可能なオプションのうち，ごく一部であることに注意すること．


## [`/CMakeLists.txt`](/CMakeLists.txt)
### コンパイルオプション
TBA

### C2A Core 機能
#### `C2A_USE_ALL_CORE_APPS`
`ON` にすることで，全ての Core Application をビルドする．  
その場合， `/src_user/applications/CMakeLists.txt` にビルドしたい Core Application を適宜追加すること．

https://github.com/arkedge/c2a-core/blob/459dbbf84d9587730f248ccbadb5a5745b28903d/examples/subobc/src/src_user/applications/CMakeLists.txt#L9-L13


### その他
#### `C2A_USE_STDINT_WRAPPER`
C89 ターゲットでビルドする際に `ON` にすることで，コンパイラが提供するものではなく，ユーザ定義の `stdint.h` を使う．  
その場合， `src_user/library/stdint_impl.h` にユーザー定義の `stdint.h` を配置すること．
