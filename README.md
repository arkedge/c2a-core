# Core of Command Centric Architecture

## C2A
- Command Centric Architecture
- OBC 搭載フライトソフトウェアフレームワーク
- 本リポジトリは， ArkEdge Space Inc. による C2A の先行開発のため，[ut-issl/c2a-core v3.10.0](https://github.com/ut-issl/c2a-core/releases/tag/v3.10.0) から fork したもの
- v3 系列の開発は [ut-issl/c2a-core](https://github.com/ut-issl/c2a-core) で引き続き行われる

### C2A core
- 各 C2A で共通利用される C2A の中核部のコード
- 基本的には，各々の C2A user repository （サンプル： TBW） にて， git submodule で参照される．
- core 開発のための最小限の user 部は [`Examples`](./Examples) にあり，これを用いてビルドし，検証する．


## ドキュメント
- ドキュメント:  https://github.com/arkedge/c2a-core/tree/develop/Docs
- リファレンス (TBD):  https://github.com/ut-issl/c2a-reference


## 開発
### セットアップ
1. clone 後， [`setup.bat`](./setup.bat) or [`setup.sh`](./setup.sh) を実行

### Kanban (開発マイルストーンなど)
- https://github.com/orgs/arkedge/projects/43/views/1

### SILS (Software-In-the-Loop-Simulation) 環境
- C2A は， [S2E](https://github.com/ut-issl/s2e-core) などの SILS を用いて，フライトソフトウェアがそのまま実行される SILS を構築できる．
- つまり，ターゲットの OBC 上で動くソフトウェアが， PC などの上で，そのままエミュレートできる．
- [`Examples/minimum_user`](./Examples/minimum_user) にある， 最小限の C2A 実行サンプルは， [S2E User for C2A Core](https://github.com/ut-issl/s2e-user-for-c2a-core) によってエミュレーション可能である．
    - また， [`Examples/2nd_obc_user`](./Examples/2nd_obc_user) にある 2nd OBC (非 MOBC) の user 部を使うことで， C2A 間通信も模擬できる．

### テスト
- 特定の user を仮定しないと，各種パラメタが確定しないため， [`Examples/minimum_user`](./Examples/minimum_user) でテストする．
- C2A 間通信に関連する部分のテストのみは [`Examples/2nd_obc_user`](./Examples/2nd_obc_user) を用いる．
- 詳細は [Test](./Examples/minimum_user/src/src_user/Test) 参照．

### ブランチ
- `main`: リリース版（[詳細](./Docs/General/release.md)）
- `develop`: 概ね検証された最新版（beta 機能含む）
- `feature/*` : 開発ブランチ
- `hotfix/*` : 重大バグ修正用ブランチ


## 採用実績・動作実績
C2A Core の採用実績のある衛星 OBC や動作実績のあるボードの情報をまとめる．

| Name | Satellite | &nbsp;&nbsp;&nbsp;Lead&nbsp;Institution&nbsp;&nbsp;&nbsp; | Launch / Deploy | CPU | &nbsp;&nbsp;&nbsp;Clock&nbsp;&nbsp;&nbsp; | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ROM&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;RAM&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;NVRAM&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Storage&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Interface&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; | Reference |
| -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- | -- |
| OBC (2U) | MAGNARO (Tigris) | Inamori Lab. at Nagoya University | 2022/10/12 | STMicroelectronics STM32F4 | 90 MHz | 2 MiB internal ROM | 384 KiB internal RAM, 500 KiB external SRAM | 524 KiB MRAM, 131 KiB EEPROM | 16 GB SD card | UART, SPI, I2C, GPIO, ADC, DCMI | [^1] |
| OBC (1U) | MAGNARO (Piscis) | Inamori Lab. at Nagoya University | 2022/10/12 | STMicroelectronics STM32F4 | 45 MHz | 2 MiB internal ROM | 384 KiB internal RAM, 500 KiB external SRAM | 524 KiB MRAM, 131 KiB EEPROM | 16 GB SD card | UART, SPI, I2C, GPIO, ADC, DCMI | [^1] |
| MOBC | SPHERE-1 EYE | Sony Group Corporation, ISSL at the University of Tokyo | 2023/01/03 | Renesas Electronics SH-2A | 200 MHz | 2.5 MiB internal ROM | 128 KiB internal RAM, 8 MiB external SRAM | 2 MiB MRAM | 2 GiB NAND flash memory | UART (RS422, LVTTL), CCSDS (LVTTL), GPIO (LVTTL), ADC | |
| AOBC | SPHERE-1 EYE | Sony Group Corporation, ISSL at the University of Tokyo | 2023/01/03 | Microchip Technology PIC32MX7 | 80 MHz | 512 KiB internal ROM | 128 KiB internal RAM | 512 KiB FRAM | None | UART (RS422, RS485, LVTTL), SPI, I2C, GPIO (LVTTL), ADC | |
| TOBC | SPHERE-1 EYE | Sony Group Corporation, ISSL at the University of Tokyo | 2023/01/03 | Microchip Technology PIC32MX7 | 30 MHz | 512 KiB internal ROM | 128 KiB internal RAM | None | None | UART (LVTTL), I2C, GPIO (LVTTL), ADC | |
| AOBC | OPTIMAL-1 | ArkEdge Space Inc. | 2023/01/06 | | | | | | | | |
| MOBC | ONGLAISAT | ISSL at the University of Tokyo | - | Renesas Electronics SH-2A | 200 MHz | 2.5 MiB internal ROM | 128 KiB internal RAM, 8 MiB external SRAM | 2 MiB MRAM | 2 GiB NAND flash memory | UART (RS422, LVTTL), CCSDS (LVTTL), GPIO (LVTTL), ADC | [^1] |
| AOBC | ONGLAISAT | ISSL at the University of Tokyo | - | Microchip Technology PIC32MX7 | 80 MHz | 512 KiB internal ROM | 128 KiB internal RAM | 512 KiB FRAM | None | UART (RS422, RS485, LVTTL), SPI, I2C, GPIO (LVTTL), ADC | [^1] |
| TOBC | ONGLAISAT | ISSL at the University of Tokyo | - | Microchip Technology PIC32MX7 | 30 MHz | 512 KiB internal ROM | 128 KiB internal RAM | None | None | UART (LVTTL), I2C, GPIO (LVTTL), ADC | [^1] |


[^1]: Ryo Suzumoto, et al. Improvement of C2A (Command-Centric Architecture) Reusability for Multiple Types of OBCs and Development of Continuous Integration Environment for Reliability of Flight Software. _33rd International Symposium on Space Technology and Science_, 2022-f-58, 2022.


## 関連リンク
### C2A 関連ツール
- https://github.com/ut-issl/tlm-cmd-db
- https://github.com/ut-issl/c2a-tlm-cmd-code-generator
- https://github.com/ut-issl/python-wings-interface
- https://github.com/ut-issl/c2a-enum-loader
- https://github.com/ut-issl/s2e-user-for-c2a-core
- https://github.com/arkedge/c2a-tlmcmddb
- https://github.com/arkedge/workflows-c2a


### User 実装例
- https://github.com/arkedge/c2a-user-for-raspi


### 関連 crate
- https://crates.io/crates/c2a-core （本リポジトリを crate 化したもの）
- https://crates.io/crates/tlmcmddb
- https://crates.io/crates/tlmcmddb-cli
- https://crates.io/crates/tlmcmddb-csv
