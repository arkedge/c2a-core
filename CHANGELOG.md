# CHANGELOG

注意: これは既存の C2A core update の「リリースの間の Pull Request を眺めてなんとなく察する」という曖昧な操作を緩和していくための試みであり，C2A user に対するお知らせを行う場として使っていくことを意図しています．初めから c2a-core の全変更を取り扱うと不必要に煩雑になるだけになってしまうため，完全な変更内容の一覧についてはこれまで通り [GitHub Releases](https://github.com/arkedge/c2a-core/releases) などから参照してください．

## v4.0.0 (Unreleased)

c2a-core v4.0.0 は ArkEdge Space Inc. による C2A の先行開発のための c2a-core の fork 後最初のリリースとなる．
v4.0.0 では，fork に伴う開発体制の変更に加え，以下の大きなセマンティクス的な変更を行ったため，major update とした．

- 大規模な rename
- 各ツールの c2a-core リポジトリへの吸収合併
- [Rust](https://www.rust-lang.org/) のエコシステム / C2A 標準開発環境 C2A Boom の導入

大規模な rename はあくまで現在の設計・コードの責務をできるだけ明らかにし，（特に C2A の開発経験の無い一般的なソフトウェアエンジニアを中心とした）開発者のディスコミュニケーションを低減する目的で実施した．
また，ディレクトリ名に大文字・小文字があったりなかったりする，不必要に階層構造が深いといった歪なディレクトリ構造の修正も行った．

ex: `IfWrapper` -> `HAL`, `Driver` -> `Component Driver`, `src/src_user/Settings/TlmCmd/data_base` -> `tlm-cmd-db`

これらの rename は大規模ではあるものの，C2A としてのロジックの変更やリファクタリングはほぼ行っていない．
この移行については migration script を用意している（Migration Guide 参照）．

各ツールの c2a-core リポジトリへの吸収合併については，enum-loader や tlm-cmd-code-generator などの c2a-core のコード構成に依存したツールのリリース単位は c2a-core と同一であると判断し実施した．
これらのツールについては，今後は各 C2A user で使用している c2a-core と同じバージョンのものを使用してもらうことになる（実務上は `src/src_core` から使うことになる）．

Rust エコシステム / C2A Boom の導入については，ArkEdge Space Inc. 内での開発成果を OSS 化しつつ実施した．
これらについては c2a-core v3 系に対しても互換性があり導入可能だが，新機能の開発や積極的なサポートは c2a-core v4 系以降を前提として行われる可能性がある．

### Breaking Changes

- [ut-issl/c2a-core](https://github.com/ut-issl/c2a-core) [v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0) から ArkEdge Space Inc. による先行開発のために fork し，リポジトリを [arkedge/c2a-core](https://github.com/arkedge/c2a-core) に移行
  - [#1](https://github.com/arkedge/c2a-core/pull/1): ライセンス情報の更新
- [#18](https://github.com/arkedge/c2a-core/pull/18): CommandDispatcher の統一的なテレメの提供
-  [#19](https://github.com/arkedge/c2a-core/pull/19): App の initializer, entry_point の返り値を void から RESULT にする
- 各ディレクトリ名・構造の変更
  - [#15](https://github.com/arkedge/c2a-core/pull/15): example user を `minimum_user` -> `mobc`, `2nd_obc_user` -> `subobc` に変更
  - [#20](https://github.com/arkedge/c2a-core/pull/20): ディレクトリ名のコーディング規約を `CamelCase` から `snake_case` に変更
    - v4 系統ではこの変更は移行期間とし，あらゆるディレクトリ名を一気に変更する必要はないものとしている
    - [#42](https://github.com/arkedge/c2a-core/pull/42): Rust の crate のためのディレクトリは小文字の `-` 区切りとする
  - [#8](https://github.com/arkedge/c2a-core/issues/8): C2A 用語の rename
    - [#21](https://github.com/arkedge/c2a-core/pull/21), [#24](https://github.com/arkedge/c2a-core/pull/24): `IfWrapper` -> `HAL` (Hardware Abstraction Layer)
    - [#146](https://github.com/arkedge/c2a-core/pull/146), [#147](https://github.com/arkedge/c2a-core/pull/147) `Driver` -> `Component Driver`
    - [#143](https://github.com/arkedge/c2a-core/pull/143), [#144](https://github.com/arkedge/c2a-core/pull/144) `Driver instances` -> `Component Service`
    - [#153](https://github.com/arkedge/c2a-core/pull/153): `c2a-tlm-cmd-code-generator` -> `c2a-code-generator`
    - [#165](https://github.com/arkedge/c2a-core/pull/165), [#166](https://github.com/arkedge/c2a-core/pull/166): `IF list` -> `CDS HAL handler registry`
    - [#151](https://github.com/arkedge/c2a-core/issues/151): これは v4.0.0 以降も継続的に実施していく
  - [#28](https://github.com/arkedge/c2a-core/issues/28): 歪なディレクトリ構造の修正
    - [#122](https://github.com/arkedge/c2a-core/pull/122), [#124](https://github.com/arkedge/c2a-core/pull/124): `src/src_core/Drivers/Super` -> `src/src_core/driver`, `src/src_core/Drivers/Protocol` -> `src/src_core/driver`
    - [#138](https://github.com/arkedge/c2a-core/pull/138): `src/src_user/Test` -> `src/src_user/test`, `src/src_user/Test/test` -> `src/src_user/test`
    - [#10](https://github.com/arkedge/c2a-core/issues/10): ディレクトリ名の CamelCase をやめて snake_case にする
      - [#20](https://github.com/arkedge/c2a-core/pull/20): コーディング規約の変更
      - [#21](https://github.com/arkedge/c2a-core/pull/21), [#24](https://github.com/arkedge/c2a-core/pull/24): `IfWrapper` -> `hal`
      - [#26](https://github.com/arkedge/c2a-core/pull/26): `c2a-core/Examples` -> `c2a-core/examples`
      - [#31](https://github.com/arkedge/c2a-core/pull/31): `Docs` -> `docs`
      - [#33](https://github.com/arkedge/c2a-core/pull/33), [#39](https://github.com/arkedge/c2a-core/pull/39): `Script` -> `script`
      - [#46](https://github.com/arkedge/c2a-core/pull/46), [#48](https://github.com/arkedge/c2a-core/pull/48), [#140](https://github.com/arkedge/c2a-core/pull/140): `Applications` -> `applications`
      - [#50](https://github.com/arkedge/c2a-core/pull/50), [#51](https://github.com/arkedge/c2a-core/pull/51), [#53](https://github.com/arkedge/c2a-core/pull/53): `Library` -> `library`
      - [#54](https://github.com/arkedge/c2a-core/pull/54), [#55](https://github.com/arkedge/c2a-core/pull/55), [#125](https://github.com/arkedge/c2a-core/pull/125): `System` -> `system`
      - [#79](https://github.com/arkedge/c2a-core/pull/79), [#80](https://github.com/arkedge/c2a-core/pull/80): `TlmCmd` -> `tlm_cmd`
      - [#122](https://github.com/arkedge/c2a-core/pull/122), [#124](https://github.com/arkedge/c2a-core/pull/124): `Drivers` -> `driver`
      - [#138](https://github.com/arkedge/c2a-core/pull/138): `src/src_user/Test` -> `src/src_user/test`
      - [#167](https://github.com/arkedge/c2a-core/pull/167), [#168](https://github.com/arkedge/c2a-core/pull/168): `src/src_user/Settings` -> `src/src_user/settings`
  - 移行には [scripts/migration/](./scripts/migration/) 以下の `v4-rename-*.sh` の migration script を使用可能
    - example user についてはこの migration script のみで移行しているが，あらゆる C2A user の状況について考慮できているわけではないため，適宜対応が必要
- [#23](https://github.com/arkedge/c2a-core/issues/23), [#25](https://github.com/arkedge/c2a-core/pull/25), [#129](https://github.com/arkedge/c2a-core/pull/129), [#130](https://github.com/arkedge/c2a-core/pull/130), [#131](https://github.com/arkedge/c2a-core/pull/131): `hal`（旧 `IfWrapper`）のうち，古すぎるインターフェースを排除
  - 既に使われてしまっているものについては，各 C2A user で一旦雑に古いヘッダファイルを各々で持って対応可能（今後再設計する予定）
- [#97](https://github.com/arkedge/c2a-core/pull/97): `check_coding_rule.py` の設定ファイルである `check_coding_rule.json` の場所を C2A user のトップディレクトリに変更
  - 対応方法
    - `Script/CI/check_coding_rule.json` ないし `script/ci/check_coding_rule.json` を C2A user のトップディレクトリに移動
    - 設定ファイルから `c2a_root_dir` を削除
    - 設定ファイル中のディレクトリの設定を C2A user のトップディレクトリからの相対パスに変更
- [#99](https://github.com/arkedge/c2a-core/pull/99): [ut-issl/c2a-enum-loader](https://github.com/ut-issl/c2a-enum-loader) を c2a-core リポジトリで管理するように変更
  - import したバージョン: [ut-issl/c2a-enum-loader ae-v2.0.0](https://github.com/ut-issl/c2a-enum-loader/releases/tag/ae-v2.0.0)
- [#111](https://github.com/arkedge/c2a-core/pull/111): [ut-issl/c2a-tlm-cmd-code-generator](https://github.com/ut-issl/c2a-tlm-cmd-code-generator) を c2a-core リポジトリで管理するように変更
  - import したバージョン: [ut-issl/c2a-tlm-cmd-code-generator ae-v2.0.0](https://github.com/ut-issl/c2a-tlm-cmd-code-generator/releases/tag/ae-v2.0.0)
- [#83](https://github.com/arkedge/c2a-core/issues/83), [#86](https://github.com/arkedge/c2a-core/pull/86), [#132](https://github.com/arkedge/c2a-core/pull/132), [#139](https://github.com/arkedge/c2a-core/pull/139): CMake option の整理
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
  - `USE_SCI_COM_WINGS` -> `C2A_USE_SCI_COM_WINGS`: 今後 WINGS 向けビルドは optional なものとなるので，default で OFF に変更．使う場合はビルド時に指定するか，S2E user の `CMakeLists.txt` で設定すること
  - `USE_SCI_COM_UART` -> `C2A_USE_SCI_COM_UART`
  - `USE_SILS_MOCKUP` -> `C2A_BUILD_WITH_SILS_MOCKUP`
  - `SHOW_DEBUG_PRINT_ON_SILS` -> `C2A_BUILD_WITH_SILS_MOCKUP`


### Enhancements

- [#35](https://github.com/arkedge/c2a-core/pull/35), [#37](https://github.com/arkedge/c2a-core/pull/37): `c2a-core` crate で Rust 向けの binding を提供する
  - C2A 内で共通の [bindgen](https://github.com/rust-lang/rust-bindgen) の設定を使用可能にするため，[c2a-bind-utils](https://crates.io/crates/c2a-bind-utils) crate を追加
- C2A 単体での SILS のための C2A のランタイム実装である `c2a-sils-runtime` crate を追加
- 共通して使用可能な C2A HAL（旧 `IfWrapper`）実装群を追加
  - [#40](https://github.com/arkedge/c2a-core/pull/40): C2A HAL 実装のテンプレートや，モックとして使用可能な noop 実装を追加
  - [#41](https://github.com/arkedge/c2a-core/pull/41): [kble](https://github.com/arkedge/kble) を使った SILS 向け UART 実装を追加
- [#44](https://github.com/arkedge/c2a-core/pull/44), [#45](https://github.com/arkedge/c2a-core/pull/45): `c2a-sils-runtime` と Rust 製の C2A HAL 実装を使って，example user の C2A 単体での SILS（S2E なし）を実装
- [#56](https://github.com/arkedge/c2a-core/pull/56): 新たな C2A 標準環境である `C2A Boom` を example user に導入
  - これに伴い，各種 ArkEdge 独自のライブラリ・ツール群が 導入・OSS 化された
  - [Gaia](https://github.com/arkedge/gaia)
  - [tlmcmddb-cli](https://github.com/arkedge/c2a-tlmcmddb)
  - [kble](https://github.com/arkedge/kble)
  - [C2A DevTools](https://github.com/arkedge/c2a-devtools)
- [#59](https://github.com/arkedge/c2a-core/pull/59): example user に対して，Gaia を用いた pytest を導入
  - [python-wings-interface](https://github.com/ut-issl/python-wings-interface) 互換の Gaia（`tmtc-c2a`）向けインターフェースとして [c2a-pytest-gaia](https://github.com/arkedge/c2a-pytest-gaia) を導入
  - これに伴い，Python の仮想環境・ツールチェーン管理に [rye](https://rye-up.com/)

### Fixed

- [#30](https://github.com/arkedge/c2a-core/pull/30): `c2a-core` crate のためのソースファイルを `src/lib.rs` から `c2a_core.rs` に移動し，紛らわしい `src` ディレクトリを削除

### Compatible C2A Tools

- [c2a-code-generator](./code-generator): [ut-issl/c2a-tlm-cmd-code-generator ae-v2.0.0](https://github.com/ut-issl/c2a-tlm-cmd-code-generator/releases/tag/ae-v2.0.0) を c2a-core リポジトリに吸収し，rename した
- [c2a-enum-loader](./enum-loader): [ut-issl/c2a-enum-loader ae-v2.0.0](https://github.com/ut-issl/c2a-enum-loader/releases/tag/ae-v2.0.0) を c2a-core リポジトリに吸収した

- [ut-issl/tlm-cmd-db v2.4.0](https://github.com/ut-issl/tlm-cmd-db/releases/tag/v2.4.0)
- [ut-issl/python-wings-interface v1.5.1](https://github.com/ut-issl/python-wings-interface/releases/tag/v1.5.1)
- [arkedge/gaia v0.5.0](https://github.com/arkedge/gaia/releases/tag/v0.5.0)
- [tlmcmddb-cli 0.2.0](https://crates.io/crates/tlmcmddb-cli/0.2.0)
- [kble 0.2.0](https://crates.io/crates/kble/0.2.0)
- [C2A DevTools](https://github.com/arkedge/c2a-devtools)


### Migration Guide

- v4.0.0-alpha.0
  - [#19](https://github.com/arkedge/c2a-core/pull/18) の変更を取り込む
- v4.0.0-alpha.1
  - [#21](https://github.com/arkedge/c2a-core/pull/21), [#24](https://github.com/arkedge/c2a-core/pull/24): IfWrapper -> HAL の rename: [v4-rename-ifwrapper.sh](./script/migration/v4-rename-ifwrapper.sh) を実行
  - [#25](https://github.com/arkedge/c2a-core/pull/25): `IfWrapper/dc.h` を使用していた場合は，一旦 C2A user にヘッダを移す（`src/src_user/hal/dc.h`）
- v4.0.0-alpha.2
  - [#18](https://github.com/arkedge/c2a-core/pull/18) の変更を取り込む
- [yanked] [v4.0.0-beta.0](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.0): このバージョンは yank されているため，tag は存在するが使わないこと
- [v4.0.0-beta.1](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.1)
  - [#33](https://github.com/arkedge/c2a-core/pull/33), [#39](https://github.com/arkedge/c2a-core/pull/39): Script ディレクトリの rename を実行（[v4-rename-script.sh](./script/migration/v4-rename-script.sh)）
  - [#46](https://github.com/arkedge/c2a-core/pull/46), [#48](https://github.com/arkedge/c2a-core/pull/48): Applications ディレクトリの rename を実行（[v4-rename-applications.sh](./script/migration/v4-rename-applications.sh)）
  - [#50](https://github.com/arkedge/c2a-core/pull/50), [#51](https://github.com/arkedge/c2a-core/pull/51), [#53](https://github.com/arkedge/c2a-core/pull/53): Library ディレクトリの rename を実行（[v4-rename-library.sh](./script/migration/v4-rename-library.sh)）
  - [#54](https://github.com/arkedge/c2a-core/pull/54), [#55](https://github.com/arkedge/c2a-core/pull/55): System ディレクトリの rename を実行（[v4-rename-system.sh](./script/migration/v4-rename-system.sh)）
    - [#125](https://github.com/arkedge/c2a-core/pull/125) のため，c2a-core 部分の AddSection.pl はこの時点では動作しなくなってしまっている
  - [#79](https://github.com/arkedge/c2a-core/pull/79), [#80](https://github.com/arkedge/c2a-core/pull/80): TlmCmd ディレクトリの rename を実行（[v4-rename-tlmcmd.sh](./script/migration/v4-rename-tlmcmd.sh)）
  - [#96](https://github.com/arkedge/c2a-core/pull/96)
    - [ut-issl/c2a-enum-loader](https://github.com/c2a-enum-loader) のバージョン変更: [ae-v2.0.0](https://github.com/ut-issl/c2a-enum-loader/releases/tag/ae-v2.0.0)
    - [ut-issl/c2a-tlm-cmd-code-generator](https://github.com/ut-issl/c2a-tlm-cmd-code-generator) のバージョン変更: [ae-v2.0.0](https://github.com/ut-issl/c2a-tlm-cmd-code-generator/releases/tag/ae-v2.0.0)
  - [#97](https://github.com/arkedge/c2a-core/pull/97): check-coding-rule の設定ファイル（`check_coding_rule.json`）をトップディレクトリに移す
    - [workflows-c2a v4.3.0](https://github.com/arkedge/workflows-c2a/releases/tag/v4.3.0) からこの構造に対応
    - ただし，この挙動変更は workflows-c2a に対しては breaking change なので，新規に [.github/workflows/check-coding-rule-v4.yml](https://github.com/arkedge/workflows-c2a/blob/v4.3.0/.github/workflows/check-coding-rule-v4.yml) という workflow を生やしており，こちらに移行する必要がある（[workflows-c2a#63](https://github.com/arkedge/workflows-c2a/pull/63)）
- [v4.0.0-beta.2](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.2)
  - [#99](https://github.com/arkedge/c2a-core/pull/99): [c2a-enum-loader](./enum-loader) を `src/src_core/enum-loader` のものに切り替える
  - [#111](https://github.com/arkedge/c2a-core/pull/111): [c2a-tlm-cmd-code-generator](./code-generator) を c2a-core のものに切り替える
    - [v4.0.0-beta.2](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.2) 時点では `src/src_core/tlm-cmd-code-generator`
    - [v4.0.0-beta.5](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.5)（[#153](https://github.com/arkedge/c2a-core/pull/153)）以降は `src/src_core/code-generator`
  - [#86](https://github.com/arkedge/c2a-core/pull/86): CMake の option 修正
    - これは C2A user ごとに設定が異なるので，手動で確認・修正する必要がある
- [v4.0.0-beta.3](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.3)
  - [#122](https://github.com/arkedge/c2a-core/pull/122): Drivers ディレクトリの rename を実行（[v4-rename-driver.sh](./script/migration/v4-rename-driver.sh)）
    - [c2a-tlm-cmd-code-generator](./code-generator) にも更新があるので，念のため再度コード生成を行うこと（migration script の実行時点で diff が出て，それと変わらないはず）
  - [#132](https://github.com/arkedge/c2a-core/pull/132): C2A user 側の CMake option の整理
    - これは C2A user ごとに設定が異なるので，手動で確認・修正する必要がある
  - `C2A_USE_SCI_COM_WINGS` を default OFF にする
    - このオプションを使う場合（つまり，SILS-S2E で WINGS と疎通する場合），S2E user の `CMakeLists.txt` ないしビルドスクリプト側でこのオプションを ON にして使うこと．あくまで C2A user 単体としては，この機能は optional であるべき．
- [v4.0.0-beta.4](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.4)
  - [#146](https://github.com/arkedge/c2a-core/pull/146), [#147](https://github.com/arkedge/c2a-core/pull/147) Driver -> Component Driver の rename を実行（[v4-rename-driver2](./script/migration/v4-rename-driver2.sh)）
  - [#143](https://github.com/arkedge/c2a-core/pull/143), [#144](https://github.com/arkedge/c2a-core/pull/144) Driver instance -> Component Service の rename を実行（[v4-rename-driver-instance.sh](script/migration/v4-rename-driver-instance.sh)）
- [v4.0.0-beta.5](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.5)
  - [#153](https://github.com/arkedge/c2a-core/pull/153) code generator のパス変更: `src/src_core/tlm-cmd-code-generator` -> `src/src_core/code-generator`
  - [#138](https://github.com/arkedge/c2a-core/pull/138) C2A user の pytest のディレクトリ構成のリファクタ: [v4-rename-test-dir.sh](./script/migration/v4-rename-test-dir.sh) を実行
  - [#154](https://github.com/arkedge/c2a-core/pull/154), [#155](https://github.com/arkedge/c2a-core/pull/155): [v4-rename-component-driver-prefix.sh](./script/migration/v4-rename-component-driver-prefix.sh) を実行
  - [#165](https://github.com/arkedge/c2a-core/pull/165), [#166](https://github.com/arkedge/c2a-core/pull/166): [v4-rename-if-list.sh](./script/migration/v4-rename-if-list.sh)


- pytest への rye の導入
  - [#100](https://github.com/arkedge/c2a-core/pull/100): `build-backend` に pdm を指定する


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
- [v3.3.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.3.1)
