# CHANGELOG

注意: これは既存の C2A core update の「リリースの間の Pull Request を眺めてなんとなく察する」という曖昧な操作を緩和していくための試みであり，C2A user に対するお知らせを行う場として使っていくことを意図しています．初めから c2a-core の全変更を取り扱うと不必要に煩雑になるだけになってしまうため，完全な変更内容の一覧についてはこれまで通り [GitHub Releases](https://github.com/arkedge/c2a-core/releases) などから参照してください．



## v4.3.0 (2024-02-05)

### Breaking Changes
- [#301](https://github.com/arkedge/c2a-core/pull/301): `CCP_form_and_exec_*` の再帰実行時のメモリエラーバグの修正
- [#302](https://github.com/arkedge/c2a-core/pull/302): static に確保している packet 系の変数のローカル変数化


### Documentation

- [#299](https://github.com/arkedge/c2a-core/pull/299): アプリケーション追加時に必要な手順のドキュメント追加
- [#300](https://github.com/arkedge/c2a-core/pull/300): Tlm ID の採番についてのルールのドキュメント追加


### Migration Guide
- [Requirements](./docs/general/requirements.md) を参考に，マイコンのスタックメモリの容量に余裕があるかどうか確認すること（MUST）
  - 今後ハードウェアに対する要求事項は [docs/general/requirements.md](docs/general/requirements.md) に書かれることになるので，今後のアップデートでも適宜確認すること
  - 今回の経緯は [#303](https://github.com/arkedge/c2a-core/issues/303) を参照
  - [#301](https://github.com/arkedge/c2a-core/pull/301) で要求されるスタック容量が増加している
  - [#302](https://github.com/arkedge/c2a-core/pull/302) で結果的に同程度以上のメモリ使用量削減を行っているため，実用上は今回のアップデートに伴って C2A user 側でメモリ使用量削減をする必要は無い


## v4.2.0 (2024-01-26)

### Breaking Changes

- [#245](https://github.com/arkedge/c2a-core/pull/245): memory dump application のリファクタリング
- [#266](https://github.com/arkedge/c2a-core/pull/266): example mobc の tlm の tlm id を再採番する (CDIS, CA)
- [#269](https://github.com/arkedge/c2a-core/pull/269): Rename `src_user/settings/component_driver_super/` dir to `src_user/settings/component_driver/` dir
- [#275](https://github.com/arkedge/c2a-core/pull/275): AM tlm でのキャストを緩和し，1 tlm でダウンリンクできる個数を減少させた


### Enhancements

- C2A Boom
  - [#249](https://github.com/arkedge/c2a-core/pull/249): Example user に .nvmrc を追加し，Node version を固定
  - [#281](https://github.com/arkedge/c2a-core/pull/281): Use Gaia 0.6 in C2A Boom for C2A DevTools usage update
- c2a-core crates
  - [#133](https://github.com/arkedge/c2a-core/pull/133): sils-runtime: require c2a-core v4
  - [#134](https://github.com/arkedge/c2a-core/pull/134): c2a-core hal crates: require c2a-core v4
- code-generator
  - [#240](https://github.com/arkedge/c2a-core/pull/240): 自動生成コードの @note に，生成元DBのコミットハッシュと生成パラメータを出力する
  - [#243](https://github.com/arkedge/c2a-core/pull/243): max_tlm_num のアサーションを追加
  - [#251](https://github.com/arkedge/c2a-core/pull/251): Refactor code-generator
  - [#256](https://github.com/arkedge/c2a-core/pull/256): MOBC が定義を持っていない Sub OBC の tlm id の tlm でも GS に Forward できるように
- [#261](https://github.com/arkedge/c2a-core/pull/261): TL TLM に TL に登録された cmd 数を追加
- [#263](https://github.com/arkedge/c2a-core/pull/263): CDIS や BCT に保存された CCP をダンプする App を追加
- [#268](https://github.com/arkedge/c2a-core/pull/268): GS と FSW 側での同期のために，BCT, TL のダイジェスト (CRC) を下ろせるようにする App の追加
- [#270](https://github.com/arkedge/c2a-core/pull/270): Component Driver を統一的に c2a-core で扱えるようにし，CDS の tlm と，テレメ不通関連 Cmd を追加
- [#273](https://github.com/arkedge/c2a-core/pull/273): 任意の Component Driver に対して，任意バイト列の送受信と HAL init, reopen Cmd を提供する
- [#274](https://github.com/arkedge/c2a-core/pull/274): `TMGR_get_master_mode_cycle_in_msec` などの in_sec 版を実装
- [#260](https://github.com/arkedge/c2a-core/pull/260): C2A 初期化時に実行環境のエンディアンが設定と正しいかチェックする


### Fixed

- [#265](https://github.com/arkedge/c2a-core/pull/265): [code-generator] subobc の MD5 の計算のバグ修正
  - [#251](https://github.com/arkedge/c2a-core/pull/251): の修正であるため，中途半端なバージョンを使用していなければ対応不要
- [#279](https://github.com/arkedge/c2a-core/pull/279): `component_driver_utility.c` でのパラメタ取得のバグ修正
- [#286](https://github.com/arkedge/c2a-core/pull/286): Fix examples/subobc pytest testcase dir
- [#298](https://github.com/arkedge/c2a-core/pull/298): max_tlm_num のアサーションの off-by-one error の修正
  - [#243](https://github.com/arkedge/c2a-core/pull/243): の修正であるため，中途半端なバージョンを使用していなければ対応不要


### Documentation

- [#238](https://github.com/arkedge/c2a-core/pull/238): Add missing v4.1.0 compatible tools to changelog
- [#247](https://github.com/arkedge/c2a-core/pull/247): VS Code を使ったデバッグ手順の追加 / READMEの微修正
- [#278](https://github.com/arkedge/c2a-core/pull/278): タスク定期実行に関するドキュメントを追加


### Internal

- [#246](https://github.com/arkedge/c2a-core/pull/246): node_modules を .gitignore に追加
- [#231](https://github.com/arkedge/c2a-core/pull/231): Update actions/setup-python action to v5
- [#248](https://github.com/arkedge/c2a-core/pull/248): Update actions/setup-node action to v4.0.1
- [#253](https://github.com/arkedge/c2a-core/pull/253): Update actions/deploy-pages action to v4
- [#254](https://github.com/arkedge/c2a-core/pull/254): Update actions/upload-pages-artifact action to v3
- [#250](https://github.com/arkedge/c2a-core/pull/250): Update sksat/setup-rye action to v0.10.0
- [#252](https://github.com/arkedge/c2a-core/pull/252): Update sksat/action-clippy action to v0.4.0
- [#258](https://github.com/arkedge/c2a-core/pull/258): Update renovate default reviewers
- [#241](https://github.com/arkedge/c2a-core/pull/241): Update dependency ut-issl/s2e-core to v7.1.1
- [#271](https://github.com/arkedge/c2a-core/pull/271): CDS: ComponentDriverSuper の整理（コードに変更なし）
- [#272](https://github.com/arkedge/c2a-core/pull/272): sync_with_mobc_example.bat のメンテ
- [#264](https://github.com/arkedge/c2a-core/pull/264): Update dependency ut-issl/s2e-core to v7.2.0
- [#276](https://github.com/arkedge/c2a-core/pull/276): Update Rust crate semver to 1.0.21
- [#257](https://github.com/arkedge/c2a-core/pull/257): Update arkedge/workflows-c2a action to v5
- [#282](https://github.com/arkedge/c2a-core/pull/282): Update Swatinem/rust-cache action to v2.7.2
- [#283](https://github.com/arkedge/c2a-core/pull/283): Fix check-coding-rule v4 workflow in workflows-c2a v5
- [#284](https://github.com/arkedge/c2a-core/pull/284): Enable example build all again
- [#285](https://github.com/arkedge/c2a-core/pull/285): Use matrix job to build multiple example users with same config


### Migration Guide

- C2A Boom
  - [#281](https://github.com/arkedge/c2a-core/pull/281): `tools/install.sh` の Gaia のバージョン（Git tag）を `v0.6.1` 以降に更新する（RECOMMENDED）
    - これにより，C2A DevTools が `http://localhost:8900/devtools` から使えるようになる
    - これまで通り C2A DevTools サーバを別途立ち上げることもできるが，C2A DevTools は Gaia に[統合された](https://github.com/arkedge/gaia/pull/33)ため，非推奨となる
- コード生成
  - [#240](https://github.com/arkedge/c2a-core/pull/240), [#256](https://github.com/arkedge/c2a-core/pull/256): code-generator の機能追加・生成コード仕様の変更があるため，コード生成し直すこと（MUST）
  - 以下の各更新のうち，Tlm DB の更新を含むものは，Excel から読み込んで csv を再出力し `calced_data` を更新すること（MUST）
- [#269](https://github.com/arkedge/c2a-core/pull/269): `src/src_user/settings/component_driver_super` の rename（MUST）
  - `src/src_user/settings/component_driver_super` を `src/src_user/settings/component_driver` に rename する
  - user 側の include path を，`/component_driver_super/` から `/component_driver/` に一斉置換する
  - `src_user/settings/CMakeLists.txt` の `/component_driver_super/` が含まれる path を `/component_driver/` に一斉置換する
- [#261](https://github.com/arkedge/c2a-core/pull/261): `examples/mobc` の TL TLM を更新したので， `examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_TL.csv` を user 側の TL TLM に上書きする
- [#266](https://github.com/arkedge/c2a-core/pull/266): 以下の tlm id を変更したため，これらを user 側の tlm db に上書きする
  - `examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_CDIS.csv`
  - `examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_CA.csv`
- [#245](https://github.com/arkedge/c2a-core/pull/245): 影響範囲は memory dump application を使う OBC のみ
  - Tlm Cmd DB を `examples/mobc` のものと同様にする
    - cmd: `MEM_*`
    - tlm: `MEM`
  - `examples/mobc` の `src/src_user/settings/applications/memory_dump_define.h` を参考に作成して設定する
- [#263](https://github.com/arkedge/c2a-core/pull/263): CcpDump App の追加
  - この App を利用する user は `src/src_core/applications/ccp_dump.c` をビルド対象に加え，App 登録する
  - `examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_CCP_DUMP.csv` を user 側の tlm db に追加する
  - `examples/mobc/tlm-cmd-db/CMD_DB/SAMPLE_MOBC_CMD_DB_CMD_DB.csv` の `CCP_DUMP_*` コマンドを user 側の cmd db に追加する
  - `examples/mobc` から `src/src_user/test/src_core/applications/test_ccp_dump.py` をコピーする
    - もし，汎用テスト用 BCT `BC_TEST_USE_PYTEST` が user 側に存在しない場合， user 側の BCT ID にも加える
- [#268](https://github.com/arkedge/c2a-core/pull/268): TL BCT Digest App の追加
  - この App を利用する user は `applications/tl_bct_digest.c` をビルド対象に加え，App 登録する
  - `examples/mobc/tlm-cmd-db/TLM_DB/{SAMPLE_MOBC_TLM_DB_TL_DIGEST.csv,SAMPLE_MOBC_TLM_DB_BCT_DIGEST.csv}` を user 側の tlm db に追加する
  - `examples/mobc/tlm-cmd-db/CMD_DB/SAMPLE_MOBC_CMD_DB_CMD_DB.csv` の `TL_BCT_DIGEST_*` コマンドを user 側の cmd db に追加する
  - `examples/mobc` から `src/src_user/test/src_core/applications/test_tl_bct_digest.py` をコピーする
    - もし，汎用テスト用 BCT `BC_TEST_USE_PYTEST` が user 側に存在しない場合， user 側の BCT ID にも加える
- [#270](https://github.com/arkedge/c2a-core/pull/270), [#273](https://github.com/arkedge/c2a-core/pull/273): ComponentDriverUtility App の追加
  - `CDRV_ID` を user 側で設定する
    - `examples/mobc` を参考に，`src/src_user/settings/component_driver/component_driver_define.{c,h}` を配置し，ビルド対象に加える
  - この App を利用する user は `src/src_core/applications/component_driver_utility.c` をビルド対象に加え，App 登録する
  - `examples/mobc` を参考に，`src/src_user/settings/applications/component_driver_utility_params.h` を配置する
  - `examples/mobc/tlm-cmd-db/CMD_DB/SAMPLE_MOBC_CMD_DB_CMD_DB.csv` の `CDRV_UTIL_*` コマンドを user 側の cmd db に追加する
  - `examples/mobc/tlm-cmd-db/CMD_DB/SAMPLE_MOBC_CMD_DB_CMD_DB.csv` の `CDRV_UTIL_*` コマンドを user 側の cmd db に追加する
  - `examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_CDRV_UTIL.csv` を user 側の tlm db に追加する
  - `examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_CDRV_UTIL_HAL_RX_DATA.csv` を user 側の tlm db に追加する
  - もし， user 側で subobc ありの pytest を扱っている場合，`examples/subobc` から `src/src_user/test/test_component_driver_utility.py` をコピーする
- [#275](https://github.com/arkedge/c2a-core/pull/275): AM tlm のキャスト緩和（u8 -> u16）に伴う tlm 再設定
  - App manager のテレメサイズのデフォルト設定が変わったため，再設定すること
  - App の初期化時間がそこまで長くならないような状況であったり，テレメサイズを切り詰めたい場合は引き続き u8 としてよい
  - `examples/mobc` と同様にする場合は，`examples/mobc/tlm-cmd-db/TLM_DB/SAMPLE_MOBC_TLM_DB_AM.csv` を user 側の AM tlm に上書きする
  - `examples/mobc` を参考に `src/src_user/settings/system/app_manager_params.h` を更新する
    - tlm と整合が取れるように，特に `AM_TLM_PAGE_SIZE`, `AM_TLM_PAGE_SIZE` に注意して更新する


## v4.1.0 (2023-12-11)

### Breaking Changes

- [#193](https://github.com/arkedge/c2a-core/pull/193): VCDU を AOS transfer frame に rename する
- [#197](https://github.com/arkedge/c2a-core/pull/197): M_PDU をコード規約に合わせるなどのリファクタ
- [#199](https://github.com/arkedge/c2a-core/pull/199): Drop c2a-core crate links config
- [#201](https://github.com/arkedge/c2a-core/pull/201): Refactor release rule
- [#225](https://github.com/arkedge/c2a-core/pull/225): Space Packet 関連ファイルのディレクトリを変更
- [#214](https://github.com/arkedge/c2a-core/pull/214): CCSDS の主に MOBC 向け Data Link Layer のコードを Core 管理にする
- [#219](https://github.com/arkedge/c2a-core/pull/219): CCSDS data link layer における SCID をユーザー設定として切り出す
- [#220](https://github.com/arkedge/c2a-core/pull/220): APIDの命名規則の更新
  - 命名規則は `tlm_cmd/common_tlm_cmd_packet.h` を参照

### Fixed

- [#188](https://github.com/arkedge/c2a-core/pull/188): [v3.10.1 Backport] PL_BC_LIST_CLEARED を level high で EL 登録する
- [#204](https://github.com/arkedge/c2a-core/pull/204): More match v4.0.0 script migration
- [#205](https://github.com/arkedge/c2a-core/pull/205): Fix v4 applications rename order
- [#206](https://github.com/arkedge/c2a-core/pull/206): More match system migration
- [#211](https://github.com/arkedge/c2a-core/pull/211): Fix v4 system rename typo
- [#212](https://github.com/arkedge/c2a-core/pull/212): More match v4 tlmcmd rename migration
- [#218](https://github.com/arkedge/c2a-core/pull/218): Feedback from v4.0.0 beta.4
- [#223](https://github.com/arkedge/c2a-core/pull/223): Feedback from v4.0.0 beta.5 migration
- [#224](https://github.com/arkedge/c2a-core/pull/224): Feedback from v4.0.0 migration
- [#222](https://github.com/arkedge/c2a-core/pull/222): add section 漏れを修正

### Documentation

- [#215](https://github.com/arkedge/c2a-core/pull/215): Update v4.0.0 migration guide
- [#207](https://github.com/arkedge/c2a-core/pull/207): Move doxygen config to top
- [#208](https://github.com/arkedge/c2a-core/pull/208): Exclude Doxygen stdint_wrapper
- [#209](https://github.com/arkedge/c2a-core/pull/209): Deploy Doxygen generated HTML to GitHub Pages

### Internal

- [#179](https://github.com/arkedge/c2a-core/pull/179): Update Rust crate bindgen to 0.69.1
- [#210](https://github.com/arkedge/c2a-core/pull/210): Update dependency ut-issl/s2e-core to v7.1.0
- [#229](https://github.com/arkedge/c2a-core/pull/229): Run Rust CI on all workspace crates

### Migration Guide
- [#193](https://github.com/arkedge/c2a-core/pull/193): 影響範囲は MOBC のみ
  1. `src_user/tlm_cmd/ccsds/vcdu.{c,h}` を消し，`src_user/tlm_cmd/ccsds/aos_transfer_frame.{c,h}` を `examples/mobc` からコピーする． `CMakeLists.txt` も修正する．
  1. `VCDU` を `AOSTF` に置換する（TLM DB の csv なども）．
  1. `vcdu` を `aostf` に置換する（TLM DB の csv なども）．
  1. `vcdu.h` を `aos_transfer_frame.h` に置換する（include の修正）．
  1. コンパイルが通らないところを直す．想定されるものは以下．
     - `VCDU` 構造体 が `AosTransferFrame` 構造体に変わったので，変数定義の型名が変わっているはず．
- [#197](https://github.com/arkedge/c2a-core/pull/197): 影響範囲は MOBC のみ
  1. `src_user/tlm_cmd/ccsds/m_pdu.{c,h}` を消し，`src_user/tlm_cmd/ccsds/multiplexing_protocol_data_unit.{c,h}` を `examples/mobc` からコピーする． `CMakeLists.txt` も修正する．
  1. `m_pdu.h` を `multiplexing_protocol_data_unit.h` に置換する（include の修正）．
  1. コンパイルが通らないところを直す．想定されるものは以下．
     - `M_PDU` 構造体 が `MultiplexingProtocolDataUnit` 構造体に変わったので，変数定義の型名が変わっているはず．
- [#225](https://github.com/arkedge/c2a-core/pull/225)
  1. コンパイルが通るように，以下の include の修正を行う．
     - `#include <src_core/tlm_cmd/ccsds/space_packet.h>` -> `#include <src_core/tlm_cmd/ccsds/space_packet_protocol/space_packet.h>`
     - `#include <src_core/tlm_cmd/ccsds/space_packet_typedef.h>` -> `#include <src_core/tlm_cmd/ccsds/space_packet_protocol/space_packet_typedef.h>`
     - `#include <src_core/tlm_cmd/ccsds/tlm_space_packet.h>` -> `#include <src_core/tlm_cmd/ccsds/space_packet_protocol/tlm_space_packet.h>`
     - `#include <src_core/tlm_cmd/ccsds/cmd_space_packet.h>` -> `#include <src_core/tlm_cmd/ccsds/space_packet_protocol/cmd_space_packet.h>`
- [#214](https://github.com/arkedge/c2a-core/pull/214): 影響範囲は MOBC の CCSDS Data Link Layer の実装
  1. AOS Space Data Link Protocol の実装を c2a-core のものに切り替える
     1. `src_user/tlm_cmd/ccsds/` 内の既存の実装を消す
        - `aos_transfer_frame.{c,h}`
        - `multiplexing_protocol_data_unit.{c,h}`
        - `tcp_to_m_pdu.{c,h}`
     1. c2a-core の実装を使うように切り替える
        - `src_core/ccsds/aos_space_data_link_protocol/` 内のソースファイルをビルド対象に追加する
        - CMake の場合， `C2A_USE_CORE_CCSDS_AOS_SPACE_DATA_LINK_PROTOCOL` option を `ON` にするだけでよい (C2A user top の `CMakeLists.txt`)
          - `examples/mobc/CMakeLists.txt` を参考にできる．
     1. コンパイルが通らないところを直す．ファイルの場所が変わったことによる include path の修正が想定される．
  1. TC Space Data Link Protocol の実装を c2a-core のものに切り替える
     1. `src_user/tlm_cmd/ccsds/` 内の既存の実装を消す
        - `tc_segment.{c,h}`
        - `tc_transfer_frame.{c,h}`
     1. c2a-core の実装を使うように切り替える
        - `src_core/ccsds/tc_space_data_link_protocol/` 内のソースファイルをビルド対象に追加する
        - CMake の場合， `C2A_USE_CORE_CCSDS_TC_SPACE_DATA_LINK_PROTOCOL` option を `ON` にするだけでよい (C2A user top の `CMakeLists.txt`)
          - `examples/mobc/CMakeLists.txt` を参考にできる．
     1. コンパイルが通らないところを直す．ファイルの場所が変わったことによる include path の修正が想定される．
- [#219](https://github.com/arkedge/c2a-core/pull/219): 影響範囲は MOBC のみ
  1. PR の diff (`examples/mobc/src/`) に出ている修正を， user にも反映させる．
- [#220](https://github.com/arkedge/c2a-core/pull/220)
  1. `tlm_cmd/common_tlm_cmd_packet.h` にある命名規則に従うように， APID の命名を更新する．

### Compatible C2A Tools

- [ut-issl/tlm-cmd-db v2.4.0](https://github.com/ut-issl/tlm-cmd-db/releases/tag/v2.4.0)
- [ut-issl/python-wings-interface v1.5.1](https://github.com/ut-issl/python-wings-interface/releases/tag/v1.5.1)
- [arkedge/gaia v0.5.0](https://github.com/arkedge/gaia/releases/tag/v0.5.0)
- [tlmcmddb-cli 0.2.0](https://crates.io/crates/tlmcmddb-cli/0.2.0)
- [kble 0.2.0](https://crates.io/crates/kble/0.2.0)
- [C2A DevTools](https://github.com/arkedge/c2a-devtools)


## v4.0.1 (2023-11-09)

### Fixed

- [#180](https://github.com/arkedge/c2a-core/pull/180): Refactor v4 IfWrapper migration script
- [#182](https://github.com/arkedge/c2a-core/pull/182): Refactor v4 migration script

### Internal

- [#185](https://github.com/arkedge/c2a-core/pull/185): Add Gaia label

## v4.0.0 (2023-11-02)

[arkedge/c2a-core](https://github.com/arkedge/c2a-core) v4.0.0 は ArkEdge Space Inc. による C2A の先行開発のための [ut-issl/c2a-core](https://github.com/ut-issl/c2a-core) の fork 後最初のリリースとなる．
v4.0.0 では，fork に伴う開発体制の変更に加え，以下の大きなセマンティクス的な変更を行ったため，major update とした．

- 大規模な rename
- 各ツールの c2a-core リポジトリへの吸収合併
- [Rust](https://www.rust-lang.org/) のエコシステム / C2A 標準開発環境 C2A Boom の導入

大規模な rename はあくまで現在の設計・コードの責務をできるだけ明らかにし，（特に C2A の開発経験の無い一般的なソフトウェアエンジニアを中心とした）開発者のディスコミュニケーションを低減する目的で実施した．

ex: `IfWrapper` -> `HAL`, `Driver` -> `Component Driver`

また，ディレクトリ名に大文字・小文字があったりなかったりする，不必要に階層構造が深いといった歪なディレクトリ構造の修正も行った．

ex: `src/src_core/System` -> `src/src_core/system`, `src/src_user/Settings/TlmCmd/data_base` -> `tlm-cmd-db`

これらの rename は大規模ではあるものの，C2A としてのロジックの変更やリファクタリングはほぼ行っていない．
この移行については migration script を用意している（Migration Guide 参照）．

各ツールの c2a-core リポジトリへの吸収合併については，[ut-issl/c2a-enum-loader](https://github.com/ut-issl/c2a-enum-loader) や [ut-issl/c2a-tlm-cmd-code-generator](https://github.com/ut-issl/c2a-tlm-cmd-code-generator) などの c2a-core のコード構成に依存したツールのリリース単位は c2a-core と同一であると判断し実施した．
これらのツールについては，今後は各 C2A user で使用している c2a-core と同じバージョンのものを使用してもらうことになる（実務上は `src/src_core` から使うことになる）．

Rust エコシステム / C2A Boom の導入については，ArkEdge Space Inc. 内での開発成果を OSS 化しつつ実施した．
これらについては c2a-core v3 系に対しても互換性があり導入可能だが，新機能の開発や積極的なサポートは c2a-core v4 系以降を前提として行われる可能性がある．

### Breaking Changes

- [ut-issl/c2a-core](https://github.com/ut-issl/c2a-core) [v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0) から ArkEdge Space Inc. による先行開発のために fork し，リポジトリを [arkedge/c2a-core](https://github.com/arkedge/c2a-core) に移行
  - [#1](https://github.com/arkedge/c2a-core/pull/1): ライセンス情報の更新
- [94](https://github.com/arkedge/c2a-core/pull/94): メジャーバージョンアップ中は煩雑さ軽減のために c2a-core の Pre-Release の免除を可能に
- [#18](https://github.com/arkedge/c2a-core/pull/18): CommandDispatcher の統一的なテレメの提供
-  [#19](https://github.com/arkedge/c2a-core/pull/19): App の initializer, entry_point の返り値を void から RESULT にする
- 各ディレクトリ名・構造の変更
  - [#15](https://github.com/arkedge/c2a-core/pull/15): example user を `minimum_user` -> `mobc`, `2nd_obc_user` -> `subobc` に変更
  - [#20](https://github.com/arkedge/c2a-core/pull/20): ディレクトリ名のコーディング規約を `CamelCase` から `snake_case` に変更
    - v4 系統ではこの変更は移行期間とし，あらゆるディレクトリ名を一気に変更する必要はないものとしている
    - [#42](https://github.com/arkedge/c2a-core/pull/42): Rust の crate のためのディレクトリは小文字の `-` 区切りとする
  - [#8](https://github.com/arkedge/c2a-core/issues/8): C2A 用語の rename
    - [#21](https://github.com/arkedge/c2a-core/pull/21), [#24](https://github.com/arkedge/c2a-core/pull/24): `IfWrapper` -> `HAL` (Hardware Abstraction Layer)
    - [#146](https://github.com/arkedge/c2a-core/pull/146), [#147](https://github.com/arkedge/c2a-core/pull/147), [#149](https://github.com/arkedge/c2a-core/pull/149): `Driver` -> `Component Driver`
    - [#143](https://github.com/arkedge/c2a-core/pull/143), [#144](https://github.com/arkedge/c2a-core/pull/144), [#150](https://github.com/arkedge/c2a-core/pull/150): `Driver instances` -> `Component Service`
    - [#153](https://github.com/arkedge/c2a-core/pull/153): `c2a-tlm-cmd-code-generator` -> `c2a-code-generator`
    - [#165](https://github.com/arkedge/c2a-core/pull/165), [#166](https://github.com/arkedge/c2a-core/pull/166), [#169](https://github.com/arkedge/c2a-core/pull/169): `IF list` -> `CDS HAL handler registry`
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
    - [#172](https://github.com/arkedge/c2a-core/pull/172): `src/src_user/settings/tlm_cmd/data_base` -> `tlm-cmd-db`
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
- [#38](https://github.com/arkedge/c2a-core/pull/38): C2A 単体での SILS のための C2A のランタイム実装である `c2a-sils-runtime` crate を追加
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
  - これに伴い，Python の仮想環境・ツールチェーン管理に [rye](https://rye-up.com/) を導入
  - [#60](https://github.com/arkedge/c2a-core/pull/60): example user の Gaia pytest CI を導入
- [#117](https://github.com/arkedge/c2a-core/pull/117): `tlm-cmd-code-generator/settings.json` を commit しないようにし，example user 用の設定ファイルを追加した
- [#127](https://github.com/arkedge/c2a-core/pull/127): Import README.md as crate documentation

### Fixed

- [#77](https://github.com/arkedge/c2a-core/pull/77): Use locked version dependencies for install jrsonnet
- [#92](https://github.com/arkedge/c2a-core/pull/92): Build C2A example users for i686 target
- [#96](https://github.com/arkedge/c2a-core/pull/96): Use released version tools

### Documentation

- [#1](https://github.com/arkedge/c2a-core/pull/1): v4 fork に伴う README，Licence の修正
- [#3](https://github.com/arkedge/c2a-core/pull/3): Issue/PR template の更新
- [#16](https://github.com/arkedge/c2a-core/pull/16): Docs の中でコードを参照している URL の行数を修正
- [#29](https://github.com/arkedge/c2a-core/pull/29): Update repository URL in Rust package due to fork
- [#52](https://github.com/arkedge/c2a-core/pull/52): Update README about SILS runtime
- [#62](https://github.com/arkedge/c2a-core/pull/62): 関連リポジトリをreadmeに追加
- [#65](https://github.com/arkedge/c2a-core/pull/65): 通信プロトコルについてのドキュメントを更新
- [#85](https://github.com/arkedge/c2a-core/pull/85): Create CHANGELOG.md & write v4 current change log
- [#93](https://github.com/arkedge/c2a-core/pull/93): リリースルールの明瞭化
- [#123](https://github.com/arkedge/c2a-core/pull/123): c2a-sils-runtime や C2A DevTools での開発のためのドキュメントを追加
- [#135](https://github.com/arkedge/c2a-core/pull/135): Fix CHANGELOG.md
- [#157](https://github.com/arkedge/c2a-core/pull/157): Update v4 changelog
- [#177](https://github.com/arkedge/c2a-core/pull/177): インデントのスペースを GFM に準拠する

### Internal

- [#4](https://github.com/arkedge/c2a-core/pull/4): Update Rust crate semver to 1.0.18
- [#27](https://github.com/arkedge/c2a-core/pull/27): エンコーディングチェック CI に subobc が抜けていたので追加
- [#30](https://github.com/arkedge/c2a-core/pull/30): `c2a-core` crate のためのソースファイルを `src/lib.rs` から `c2a_core.rs` に移動し，紛らわしい `src` ディレクトリを削除
- [#34](https://github.com/arkedge/c2a-core/pull/34): Make c2a-core repository as cargo workspace
- [#36](https://github.com/arkedge/c2a-core/pull/36): Add rustfmt check
- [#47](https://github.com/arkedge/c2a-core/pull/47): Update Swatinem/rust-cache action to v2.6.2
- [#57](https://github.com/arkedge/c2a-core/pull/57): Run Rust CI on develop
- [#61](https://github.com/arkedge/c2a-core/pull/61): Update reviewdog/action-setup action to v1.0.6
- [#63](https://github.com/arkedge/c2a-core/pull/63): Pre Release (v4.0.0-beta.0): v4 の最初の beta release
- [#66](https://github.com/arkedge/c2a-core/pull/66): Update actions/setup-node action to v3.8.1
- [#67](https://github.com/arkedge/c2a-core/pull/67): 主に S2E の更新のために Renovate regexManager で GitHub Actions workflow 内のバージョン直打ちの依存の自動更新
- [#68](https://github.com/arkedge/c2a-core/pull/68): Update dependency ut-issl/s2e-core to v6.4.0
- [#69](https://github.com/arkedge/c2a-core/pull/69): Remove Renovate regexManager match v prefix
- [#72](https://github.com/arkedge/c2a-core/pull/72): Update arkedge/workflows-c2a action to v4
- [#74](https://github.com/arkedge/c2a-core/pull/74): pytest CI の 重複を除去する
- [#75](https://github.com/arkedge/c2a-core/pull/75): Update sksat/setup-rye action to v0.7.0
- [#78](https://github.com/arkedge/c2a-core/pull/78): Automatically add bug label to bug report issue
- [#81](https://github.com/arkedge/c2a-core/pull/81): Update actions/checkout action to v4
- [#87](https://github.com/arkedge/c2a-core/pull/87): Update Rust crate bindgen to 0.68.1
- [#89](https://github.com/arkedge/c2a-core/pull/89): Update crazy-max/ghaction-github-labeler action to v5
- [#90](https://github.com/arkedge/c2a-core/pull/90): Update Swatinem/rust-cache action to v2.7.0
- [#98](https://github.com/arkedge/c2a-core/pull/98): Update arkedge/workflows-c2a action to v4.3.0
- [#101](https://github.com/arkedge/c2a-core/pull/101): Update actions/checkout action to v4.1.0
- [#102](https://github.com/arkedge/c2a-core/pull/102): Update Rust crate semver to 1.0.19
- [#109](https://github.com/arkedge/c2a-core/pull/109): Update sksat/setup-rye action to v0.8.0
- [#110](https://github.com/arkedge/c2a-core/pull/110): Update arkedge/workflows-c2a action to v4.4.0
- [#112](https://github.com/arkedge/c2a-core/pull/112): Update python Docker tag to v3.12
- [#113](https://github.com/arkedge/c2a-core/pull/113): Update sksat/setup-rye action to v0.9.0
- [#118](https://github.com/arkedge/c2a-core/pull/118): Remove unneeded directory
- [#120](https://github.com/arkedge/c2a-core/pull/120): Update Rust crate semver to 1.0.20
- [#126](https://github.com/arkedge/c2a-core/pull/126): Add example user's tlm_cmd code generation check workflow
- [#136](https://github.com/arkedge/c2a-core/pull/136): Update arkedge/workflows-c2a action to v4.5.1
- [#137](https://github.com/arkedge/c2a-core/pull/137): Update arkedge/workflows-c2a action to v4.5.2
- [#152](https://github.com/arkedge/c2a-core/pull/152): Update dependency ut-issl/s2e-core to v7
  - [ut-issl/s2e-core#519](https://github.com/ut-issl/s2e-core/pull/519): Fix include c2a-core v4
- [#158](https://github.com/arkedge/c2a-core/pull/158): Update actions/checkout action to v4.1.1
- [#159](https://github.com/arkedge/c2a-core/pull/159): Update sksat/setup-rye action to v0.9.1
- [#160](https://github.com/arkedge/c2a-core/pull/160): Update sksat/action-clippy action to v0.3.0
- [#161](https://github.com/arkedge/c2a-core/pull/161): Update Swatinem/rust-cache action to v2.7.1
- [#164](https://github.com/arkedge/c2a-core/pull/164): Update actions/setup-node action to v4
- [#173](https://github.com/arkedge/c2a-core/pull/173): Fix missing rename/drop in scripts

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

注意: migration 方法やスクリプトの修正などもありえるため，本 Migration Guide 及び Migration Script は c2a-core v4 系のその時点での最新版を参照すること．例えば，使う c2a-core version が v4.0.0 であっても，v4.1.0 や v4.2.0 がリリースされていれば，その時点のものを参照すること．

- pytest への [rye](https://rye-up.com/) の導入（RECOMMENDED）
  - [#59](https://github.com/arkedge/c2a-core/pull/59): Gaia pytest の導入
  - c2a-core [example user](./examples) では [WINGS](https://github.com/ut-issl/wings) に変わる新たな地上局システムである [Gaia](https://github.com/arkedge/gaia) を用いており，pytest のバックエンドシステムを [Gaia](https://github.com/arkedge/gaia) に切り替えている（RECOMMENDED）
  - この切り替えと同時に，pytest project の依存関係管理のために [rye](https://rye-up.com/) を導入している．
  - [Gaia](https://github.com/arkedge/gaia)への切り替えの有無に関わらず，pytest を含む Python の環境整備には [rye](https://rye-up.com/) を推奨する（RECOMMENDED）
- 現在使用している c2a-core version を確認する（MUST）
  - [ut-issl/c2a-core](https://github.com/ut-issl/c2a-core) v3.8 系以前: サポート外．まずは v3.9 系までアップデートすること．
  - [ut-issl/c2a-core v3.9.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.0): サポート外．[ut-issl/c2a-core v3.9.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.1) が結果的に breaking な release となってしまっているため，必ず [ut-issl/c2a-core v3.9.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.1) を経由して v3.10 系にアップデートすること．
  - [ut-issl/c2a-core v3.9.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.1) ~ [ut-issl/c2a-core v3.9.2](https://github.com/ut-issl/c2a-core/releases/tag/v3.9.2): 移行はほぼ確実に可能と思われるが，明示的なサポートはしない．容易にアップデートが可能であるはずなため，事前に[ut-issl/c2a-core v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0) へアップデートすること．
  - [ut-issl/c2a-core v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0): [v4.0.0](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0) ~ [v4.0.1](https://github.com/arkedge/c2a-core/releases/tag/v4.0.1) への移行をサポート．
  - [ut-issl/c2a-core v3.10.1](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.1): 限定的にサポート．
    - [v4.0.0](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0) ~ [v4.0.1](https://github.com/arkedge/c2a-core/releases/tag/v4.0.1) への移行: 可能ではあるものの，[ut-issl/c2a-core v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0) からの差分は一時的に失われることになる
    - v4.0.2(TBD) で移行をサポート予定
- [v4.0.0-alpha.0](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-alpha.0)
  - [#19](https://github.com/arkedge/c2a-core/pull/18) の変更を取り込む
- [v4.0.0-alpha.1](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-alpha.1)
  - [#21](https://github.com/arkedge/c2a-core/pull/21), [#24](https://github.com/arkedge/c2a-core/pull/24): IfWrapper -> HAL の rename: [v4-rename-ifwrapper.sh](./script/migration/v4-rename-ifwrapper.sh) を実行
  - [#25](https://github.com/arkedge/c2a-core/pull/25): `IfWrapper/dc.h` を使用していた場合は，一旦 C2A user にヘッダを移す（`src/src_user/hal/dc.h`）
- [v4.0.0-alpha.2](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-alpha.2)
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
  - [#100](https://github.com/arkedge/c2a-core/pull/100): rye の `build-backend` に pdm を指定する
    - [c2a-enum-loader](./enum-loader) の相対パスでの指定への切り替えのためにすることになる（SHALL）
    - これにより，rye の lockfile にそれを実行した PC での絶対パスが吐かれてしまうという既知の問題があるが，これは別途今後対処予定（[#106](https://github.com/arkedge/c2a-core/issues/106)）であり，v4.0.0 の段階では一旦許容するものとしている
  - [#99](https://github.com/arkedge/c2a-core/pull/99): [c2a-enum-loader](./enum-loader) を `src/src_core/enum-loader` のものに切り替える
    - このバージョンから [ut-issl/c2a-enum-loader](https://github.com/ut-issl/c2a-enum-loader/) が c2a-core リポジトリに同梱されるようになった
    - そのため，pytest で c2a-enum-loader を指定している部分を切り替える必要がある（SHOULD）
      - `src/src_user/Test/pyproject.toml` の `dependencies` の `c2aenum` の部分を以下のようにする
      - `"c2aenum @ file:///${PROJECT_ROOT}/../../src_core/enum-loader"`
  - [#111](https://github.com/arkedge/c2a-core/pull/111): [c2a-tlm-cmd-code-generator](./code-generator) を c2a-core のものに切り替える
    - このバージョンから [ut-issl/c2a-tlm-cmd-code-generator](https://github.com/ut-issl/c2a-tlm-cmd-code-generator) が c2a-core リポジトリに同梱されるようになった
    - そのため，各 C2A user でのコード生成は以下のディレクトリで実行することになる（SHALL）
      - [v4.0.0-beta.2](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.2) 時点では `src/src_core/tlm-cmd-code-generator`
      - [v4.0.0-beta.5](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.5)（[#153](https://github.com/arkedge/c2a-core/pull/153)）以降は `src/src_core/code-generator`
  - [#86](https://github.com/arkedge/c2a-core/pull/86): C2A 全体で共通の CMake の option 修正
    - C2A 全体での CMake option の命名の [コーディングルール](https://github.com/arkedge/c2a-core/blob/6a32752b6d5cea1ef5a5eb68c410cf705315170d/docs/general/coding_rule.md#cmake-option) が追加された
    - C2A user ごとに設定があるので，手動で確認・修正する必要がある
    - C2A user・c2a-core を横断する設定なので，c2a-core と命名を揃えなければならない（MUST）
    - c2a-core でのマージ順などの関係上リリースが分かれているが，[#132](https://github.com/arkedge/c2a-core/pull/132) と同様の対応はこれと同種のものとして v4.0.0-beta.2 に更新するタイミングでまとめてやってもよい（MAY）
      - あくまで c2a-core リポジトリでの作業としては C2A 全体で共通の option の整理を [#86](https://github.com/arkedge/c2a-core/pull/86) で，C2A user 固有の設定を [#132](https://github.com/arkedge/c2a-core/pull/132) で別途実施しているが，必ず v4.0.0-beta.3 でやらなければならない理由があるわけではない
- [v4.0.0-beta.3](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.3)
  - [#122](https://github.com/arkedge/c2a-core/pull/122): Drivers ディレクトリの rename を実行（[v4-rename-driver.sh](./script/migration/v4-rename-driver.sh)）
    - [c2a-tlm-cmd-code-generator](./code-generator) にも更新があるので，念のため再度コード生成を行うこと（migration script の実行時点で diff が出て，それと変わらないはず）
  - [#132](https://github.com/arkedge/c2a-core/pull/132): C2A user 側の CMake option の整理
    - C2A user ごとに設定が異なるので，手動で確認・修正する必要がある
    - ただし，example user なので基本的には各 C2A user でもテンプレート的にこの実装が用いられているはずであり，同じ/同様の option 名を使っている場合は example user での命名に揃えることが推奨される（RECOMMENDED）
  - `C2A_USE_SCI_COM_WINGS` を default OFF にする（RECOMMENDED）
    - このオプションを使う場合（つまり，SILS-S2E で WINGS と疎通する場合），S2E user の `CMakeLists.txt` ないしビルドスクリプト側でこのオプションを ON にして使うこと．あくまで C2A user 単体としては，この機能は optional であるべき．
- [v4.0.0-beta.4](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.4)
  - [#146](https://github.com/arkedge/c2a-core/pull/146), [#147](https://github.com/arkedge/c2a-core/pull/147): Driver -> Component Driver の rename を実行（[v4-rename-driver2](./script/migration/v4-rename-driver2.sh)）
  - [#143](https://github.com/arkedge/c2a-core/pull/143), [#144](https://github.com/arkedge/c2a-core/pull/144), [#150](https://github.com/arkedge/c2a-core/pull/150): Driver instance -> Component Service の rename を実行（[v4-rename-driver-instance.sh](script/migration/v4-rename-driver-instance.sh)）
    - 略語の 'DI' は短すぎるため，文脈が明らかなものしか sed していない．適宜検索して置き換えること（特にコマンド名になっているようなものに注意）．
- [v4.0.0-beta.5](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0-beta.5)
  - [#153](https://github.com/arkedge/c2a-core/pull/153): code generator のパス変更: `src/src_core/tlm-cmd-code-generator` -> `src/src_core/code-generator`
  - [#138](https://github.com/arkedge/c2a-core/pull/138): C2A user の pytest のディレクトリ構成のリファクタ: [v4-rename-test-dir.sh](./script/migration/v4-rename-test-dir.sh) を実行
  - [#154](https://github.com/arkedge/c2a-core/pull/154), [#155](https://github.com/arkedge/c2a-core/pull/155): [v4-rename-component-driver-prefix.sh](./script/migration/v4-rename-component-driver-prefix.sh) を実行
  - [#165](https://github.com/arkedge/c2a-core/pull/165), [#166](https://github.com/arkedge/c2a-core/pull/166), [#169](https://github.com/arkedge/c2a-core/pull/169): [v4-rename-if-list.sh](./script/migration/v4-rename-if-list.sh)
- [v4.0.0](https://github.com/arkedge/c2a-core/releases/tag/v4.0.0)
  - [#167](https://github.com/arkedge/c2a-core/pull/167), [#168](https://github.com/arkedge/c2a-core/pull/168): `src/src_user/Settings` -> `src/src_user/settings`: [v4-rename-settings.sh](./script/migration/v4-rename-settings.sh) を実行
  - [#172](https://github.com/arkedge/c2a-core/pull/172): `src/src_user/settings/tlm_cmd/data_base` -> `tlm-cmd-db` の rename を実行（[v4-move-db-dir.sh](script/migration/v4-move-db-dir.sh)）


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
