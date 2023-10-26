# Parameter Settings
C2Aのユーザーオーバーライド可能なパラメタ設定について．  
各定義の説明は `詳細説明` のリンクのあるコードを参照すること．

NOTE:  
メモリ使用量は，基本的にはそれぞれの環境でビルドして，memmapを確認するべき．  
それによって，（core, userに関わらず）メモリ使用状況が正確にわかり，適切な削減ができるようになる．

NOTE: 構造体にはパディングが入る場合があり，実際にはここで示した値よりも大きくなることが多い．

FIXME: 2021/12/04現在，だいぶ情報が古いので更新する．

## 目次
- [C2Aのメモリ使用量に大きく影響する設定](#c2aのメモリ使用量に大きく影響する設定)
    - [block_command_table_params](#block_command_table_params)
    - [packet_handler_params](#packethandlerparams)
    - [driver_super_params](#driver_super_params)
- [メモリ使用量にあまり影響しない設定](#メモリ使用量にあまり影響しない設定)
    - [command_analyze_params](#command_analyze_params)
    - [telemetry_frame_params](#telemetry_frame_params)
    - [event_logger_params](#event_logger_params)
    - [event_handler_params](#event_handler_params)
    - [app_manager_params](#app_manager_params)
    - [obc_time_params](#obc_time_params)


## C2Aのメモリ使用量に大きく影響する設定
### block_command_table_params
C2Aでもっともメモリサイズを消費するであろう箇所．  
コマンドのポインタデータ `BlockCommandTable` のサイズと実行情報のデータ `BlockCommandExecutor` のサイズ，コマンド，パラメーター実体 `BCT_Table`，`BCE_Params` のサイズを規定．

現時点では`BlockCommandTable`のサイズはおおよそ以下の通り.
```
  (a + b) * BCT_MAX_BLOCKS + c

  a: sizeof(BCT_Table*)
  b: sizeof(BCT_Func)
  c: 2 * sizeof(BCT_Pos)
```
`BlockCommandExecutor` のサイズは
```
  (a + b) * BCT_MAX_BLOCKS

  a: sizeof(BCE_Params*)
  b: sizeof(BCE_Func)
```
各実体 `BCT_Table`，`BCE_Params` のサイズは
```
  BCT_Table:
    (a + b) * BCT_MAX_BLOCKS

    a: 1, b: BC_CMD_MAX_LENGTH * BC_MAX_CMD_NUM
  BCE_Params:
    a * BCT_MAX_BLOCKS

    a: sizeof(BCE_Params)
```
NOTE: おおよそと言っているのは，structのpaddingサイズはコンパイラ依存であるため．

たとえば，以下の設定ではおよそ 200 KB．
```cpp
#define BC_CMD_MAX_LENGTH (64)
#define BC_MAX_CMD_NUM    (32)
#define BCT_MAX_BLOCKS    (100)
```

注意点:  
`BCT_CMD_MAX_LENGTH` 以上のサイズのコマンドは BCT に保存できなくなる．  
これは， BCT を予め static にメモリ確保するためである．  
したがって，最大コマンド長に合わせるか，メモリ使用量と対応コマンド種類のトレードオフをとることになる．

NOTE:  
コマンドの情報自体は `/src_core/tlm_cmd/block_command_table.h` に，BC の実行パラメーター，状態などは `block_command_executor.h` に保存されている．コマンドの最大サイズやフォーマットなどの定義は user 依存である．
更に言えばコマンドの実体自体は `/src_user/tlm_cmd/block_command_user_settings.c` で定義されているため実体をどこに置くかは user 側で自由に定義出来る．
そのため例えば普通の揮発メモリ上だけでなく不揮発メモリ上やFLASH上に載せることも可能．  
IFはCommonTlmCmdPacket.hなどで策定される．  
（現状，coreとuserの分離が雑なので，近いうちに改良予定）

NOTE: `sizeof(bct_id_t)` は `SIZE_OF_BCT_ID_T` にて設定可能である．

#### 定義
- core: [`/src_core/tlm_cmd/block_command_table.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/block_command_table.h)，[`/src_core/tlm_cmd/block_command_executor.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/block_command_executor.h)
- user: `/src_user/settings/tlm_cmd/block_command_table_params.h`，`/src_user/tlm_cmd/block_command_user_settings.h`

#### 詳細説明
[`/src_core/tlm_cmd/block_command_table.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/block_command_table.h)，[`/src_core/tlm_cmd/block_command_executor.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/block_command_executor.h) を参照．


- - -
### packet_handler_params
各種コマンド，テレメのキューサイズを決める．  
それぞれのキューにいくつのコマンド，テレメを保持することができるかを制御する．
コマンドキューサイズは `HOGE_CMD_LIST_MAX` であり，テレメキューサイズは  `HOGE_TLM_LIST_MAX`  である．

また，そのキューのテレメのためのページネーションを定義する．  
ページネーションはテレメサイズと相談して決める．

なお，現在のC2A CoreはTLが3本ある．
将来的には可変予定だが，現時点で4本以上にしたい場合はuser側でUserPacketHandlerに追加CDISを定義すること．

現時点では，各キューのサイズはおおよそ，以下Byte．
```
  sizeof(PL_Node) * HOGE_LIST_MAX

  sizeof(PL_Node): struct pointer size + sizeof(CTCP)
  CTCP: ユーザー定義型（デフォルトではTCPで，サイズはTCP_MAX_LEN）
```
NOTE: 構造体ポインタサイズは環境依存．

たとえば，以下の設定，環境では，およそ6.8 KB．
```cpp
#define HOGE_LIST_MAX    (16) 
#define TCP_MAX_LEN      (432u)
struct pointer size:     4 byte
```

#### 定義
- core: [`/src_core/tlm_cmd/PacketHandler.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/PacketHandler.h)
- user: `/src_user/settings/tlm_cmd/packet_handler_params.h`

#### 詳細説明
[`/src_core/tlm_cmd/PacketHandler.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/PacketHandler.h) を参照．


- - -
### driver_super_params
DriverSuperのストリーム数とバッファ数を制御する．

ストリーム数がNとは，１つのDriverで１つの通信先コンポーネントとやりとりする際，その間を流れるビットストリームの解釈をN種類並列で処理できる，ということである．  
（という，汎用機能を提供しているだけで，使い方は自由．EQUでいう，定期・非定期テレメを実現したいなら２つのストリームを使えば良い）

この値によって影響を受けるメモリ使用量は，現時点では以下Byte．
```
  (DRIVER_SUPER_STREAM_MAX * (DRIVER_SUPER_RX_FRAME_SIZE_MAX + DRIVER_SUPER_RX_BUFFER_SIZE_MAX) + DRIVER_SUPER_RX_BUFFER_SIZE_MAX)
        * DriverSuperを継承したDriver数 + 2 * DRIVER_SUPER_RX_BUFFER_SIZE_MAX
```
NOTE: この値が関わる部分を抽出しているため，Driverの他の機能のためのメモリは含まない．

たとえば，以下の設定，環境では，およそ30 KB．
```cpp
#define DRIVER_SUPER_STREAM_MAX          (3)
#define DRIVER_SUPER_RX_BUFFER_SIZE_MAX  (1024)
#define DRIVER_SUPER_RX_FRAME_SIZE_MAX   (1024)
DriverSuperを継承したDriver数:           4
```

#### 定義
- core: [`/src_core/component_driver/driver_super.h`](https://github.com/arkedge/c2a-core/blob/develop/component_driver/driver_super.h)
- user: `/src_user/settings/DriverSuper/driver_super_params.h`

#### 詳細説明
[`/src_core/driver/Super/DriverSuper.h`](https://github.com/arkedge/c2a-core/blob/develop/driver/driver_super.h) を参照．


## メモリ使用量にあまり影響しない設定
### command_analyze_params
最大で何個のコマンドを登録できるかを制御する．  
また，そのコマンドテーブルのテレメのためのページネーションを定義する．  
ページネーションはテレメサイズと相談して決める．

メモリ使用量は，現時点では以下Byte．
```
  sizeof(int (*cmd_func)(const CTCP*)) * CMD_MAX_CMDS

  CTCP: ユーザー定義型（デフォルトではTCP）
```
NOTE: つまり関数ポインタのサイズ次第（コンパイラ依存）

#### 定義
- core: [`/src_core/tlm_cmd/CommandAnalyze.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/CommandAnalyze.h)
- user: `/src_user/settings/tlm_cmd/command_analyze_params.h`

#### 詳細説明
[`/src_core/tlm_cmd/CommandAnalyze.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/CommandAnalyze.h) を参照．


- - -
### telemetry_frame_params
最大で何個のテレメを登録できるかを制御する．  
また，そのテレメテーブルのテレメのためのページネーションを定義する．  
ページネーションはテレメサイズと相談して決める．

メモリ使用量は，現時点では以下Byte．
```
  sizeof(int (*tlm_func)(unsigned char*, int)) * TLM_MAX_TLMS
```
NOTE: つまり関数ポインタのサイズ次第（コンパイラ依存）

#### 定義
- core: [`/src_core/tlm_cmd/TelemetryFrame.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/TelemetryFrame.h)
- user: `/src_user/settings/tlm_cmd/telemetry_frame_params.h`

#### 詳細説明
[`/src_core/tlm_cmd/TelemetryFrame.h`](https://github.com/arkedge/c2a-core/blob/develop/tlm_cmd/TelemetryFrame.h) を参照．


- - -
### event_logger_params
Event Logger の各種テーブルサイズなどを制御する．  
Event Logger はパラメタが多いいので，ここではTLog，CLogが共に有効であることを仮定する．

まず， EL_Event 構造体のサイズは以下Byte．
```
- EL_IS_ENABLE_EVENT_NOTE が定義されている場合
  2 * sizeof(enum) + sizeof(ObcTime) + 8

- EL_IS_ENABLE_EVENT_NOTE が定義されていない場合
  2 * sizeof(enum) + sizeof(ObcTime) + 4

  sizeof(ObcTime): 時刻に関わり，完全に環境依存だが，3 * sizeof(size_t) となる環境が多いのでは？
```
NOTE: enum のサイズ次第（コンパイラ依存）

TLog のテーブルサイズはそれぞれの `EL_ERROR_LEVEL` に対して，以下Byte.
```
  sizeof(EL_Event) * EL_TLOG_LOG_SIZE_MAX_${EL_ERROR_LEVEL} + 6

  EL_TLOG_LOG_SIZE_MAX_${EL_ERROR_LEVEL}: EL_TLOG_TLM_PAGE_SIZE * EL_TLOG_TLM_PAGE_MAX_${EL_ERROR_LEVEL}
```
NOTE: 合計サイズは全 `EL_ERROR_LEVEL` の合計値

CLog のテーブルサイズはそれぞれの `EL_ERROR_LEVEL` に対して，以下Byte.
```
  (2 + sizeof(EL_Event) + sizeof(el_clog_counter_t) + sizeof(ObcTime)) * EL_CLOG_LOG_SIZE_MAX_${EL_ERROR_LEVEL} + 2

  sizeof(el_clog_counter_t): SIZE_OF_EL_CLOG_COUNTER_T
  EL_CLOG_LOG_SIZE_MAX_${EL_ERROR_LEVEL}: EL_CLOG_TLM_PAGE_SIZE * EL_CLOG_TLM_PAGE_MAX_${EL_ERROR_LEVEL}
```
NOTE: 合計サイズは全 `EL_ERROR_LEVEL` の合計値

EL_Event の有効かどうかのテーブルサイズは以下Byte.
```
  EL_GROUP_MAX / 8 + 1
```

Event Logger 全体のメモリ使用量は以下Byte.
```
  すべての `EL_ERROR_LEVEL` に対する TLog, CLog テーブルサイズ + EL_Event の有効かどうかのテーブルサイズ
      + 4 * EL_ERROR_LEVEL_MAX + 2 * sizeof(enum) + sizeof(EL_Event) + 6

  EL_ERROR_LEVEL_MAX: `EL_ERROR_LEVEL` の数
```

#### 定義
- core: [`/src_core/system/event_manager/event_logger.h`](https://github.com/arkedge/c2a-core/blob/develop/system/event_manager/event_logger.h)
- user: `/src_user/settings/system/event_handler_params.h`

#### 詳細説明
[`/src_core/system/event_manager/event_logger.h`](https://github.com/arkedge/c2a-core/blob/develop/system/event_manager/event_logger.h) を参照．


- - -
### event_handler_params
TBW


- - -
### app_manager_params
AppManagerのAppInfo保存テーブルサイズを制御する．  
すなわち，ここで登録できる最大アプリ数を規定する．

この値によって影響を受けるメモリ使用量は，現時点では以下Byte．
```
  sizeof(AppInfo) * AM_MAX_APPS

  sizeof(AppInfo): 時刻やポインタサイズに関わり完全に環境依存．
                   典型的なマイコンであれば， 6 * sizeof(size_t) + 1 程度か．
```
NOTE: この値が関わる部分を抽出しているため，AppManagerの他の機能のためのメモリは含まない．

#### 定義
- core: [`/src_core/system/application_manager/AppManager.h`](https://github.com/arkedge/c2a-core/blob/develop/system/application_manager/AppManager.h)
- user: `/src_user/settings/system/app_manager_params.h`

#### 詳細説明
[`/src_core/system/application_manager/AppManager.h`](https://github.com/arkedge/c2a-core/blob/develop/system/application_manager/AppManager.h) を参照．


- - -
### obc_time_params
OBCのTIについての設定．  
cycleやstepについてはTimeManager (documentはまだ書いてない....) 参照のこと．

NOTE: メモリ使用量にほぼ関係なし

#### 定義
- core: [`/src_core/system/time_manager/obc_time.h`](https://github.com/arkedge/c2a-core/blob/develop/system/time_manager/obc_time.h)
- user: `/src_user/settings/system/obc_time_params.h`

#### 詳細説明
[`/src_core/system/time_manager/obc_time.h`](https://github.com/arkedge/c2a-core/blob/develop/system/time_manager/obc_time.h) を参照．










