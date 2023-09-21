# Release

## リリース
現在， `main` ブランチがリリースブランチとなっている．  
リリースには，本 Release と Pre-release の２つを設ける．

- 本 Release
    - まとまった機能更新のリリース．
    - バージョンを上げる．
    - `v3.4.0` → `v3.5.0` など
- Pre-release
    - [Tools/Overview](../tools/overview.md) などに対して非互換なアップデートが入った場合や，Bug fix や 大きな機能更新などで，本 Release 前に User サイドで最新の Core が必要になることが予想される際のリリース．
    - `beta` をリリースする．
        - `v3.4.0` → `v3.5.0-beta.0` など
    - なお，メジャーバージョンアップ中では様々な破壊的な変更が連続するため，煩雑さ低減のためにツール等の互換性維持のための Pre-release は，一定程度免除することが可能である．

リリースの手順は以下のようにする．

### 本 Release

1. バージョン番号をインクリメントする PR (Pull Request) を発行し，`develop` ブランチへ merge する．
    - [c2a_core_main.h](https://github.com/arkedge/c2a-core/blob/develop/c2a_core_main.h) 内の `C2A_CORE_VER_*` をインクリメントする．
    - [Cargo.toml](https://github.com/arkedge/c2a-core/blob/develop/Cargo.toml) 内の `workspace.package.version` をインクリメントする．
    - この後リリースを控えるので，念の為すべてのテストを再度回す．
    - `#define C2A_CORE_VER_PRE` は `("")` とする．
    - PR タイトルは `Update version (v3.4.0)` のようにする．
    - 例: https://github.com/ut-issl/c2a-core/pulls?q=is%3Apr+Update+version
1. バージョン上げ PR が merge されたら，直ちに（他の PR を止め） `develop` から `main` に PR を発行し，すべてのテストを回し， merge する．
    - PR タイトルは以下のようにする．
        - `Update main (v3.4.0) on 2021-12-31`
        - `Update main (v3.4.0) on 2021-12-31 - ほげほげ`
    - PR のディスクリプションは [これ](https://github.com/ut-issl/c2a-core/pull/151) のように書く．
    - 例: https://github.com/ut-issl/c2a-core/pulls?q=is%3Apr+Update+main
1. [tag](https://github.com/ut-issl/c2a-core/tags) を打ち， [release](https://github.com/ut-issl/c2a-core/releases) を発行する．
    - tag 名は `v3.4.0` のようにする．
    - release 名は `v3.4.0` や `v3.3.1 初版` のようにする．
    - release には以下を含める．
        - Release Note として簡潔な更新差分の箇条書き
        - `main` に merge したときの PR のリンク
1. `cargo publish` する．

これを，だいたい以下のような粒度で行う．

- 最後のリリースからおおよそ１ヶ月程度が経過した場合（更新が少ない場合を除く）
- まとまった機能実装が一段落した場合

ただし，あるまとまった機能の実装の PR が複数に分割され，一部がまだ merge されていない場合はリリースを控えること．

### Pre-release
1. ある `feature` ブランチから `develop` ブランチへの PR が発行されたとき，それが Pre-release に相当する場合，その PR は Pre-release PR と呼び，次のような手順を踏む．
    - PR タイトルは以下のようにする．
        - `Pre Release (v3.5.0-beta.0): 通常のPRのタイトル`
    - 対応する Tools の PR のリンクを貼る．
    - `#define C2A_CORE_VER_PRE` に `("beta.0")` などをセットする．
    - 本 Release 後最初の Pre-release の場合， `C2A_CORE_VER_*` をインクリメントする．
    - [Cargo.toml](https://github.com/arkedge/c2a-core/blob/develop/Cargo.toml) 内の `package.version` を同様にインクリメントする．
    - 例: https://github.com/ut-issl/c2a-core/pulls?q=is%3Apr+Pre+Release
1. [tag](https://github.com/ut-issl/c2a-core/tags) を打ち， [release](https://github.com/ut-issl/c2a-core/releases) を Pre-release として発行する．
    - Pre-release PR を merge したその merge commit（`develop` ブランチ上）で tag を打つ．
    - tag 名は `v3.5.0-beta.0` のようにする．
    - release 名は `v3.5.0-beta.0` のようにする．
    - release には以下を含める．
        - 非互換となった Tools の新しい バージョン (Release) へのリンク
        - `develop` に merge したときの PR のリンク
1. `cargo publish` する．


## バージョニング
[Semantic Versioning](https://semver.org) を採用する．


## C2A Tools との互換性について
少なくとも， [Tools/Overview](../tools/overview.md) の Tool に後方互換性のない更新があった場合は，新しいバージョンをリリースする．  
Tool のリリースには，以下に注意する．

- C2A と同様の規則で Relase を発行する．
- release には以下を含める．
    - Release Note として簡潔な更新差分の箇条書き
    - 対応する最小 C2A Core バージョン
    - この Tool に適合させたときの C2A Core の PR へのリンク

例:

- https://github.com/ut-issl/c2a-tlm-cmd-code-generator/releases
- https://github.com/ut-issl/python-wings-interface/releases
- https://github.com/ut-issl/tlm-cmd-db/releases
