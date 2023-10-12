# CHANGELOG

注意: これは既存の C2A core update の「リリースの間の Pull Request を眺めてなんとなく察する」という曖昧な操作を緩和していくための試みであり，C2A user に対するお知らせを行う場として使っていくことを意図しています．初めから c2a-core の全変更を取り扱うと不必要に煩雑になるだけになってしまうため，完全な変更内容の一覧についてはこれまで通り [GitHub Releases](https://github.com/arkedge/c2a-core/releases) などから参照してください．

## v4.0.0 (Unreleased)

### Breaking Changes

- [ut-issl/c2a-core](https://github.com/ut-issl/c2a-core) [v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0) から ArkEdge Space Inc. による先行開発のために fork し，リポジトリを [arkedge/c2a-core](https://github.com/arkedge/c2a-core) に移行
  - ライセンス情報の更新: [#1](https://github.com/arkedge/c2a-core/pull/1)
- 各ディレクトリ名・構造の変更
  - example user を `minimum_user` -> `mobc`, `2nd_obc_user` -> `subobc` に変更: [#15](https://github.com/arkedge/c2a-core/pull/15)
  - ディレクトリ名のコーディング規約を `CamelCase` から `snake_case` に変更: [#20](https://github.com/arkedge/c2a-core/pull/20)
    - v4 系統ではこの変更は移行期間とし，あらゆるディレクトリ名を一気に変更する必要はないものとしている
    - Rust の crate のためのディレクトリは小文字の `-` 区切りとする: [#42](https://github.com/arkedge/c2a-core/pull/42)
  - C2A 用語の rename: [#8](https://github.com/arkedge/c2a-core/issues/8)
    - `IfWrapper` -> `HAL` (Hardware Abstraction Layer): [#21](https://github.com/arkedge/c2a-core/pull/21), [#24](https://github.com/arkedge/c2a-core/pull/24)
  - 歪なディレクトリ構造の修正: [#28](https://github.com/arkedge/c2a-core/issues/28)
  - 移行には [scripts/migration/](./scripts/migration/) 以下の `v4-rename-*.sh` の migration script を使用可能
    - example user についてはこの migration script のみで移行しているが，あらゆる C2A user の状況について考慮できているわけではないため，適宜対応が必要
- `hal`（旧 `IfWrapper`）のうち，古すぎるインターフェースを排除: [#23](https://github.com/arkedge/c2a-core/issues/23), [#25](https://github.com/arkedge/c2a-core/pull/25), [#129](https://github.com/arkedge/c2a-core/pull/129), [#130](https://github.com/arkedge/c2a-core/pull/130), [#131](https://github.com/arkedge/c2a-core/pull/131)
  - 既に使われてしまっているものについては，各 C2A user で一旦雑に古いヘッダファイルを各々で持って対応可能（今後再設計する予定）
- `check_coding_rule.py` の設定ファイルである `check_coding_rule.json` の場所を C2A user のトップディレクトリに変更: [#97](https://github.com/arkedge/c2a-core/pull/97)
  - 対応方法
    - `Script/CI/check_coding_rule.json` ないし `script/ci/check_coding_rule.json` を C2A user のトップディレクトリに移動
    - 設定ファイルから `c2a_root_dir` を削除
    - 設定ファイル中のディレクトリの設定を C2A user のトップディレクトリからの相対パスに変更
- [ut-issl/c2a-enum-loader](https://github.com/ut-issl/c2a-enum-loader) を c2a-core リポジトリで管理するように変更: [#99](https://github.com/arkedge/c2a-core/pull/99)
  - import したバージョン: [ut-issl/c2a-enum-loader ae-v2.0.0](https://github.com/ut-issl/c2a-enum-loader/releases/tag/ae-v2.0.0)
- [ut-issl/c2a-tlm-cmd-code-generator](https://github.com/ut-issl/c2a-tlm-cmd-code-generator) を c2a-core リポジトリで管理するように変更: [#111](https://github.com/arkedge/c2a-core/pull/111)
  - import したバージョン: [ut-issl/c2a-tlm-cmd-code-generator ae-v2.0.0](https://github.com/ut-issl/c2a-tlm-cmd-code-generator/releases/tag/ae-v2.0.0)
- CMake option の整理: [#86](https://github.com/arkedge/c2a-core/pull/86)
  - `C2A_` prefix に統一した（これはコーディング規約にも追加）
  - 意味が分かりにくい命名の変更，今後 optional としていく挙動を default OFF とした
  - `option()` の挙動はユーザ指定によってかなり変わるため，該当する変更は単なるビルドチェックなどではなくすべて grep して変更すること
  - `BUILD_C2A_AS_UTF8` -> `C2A_BUILD_AS_UTF8`
  - `BUILD_C2A_AS_C99` -> `C2A_BUILD_AS_C99`
  - `BUILD_C2A_AS_CXX` -> `C2A_BUILD_AS_CXX`
  - `(NOT USE_32BIT_COMPILER)` -> `C2A_BUILD_FOR_32BIT`: `ON` の時に明示的に 32bit ターゲットとしてビルドする（`-m32` をつける）
  - `(NOT C2A_USE_STDINT_WRAPPER)` -> `C2A_USE_STDINT_WRAPPER`: C89 ターゲットでビルドする際に `ON` にすることでユーザ定義の `stdint.h` を使う
  - `BUILD_C2A_AS_SILS_FW` -> `C2A_BUILD_FOR_SILS`
  - `USE_ALL_C2A_CORE_APPS` -> `C2A_USE_ALL_CORE_APPS`
  - `USE_ALL_C2A_CORE_TEST_APPS` -> `C2A_USE_ALL_CORE_TEST_APPS`
  - `USE_ALL_C2A_CORE_LIB` -> `C2A_USE_ALL_CORE_LIB`


### Enhancements

- `c2a-core` crate で Rust 向けの binding を提供する :[#35](https://github.com/arkedge/c2a-core/pull/35), [#37](https://github.com/arkedge/c2a-core/pull/37)
  - C2A 内で共通の [bindgen](https://github.com/rust-lang/rust-bindgen) の設定を使用可能にするため，[c2a-bind-utils](https://crates.io/crates/c2a-bind-utils) crate を追加
- C2A 単体での SILS のための C2A のランタイム実装である `c2a-sils-runtime` crate を追加
- 共通して使用可能な C2A HAL（旧 `IfWrapper`）実装群を追加
  - C2A HAL 実装のテンプレートや，モックとして使用可能な noop 実装を追加: [#40](https://github.com/arkedge/c2a-core/pull/40)
  - [kble](https://github.com/arkedge/kble) を使った SILS 向け UART 実装を追加: [#41](https://github.com/arkedge/c2a-core/pull/41)
- `c2a-sils-runtime` と Rust 製の C2A HAL 実装を使って，example user の C2A 単体での SILS（S2E なし）を実装: [#44](https://github.com/arkedge/c2a-core/pull/44), [#45](https://github.com/arkedge/c2a-core/pull/45)
- 新たな C2A 標準環境である `C2A Boom` を example user に導入: [#56](https://github.com/arkedge/c2a-core/pull/56)
  - これに伴い，各種 ArkEdge 独自のライブラリ・ツール群が 導入・OSS 化された
  - [Gaia](https://github.com/arkedge/gaia)
  - [tlmcmddb-cli](https://github.com/arkedge/c2a-tlmcmddb)
  - [kble](https://github.com/arkedge/kble)
  - [C2A DevTools](https://github.com/arkedge/c2a-devtools)
- example user に対して，Gaia を用いた pytest を導入: [#59](https://github.com/arkedge/c2a-core/pull/59)
  - [python-wings-interface](https://github.com/ut-issl/python-wings-interface) 互換の Gaia（`tmtc-c2a`）向けインターフェースとして [c2a-pytest-gaia](https://github.com/arkedge/c2a-pytest-gaia) を導入
  - これに伴い，Python の仮想環境・ツールチェーン管理に [rye](https://rye-up.com/)

### Fixed

- `c2a-core` crate のためのソースファイルを `src/lib.rs` から `c2a_core.rs` に移動し，紛らわしい `src` ディレクトリを削除: [#30](https://github.com/arkedge/c2a-core/pull/30)

### Compatible C2A Tools

- [ut-issl/tlm-cmd-db v2.4.0](https://github.com/ut-issl/tlm-cmd-db/releases/tag/v2.4.0)
- [ut-issl/python-wings-interface v1.5.1](https://github.com/ut-issl/python-wings-interface/releases/tag/v1.5.1)
- [arkedge/gaia v0.5.0](https://github.com/arkedge/gaia/releases/tag/v0.5.0)
- [tlmcmddb-cli 0.2.0](https://crates.io/crates/tlmcmddb-cli/0.2.0)
- [kble 0.2.0](https://crates.io/crates/kble/0.2.0)
- [C2A DevTools](https://github.com/arkedge/c2a-devtools)


## Previous Releases

- [v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0)
- [v3.9.2](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.2)
- [v3.9.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.1)
- [v3.9.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.0)
- [v3.8.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.8.0)
- [v3.7.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.7.0)
- [v3.6.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.6.0)
- [v3.5.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.5.0)
- [v3.4.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.4.0)
- [v3.1.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.3.1)
