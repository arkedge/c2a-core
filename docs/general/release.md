# Release

デフォルトのリリースブランチは `main` とする．

また，主な開発とは別に，既存のリリースに対するメンテナンスのためのリリースを行う際は `stable-4.1` のようなブランチを切ってリリースブランチとする．

互換性の維持は基本的には（Pre Release ではない）本 Release を対象に行う．
ただし，[Tools](../tools/overview.md) などの外部プロジェクトに対して非互換となるような大きな変更を入れる際には，その変更をマージした時点で Pre Release を行うものとする．

これは [Tools](../tools/overview.md) などのテストを迅速に行ったり，（非推奨ではあるが）Pre Release なバージョンの c2a-core を使っている C2A user で使用する [Tools](../tools/overview.md) などのバージョンを明らかにするための処置となる．
そのため，破壊的な変更が多く発生することが明らかな major update 中には，煩雑さ軽減のためこの処置は一定程度免除してもよいものとする．

## Release 手順

1. バージョン番号をインクリメントする
   - [c2a_core_main.h](https://github.com/arkedge/c2a-core/blob/main/c2a_core_main.h) 内の `C2A_CORE_VER_*` をインクリメントする．
     - Pre Release でない場合，`#define C2A_CORE_VER_PRE` は `("")` とする．
     - Pre Release のときは `#define C2A_CORE_VER_PRE` に `("beta.0")` などをセットする．
   - [Cargo.toml](https://github.com/arkedge/c2a-core/blob/main/Cargo.toml) 内の `workspace.package.version` をインクリメントする．
1. リリースのための PR (Pull Request) をリリースブランチに発行する
   - この後リリースを控えるので，念の為すべてのテストを再度回す．
   - PR タイトルは `Update version (v3.4.0)` のようにする．
   - PR のディスクリプションは [これ](https://github.com/ut-issl/c2a-core/pull/151) のように書く．
   - 例: https://github.com/arkedge/c2a-core/pulls?q=is%3Apr+Update+version
1. リリースのための PR を merge して，その merge commit に [Git tag](https://github.com/arkedge/c2a-core/tags) を打つ
   - tag 名は `v3.4.0` のようにする．
   - Pre Release のときは `v3.5.0-beta.0` のようにする．
1. `cargo publish` する．
1. [GitHub Release](https://github.com/ut-issl/c2a-core/releases) を発行する．
   - Release Title は `v3.4.0` のようにする．
   - Release Note には以下を含める．
     - 簡潔な更新差分の箇条書き
     - `main` に merge したときの PR のリンク
     - サポートする Tools のバージョン


## バージョニング
[Semantic Versioning](https://semver.org) を採用する．

ただし，c2a-core における Public API は定義中であり，C言語のフレームワークであるという特性上あらゆる変更が容易に破壊的な変更となってしまうため，minor update でもユーザ側対応は必須となることがほとんどであることには注意すること．


## C2A Tools のリリース
Tool のリリースには，以下に注意する．

- c2a-core と同様の規則で Relase を発行する．
- release には以下を含める．
  - Release Note として簡潔な更新差分の箇条書き
  - 対応する最小 C2A Core バージョン
  - この Tool に適合させたときの C2A Core の PR へのリンク

例:

- https://github.com/ut-issl/python-wings-interface/releases
- https://github.com/ut-issl/tlm-cmd-db/releases
