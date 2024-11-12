/**
 * @file
 * @brief コマンド定義
 * @note  このコードは自動生成されています！
 * @note  コード生成元 tlm-cmd-db:
 *          repository:    github.com/arkedge/c2a-core.git
 *          CSV files MD5: 03076efb966c0f746783e116ba4fe38a
 * @note  コード生成パラメータ:
 *          db_prefix:             SAMPLE_MOBC
 *          tlm_id_range:          [0x00, 0x100]
 *          is_cmd_prefixed_in_db: 0
 *          input_file_encoding:   utf-8
 *          output_file_encoding:  utf-8
 */
#ifndef COMMAND_DEFINITIONS_H_
#define COMMAND_DEFINITIONS_H_

typedef enum
{
  Cmd_CODE_NOP                                                   = 0x0000,  //!< ダミーコマンド
  Cmd_CODE_TMGR_SET_TIME                                         = 0x0001,  //!< MOBC時刻設定コマンド
  Cmd_CODE_TMGR_UPDATE_UNIXTIME                                  = 0x0002,  //!< MOBC UNIXTIME修正コマンド
  Cmd_CODE_TMGR_SET_UTL_UNIXTIME_EPOCH                           = 0x0003,  //!< UTL_cmdで用いるunixtimeの紀元を変更する (utl_unixtime_epoch [s])
  Cmd_CODE_TMGR_SET_CYCLE_CORRECTION                             = 0x0004,  //!< CYCLES_PER_SECの補正倍率を変更する@@ 初期値は1.0
  Cmd_CODE_TMGR_RESET_CYCLE_CORRECTION                           = 0x0005,  //!< CYCLES_PER_SECの補正倍率を1.0に初期化する
  Cmd_CODE_TMGR_CLEAR_UNIXTIME_INFO                              = 0x0006,  //!< unixtime_info_ を初期化する
  Cmd_CODE_AM_REGISTER_APP                                       = 0x0007,  //!< アプリ登録コマンド
  Cmd_CODE_AM_INITIALIZE_APP                                     = 0x0008,  //!< アプリ初期化コマンド
  Cmd_CODE_AM_EXECUTE_APP                                        = 0x0009,  //!< アプリ実行コマンド
  Cmd_CODE_AM_SET_PAGE_FOR_TLM                                   = 0x000A,  //!< テレメトリ生成用ページ番号設定
  Cmd_CODE_AM_CLEAR_APP_INFO                                     = 0x000B,  //!< アプリ実行時間計測の初期化
  Cmd_CODE_MM_SET_MODE_LIST                                      = 0x000C,  //!< モード定義リスト設定コマンド
  Cmd_CODE_MM_SET_TRANSITION_TABLE                               = 0x000D,  //!< モード遷移定義テーブル書き換えコマンド
  Cmd_CODE_MM_START_TRANSITION                                   = 0x000E,  //!< モード遷移開始コマンド
  Cmd_CODE_MM_FINISH_TRANSITION                                  = 0x000F,  //!< モード遷移終了通知コマンド
  Cmd_CODE_MM_UPDATE_TRANSITION_TABLE_FOR_TLM                    = 0x0010,  //!< モードのTLMテーブル情報更新
  Cmd_CODE_TDSP_SET_TASK_LIST                                    = 0x0011,  //!< タスクリスト設定コマンド
  Cmd_CODE_TLCD_CLEAR_ALL_TIMELINE                               = 0x0012,  //!< 全TLC登録解除
  Cmd_CODE_TLCD_CLEAR_TIMELINE_AT                                = 0x0013,  //!< TI指定TLC登録解除
  Cmd_CODE_TLCD_DEPLOY_BLOCK                                     = 0x0014,  //!< BLC展開
  Cmd_CODE_TLCD_CLEAR_ERR_LOG                                    = 0x0015,  //!< 最新のコマンド実行異常記録をクリア
  Cmd_CODE_TLCD_SET_SOE_FLAG                                     = 0x0016,  //!< 異常時実行停止機能設定(ライン番号@@ フラグ)
  Cmd_CODE_TLCD_SET_LOUT_FLAG                                    = 0x0017,  //!< 実行停止機能設定(ライン番号@@ フラグ)
  Cmd_CODE_TLCD_SET_ID_FOR_TLM                                   = 0x0018,  //!< テレメで降ろすTL_IDの設定とTLテレメの更新
  Cmd_CODE_TLCD_SET_PAGE_FOR_TLM                                 = 0x0019,  //!< テレメトリ生成用ページ番号設定
  Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM                              = 0x001A,  //!< テレメで降ろす CDIS の idx
  Cmd_CODE_GENERATE_TLM                                          = 0x001B,  //!< TLMパケット生成 (deprecated)
  Cmd_CODE_TG_GENERATE_TLM                                       = 0x001C,  //!< 自身の OBC のテレメを生成
  Cmd_CODE_TG_GENERATE_HP_TLM                                    = 0x001D,  //!< HP テレメを生成 (事実上，現在使われていない）
  Cmd_CODE_TG_GENERATE_RT_TLM                                    = 0x001E,  //!< RT テレメを生成
  Cmd_CODE_TG_GENERATE_ST_TLM                                    = 0x001F,  //!< ST テレメを生成
  Cmd_CODE_TG_FORWARD_TLM                                        = 0x0020,  //!< sub OBC のテレメを転送
  Cmd_CODE_TG_FORWARD_AS_HP_TLM                                  = 0x0021,  //!< HP テレメとして sub OBC のテレメを転送 (事実上，現在使われていない）
  Cmd_CODE_TG_FORWARD_AS_RT_TLM                                  = 0x0022,  //!< RT テレメとして sub OBC のテレメを転送
  Cmd_CODE_TG_FORWARD_AS_ST_TLM                                  = 0x0023,  //!< ST テレメとして sub OBC のテレメを転送
  Cmd_CODE_BCT_CLEAR_BLOCK                                       = 0x0024,  //!< BLC登録解除コマンド
  Cmd_CODE_BCT_SET_BLOCK_POSITION                                = 0x0025,  //!< BLC操作ポインタ設定コマンド
  Cmd_CODE_BCT_COPY_BCT                                          = 0x0026,  //!< BCT の内容を別の場所にコピー
  Cmd_CODE_BCT_OVERWRITE_CMD                                     = 0x0027,  //!< BCTの内容を上書き
  Cmd_CODE_BCT_FILL_NOP                                          = 0x0028,  //!< テレメ運用時の10個BCをNOP埋めする．（NOP個数）
  Cmd_CODE_BCE_ACTIVATE_BLOCK                                    = 0x0029,  //!< BLC有効化コマンド
  Cmd_CODE_BCE_ACTIVATE_BLOCK_BY_ID                              = 0x002A,  //!< BCを（無条件に）有効化
  Cmd_CODE_BCE_INACTIVATE_BLOCK_BY_ID                            = 0x002B,  //!< BCを（無条件に）無効化
  Cmd_CODE_BCE_ROTATE_BLOCK                                      = 0x002C,  //!< BLC回転コマンド
  Cmd_CODE_BCE_COMBINE_BLOCK                                     = 0x002D,  //!< BLC融合コマンド
  Cmd_CODE_BCE_TIMELIMIT_COMBINE_BLOCK                           = 0x002E,  //!< 時間制限付きBLC融合コマンド
  Cmd_CODE_BCE_RESET_ROTATOR_INFO                                = 0x002F,  //!< カウンタ類リセット
  Cmd_CODE_BCE_RESET_COMBINER_INFO                               = 0x0030,  //!< カウンタ類リセット
  Cmd_CODE_BCE_SET_ROTATE_INTERVAL                               = 0x0031,  //!< BC回転時の回転間隔周期を設定する
  Cmd_CODE_GSCD_CLEAR_ERR_LOG                                    = 0x0032,  //!< 最新のコマンド実行異常記録をクリア
  Cmd_CODE_RTCD_CLEAR_ALL_REALTIME                               = 0x0033,  //!< 全RTC登録解除
  Cmd_CODE_RTCD_CLEAR_ERR_LOG                                    = 0x0034,  //!< 最新のコマンド実行異常記録をクリア
  Cmd_CODE_MEM_SET_REGION                                        = 0x0035,  //!< 操作対象範囲の始点アドレスと終点アドレスを指定する
  Cmd_CODE_MEM_DUMP_REGION                                       = 0x0036,  //!< 指定したメモリ領域を blob tlm としてダウンリンクする (1 tlm だけ生成)
  Cmd_CODE_MEM_DUMP_SINGLE                                       = 0x0037,  //!< 指定したメモリアドレスから，1 CTP に格納できる最大サイズ分だけダンプして blob tlm としてダウンリンクする (1 tlm だけ生成)
  Cmd_CODE_MEM_LOAD                                              = 0x0038,  //!< 指定したアドレスに書き込み（アップロード）
  Cmd_CODE_MEM_SET_DESTINATION_FOR_COPY                          = 0x0039,  //!< コピー先を設定
  Cmd_CODE_MEM_COPY_REGION                                       = 0x003A,  //!< 指定した幅で， dest に copy_rp をコピーしていく
  Cmd_CODE_EL_INIT                                               = 0x003B,  //!< EL初期化
  Cmd_CODE_EL_CLEAR_LOG_ALL                                      = 0x003C,  //!< 全ログ削除
  Cmd_CODE_EL_CLEAR_LOG_BY_ERR_LEVEL                             = 0x003D,  //!< エラーレベル指定ログ削除
  Cmd_CODE_EL_CLEAR_STATISTICS                                   = 0x003E,  //!< 統計情報削除
  Cmd_CODE_EL_CLEAR_TLOG                                         = 0x003F,  //!< TLog削除
  Cmd_CODE_EL_CLEAR_CLOG                                         = 0x0040,  //!< CLog削除
  Cmd_CODE_EL_RECORD_EVENT                                       = 0x0041,  //!< イベント記録
  Cmd_CODE_EL_TLOG_SET_PAGE_FOR_TLM                              = 0x0042,  //!< TLogのダウンリンクテレメ設定
  Cmd_CODE_EL_CLOG_SET_PAGE_FOR_TLM                              = 0x0043,  //!< CLogのダウンリンクテレメ設定
  Cmd_CODE_EL_INIT_LOGGING_SETTINGS                              = 0x0044,  //!< ログ設定を初期化
  Cmd_CODE_EL_ENABLE_LOGGING                                     = 0x0045,  //!< ロギングを有効化
  Cmd_CODE_EL_DISABLE_LOGGING                                    = 0x0046,  //!< ロギングを無効化
  Cmd_CODE_EL_ENABLE_LOGGING_ALL                                 = 0x0047,  //!< すべてのロギングを有効化
  Cmd_CODE_EL_DISABLE_LOGGING_ALL                                = 0x0048,  //!< すべてのロギングを無効化
  Cmd_CODE_EL_ENABLE_TLOG_OVERWRITE                              = 0x0049,  //!< TLogのログ上書きを有効化
  Cmd_CODE_EL_DISABLE_TLOG_OVERWRITE                             = 0x004A,  //!< TLogのログ上書きを無効化
  Cmd_CODE_EL_ENABLE_TLOG_OVERWRITE_ALL                          = 0x004B,  //!< すべてのTLogのログ上書きを有効化
  Cmd_CODE_EL_DISABLE_TLOG_OVERWRITE_ALL                         = 0x004C,  //!< すべてのTLogのログ上書きを無効化
  Cmd_CODE_EH_INIT                                               = 0x004D,  //!< EHの初期化
  Cmd_CODE_EH_CLEAR_ALL_RULE                                     = 0x004E,  //!< EHのすべてのルールをクリア
  Cmd_CODE_EH_LOAD_DEFAULT_RULE                                  = 0x004F,  //!< EHのデフォルトルールを読み込み
  Cmd_CODE_EH_SET_REGISTER_RULE_EVENT_PARAM                      = 0x0050,  //!< 登録するEHルールのイベントパラメータを設定
  Cmd_CODE_EH_SET_REGISTER_RULE_CONDITION_PARAM                  = 0x0051,  //!< 登録するEHルールの対応条件パラメタを設定
  Cmd_CODE_EH_REGISTER_RULE                                      = 0x0052,  //!< 設定したパラメタでEHルールを登録
  Cmd_CODE_EH_DELETE_RULE                                        = 0x0053,  //!< 指定したルールを削除
  Cmd_CODE_EH_INIT_RULE                                          = 0x0054,  //!< 指定したルールを初期化（ルールを有効化し，ルールカウンタも初期化）
  Cmd_CODE_EH_INIT_RULE_FOR_MULTI_LEVEL                          = 0x0055,  //!< 指定したルールを初期化（ルールを有効化し，ルールカウンタも初期化）（多段EH対応用）
  Cmd_CODE_EH_ACTIVATE_RULE                                      = 0x0056,  //!< 指定したルールを有効化
  Cmd_CODE_EH_INACTIVATE_RULE                                    = 0x0057,  //!< 指定したルールを無効化
  Cmd_CODE_EH_ACTIVATE_RULE_FOR_MULTI_LEVEL                      = 0x0058,  //!< 指定したルールを有効化（多段EH対応用）
  Cmd_CODE_EH_INACTIVATE_RULE_FOR_MULTI_LEVEL                    = 0x0059,  //!< 指定したルールを無効化（多段EH対応用）
  Cmd_CODE_EH_SET_RULE_COUNTER                                   = 0x005A,  //!< EH_Rule の counter をセット
  Cmd_CODE_EH_CLEAR_RULE_COUNTER                                 = 0x005B,  //!< EH_Rule の counter を0 クリア
  Cmd_CODE_EH_CLEAR_RULE_COUNTER_BY_EVENT                        = 0x005C,  //!< EH_Rule の counter を EL_Event 指定で 0 クリア
  Cmd_CODE_EH_CLEAR_LOG                                          = 0x005D,  //!< EHの対応ログをクリア
  Cmd_CODE_EH_SET_MAX_RESPONSE_NUM                               = 0x005E,  //!< 一度のEH実行で対応するイベントの最大数を設定
  Cmd_CODE_EH_SET_MAX_CHECK_EVENT_NUM                            = 0x005F,  //!< 一度のEH実行でチェックするイベントの最大数を設定
  Cmd_CODE_EH_SET_MAX_MULTI_LEVEL_NUM                            = 0x0060,  //!< 設定可能な最大段数を設定
  Cmd_CODE_EH_SET_PAGE_OF_RULE_TABLE_FOR_TLM                     = 0x0061,  //!< EHルールテーブルのテレメページの設定
  Cmd_CODE_EH_SET_PAGE_OF_RULE_SORTED_IDX_FOR_TLM                = 0x0062,  //!< EHルールのソート済みテーブルのテレメページの設定
  Cmd_CODE_EH_SET_PAGE_OF_LOG_TABLE_FOR_TLM                      = 0x0063,  //!< EHログテーブルのテレメページの設定
  Cmd_CODE_EH_SET_TARGET_ID_OF_RULE_TABLE_FOR_TLM                = 0x0064,  //!< EHテレメに表示するルールのID設定
  Cmd_CODE_EH_MATCH_EVENT_COUNTER_TO_EL                          = 0x0065,  //!< イベント発生を検知するためのEHカウンタをELカウンタに強制定期に合わせる
  Cmd_CODE_EH_INIT_RULE_BY_EVENT_GROUP                           = 0x0066,  //!< Cmd_EH_INIT_RULE の by_event_group コマンド．想定外のものまで操作されないか注意すること
  Cmd_CODE_EH_INIT_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL           = 0x0067,  //!< Cmd_EH_INIT_RULE_FOR_MULTI_LEVEL の by_event_group コマンド．想定外のものまで操作されないか注意すること
  Cmd_CODE_EH_ACTIVATE_RULE_BY_EVENT_GROUP                       = 0x0068,  //!< Cmd_EH_ACTIVATE_RULE の by_event_group コマンド．想定外のものまで操作されないか注意すること
  Cmd_CODE_EH_INACTIVATE_RULE_BY_EVENT_GROUP                     = 0x0069,  //!< Cmd_EH_INACTIVATE_RULE の by_event_group コマンド．想定外のものまで操作されないか注意すること
  Cmd_CODE_EH_ACTIVATE_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL       = 0x006A,  //!< Cmd_EH_ACTIVATE_RULE_FOR_MULTI_LEVEL の by_event_group コマンド．想定外のものまで操作されないか注意すること
  Cmd_CODE_EH_INACTIVATE_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL     = 0x006B,  //!< Cmd_EH_INACTIVATE_RULE_FOR_MULTI_LEVEL の by_event_group コマンド．想定外のものまで操作されないか注意すること
  Cmd_CODE_EVENT_UTIL_ENABLE_EH_EXEC                             = 0x006C,  //!< EHの実行を有効に
  Cmd_CODE_EVENT_UTIL_DISABLE_EH_EXEC                            = 0x006D,  //!< EHの実行を無効に
  Cmd_CODE_EVENT_UTIL_EXEC_EH                                    = 0x006E,  //!< EHを単発で実行
  Cmd_CODE_TF_INIT                                               = 0x006F,  //!< TFの初期化
  Cmd_CODE_CA_INIT                                               = 0x0070,  //!< CAの初期化
  Cmd_CODE_TF_REGISTER_TLM                                       = 0x0071,  //!< テレメトリ登録コマンド
  Cmd_CODE_CA_REGISTER_CMD                                       = 0x0072,  //!< コマンド登録コマンド
  Cmd_CODE_TF_SET_PAGE_FOR_TLM                                   = 0x0073,  //!< テレメトリ定義テーブルページ番号設定
  Cmd_CODE_CA_SET_PAGE_FOR_TLM                                   = 0x0074,  //!< コマンド定義テーブルページ番号設定
  Cmd_CODE_TLM_MGR_INIT                                          = 0x0075,  //!< 初期化 (DCU を使っているので，完了に最大 1.5 秒ほどかかる)
  Cmd_CODE_TLM_MGR_INIT_MASTER_AND_DEPLOY_BC                     = 0x0076,  //!< master bc の初期化
  Cmd_CODE_TLM_MGR_CLEAR_HK_TLM                                  = 0x0077,  //!< HKテレメを初期化 (DCU を使っているので，完了に最大 1 秒ほどかかる)
  Cmd_CODE_TLM_MGR_CLEAR_HIGH_FREQ_TLM                           = 0x0078,  //!< HIGH_FREQ テレメを初期化 (DCU を使っているので，完了に最大 1 秒ほどかかる)
  Cmd_CODE_TLM_MGR_CLEAR_LOW_FREQ_TLM                            = 0x0079,  //!< LOW_FREQ テレメを初期化 (DCU を使っているので，完了に最大 1 秒ほどかかる)
  Cmd_CODE_TLM_MGR_CLEAR_USER_TLM                                = 0x007A,  //!< high_freq@@ low_freq を初期化 (DCU を使っているので，完了に最大 1 秒ほどかかる)
  Cmd_CODE_TLM_MGR_START_TLM                                     = 0x007B,  //!< TLM送出開始
  Cmd_CODE_TLM_MGR_STOP_TLM                                      = 0x007C,  //!< TLM送出一時停止（事実上，TLM送出用TLをクリア）
  Cmd_CODE_TLM_MGR_REGISTER_GENERATE_RT_TLM                      = 0x007D,  //!< TG_GENERATE_RT_TLM を登録
  Cmd_CODE_TLM_MGR_REGISTER_GENERATE_ST_TLM                      = 0x007E,  //!< TG_GENERATE_ST_TLM を登録
  Cmd_CODE_TLM_MGR_REGISTER_FORWARD_AS_RT_TLM                    = 0x007F,  //!< TG_GENERATE_RT_TLM を登録
  Cmd_CODE_TLM_MGR_REGISTER_FORWARD_AS_ST_TLM                    = 0x0080,  //!< TG_GENERATE_ST_TLM を登録
  Cmd_CODE_TLM_MGR_REGISTER_REPLAY_TLM                           = 0x0081,  //!< DR_REPLAY_TLM を登録
  Cmd_CODE_TLM_MGR_DELETE_GENERATE_RT_TLM                        = 0x0082,  //!< TG_GENERATE_RT_TLM を削除
  Cmd_CODE_TLM_MGR_DELETE_GENERATE_ST_TLM                        = 0x0083,  //!< TG_GENERATE_ST_TLM を削除
  Cmd_CODE_TLM_MGR_DELETE_FORWARD_AS_RT_TLM                      = 0x0084,  //!< TG_GENERATE_RT_TLM を削除
  Cmd_CODE_TLM_MGR_DELETE_FORWARD_AS_ST_TLM                      = 0x0085,  //!< TG_GENERATE_ST_TLM を削除
  Cmd_CODE_TLM_MGR_DELETE_REPLAY_TLM                             = 0x0086,  //!< DR_REPLAY_TLM を削除
  Cmd_CODE_DCU_ABORT_CMD                                         = 0x0087,  //!< 実行中の分割コマンドを停止する
  Cmd_CODE_DCU_DOWN_ABORT_FLAG                                   = 0x0088,  //!< エラー，またはコマンドによって中断ステータスとなっているコマンドを，実行可能状態に戻す
  Cmd_CODE_DCU_CLEAR_LOG                                         = 0x0089,  //!< ログをクリアする
  Cmd_CODE_CCP_DUMP_CDIS                                         = 0x008A,  //!< 指定した CDIS の Cmd キューの CCP をダンプする
  Cmd_CODE_CCP_DUMP_BCT                                          = 0x008B,  //!< 指定した BCT の CCP をダンプする
  Cmd_CODE_TL_BCT_DIGEST_TL                                      = 0x008C,  //!< 指定した TLCD ID と page no の TL の digest を計算する
  Cmd_CODE_TL_BCT_DIGEST_BCT                                     = 0x008D,  //!< 指定した block の BCT の digest を計算する
  Cmd_CODE_CDRV_UTIL_CDSC_ENABLE_MONITOR_FOR_RX_DISRUPTION       = 0x008E,  //!< CDS の受信途絶検出を有効化
  Cmd_CODE_CDRV_UTIL_CDSC_DISABLE_MONITOR_FOR_RX_DISRUPTION      = 0x008F,  //!< CDS の受信途絶検出を無効化
  Cmd_CODE_CDRV_UTIL_CDSC_SET_TIME_THRESHOLD_FOR_RX_DISRUPTION   = 0x0090,  //!< CDS の受信途絶検出の時間閾値を設定
  Cmd_CODE_CDRV_UTIL_CDSSC_ENABLE                                = 0x0091,  //!< CDS の stream を有効化
  Cmd_CODE_CDRV_UTIL_CDSSC_DISABLE                               = 0x0092,  //!< CDS の stream を無効化
  Cmd_CODE_CDRV_UTIL_CDSSC_ENABLE_MONITOR_FOR_TLM_DISRUPTION     = 0x0093,  //!< CDS のテレメ途絶検出を有効化
  Cmd_CODE_CDRV_UTIL_CDSSC_DISABLE_MONITOR_FOR_TLM_DISRUPTION    = 0x0094,  //!< CDS のテレメ途絶検出を無効化
  Cmd_CODE_CDRV_UTIL_CDSSC_SET_TIME_THRESHOLD_FOR_TLM_DISRUPTION = 0x0095,  //!< CDS のテレメ途絶検出の時間閾値を設定
  Cmd_CODE_CDRV_UTIL_HAL_INIT                                    = 0x0096,  //!< CDS の HAL を初期化
  Cmd_CODE_CDRV_UTIL_HAL_RX                                      = 0x0097,  //!< CDS の HAL で任意データ受信
  Cmd_CODE_CDRV_UTIL_HAL_TX                                      = 0x0098,  //!< CDS の HAL で任意データ送信
  Cmd_CODE_CDRV_UTIL_HAL_REOPEN                                  = 0x0099,  //!< CDS の HAL を reopen
  Cmd_CODE_CDRV_UTIL_HAL_RX_TO_RAM                               = 0x009A,  //!< CDS の HAL で任意データ受信 (RAMに)
  Cmd_CODE_CDRV_UTIL_HAL_TX_FROM_RAM                             = 0x009B,  //!< CDS の HAL で任意データ送信 (RAMから)
  Cmd_CODE_CDRV_UTIL_HAL_CLEAR_RX_BUFFER                         = 0x009C,  //!< CDS の HAL の任意データ受信バッファをクリア
  Cmd_CODE_CDRV_UTIL_SET_TLM                                     = 0x009D,  //!< 各 Component Driver の CDS のメンバをテレメでダウンリンクする設定
  Cmd_CODE_TEST_CCP_REGISTER_TLC_ASAP                            = 0x009E,  //!< CCP_register_tlc_asap  のテスト用コマンド
  Cmd_CODE_TEST_CCP_GET_RAW_PARAM_INFO                           = 0x009F,  //!< CCP UTIL の ParamGenerator のテスト
  Cmd_CODE_CSRV_GS_DRIVER_RESET                                  = 0x0100,  //!< GS ドライバの初期化
  Cmd_CODE_CSRV_GS_SET_RT_FLUSH_INTERVAL                         = 0x0101,  //!< RT AOSTFの排出間隔設定
  Cmd_CODE_CSRV_GS_SET_RP_FLUSH_INTERVAL                         = 0x0102,  //!< RP AOSTFの排出間隔設定
  Cmd_CODE_CSRV_GS_SET_FARM_PW                                   = 0x0103,  //!< FARM-1のPositive Window Width設定
  Cmd_CODE_CSRV_GS_SET_INFO                                      = 0x0104,  //!< GS送信TLMが0ならCCSDSに@@ 1ならUARTの情報が出てくる
  Cmd_CODE_CSRV_GS_CCSDS_GET_BUFFER                              = 0x0105,
  Cmd_CODE_CSRV_GS_CCSDS_SET_RATE                                = 0x0106,  //!< 40M/パラメータ が bps になる．初期値は 0xAD で 230.4 Kbps (微妙に対応していない)
  Cmd_CODE_CSRV_GS_UART_TLM_ON                                   = 0x0107,  //!< TLM の UART への送出を行う
  Cmd_CODE_CSRV_GS_UART_TLM_OFF                                  = 0x0108,  //!< TLM の UART への送出を止める
  Cmd_CODE_WDT_INIT                                              = 0x0109,  //!< WDT初期化．WDTがアプリイニシャライザを持たなくなったので追加
  Cmd_CODE_WDT_ENABLE                                            = 0x010A,  //!< WDT有効化
  Cmd_CODE_WDT_DISABLE                                           = 0x010B,  //!< WDT無効化
  Cmd_CODE_WDT_STOP_CLEAR                                        = 0x010C,  //!< WDTのカウンタクリアを止める＝リセット実行
  Cmd_CODE_WDT_START_CLEAR                                       = 0x010D,  //!< WDTのカウンタクリアを開始する
  Cmd_CODE_WDT_SET_TIMER                                         = 0x010E,  //!< WDTの監視周期を変更 (監視周期[ms])
  Cmd_CODE_UART_TEST_INIT_CSRV                                   = 0x010F,  //!< CSRV 初期化
  Cmd_CODE_UART_TEST_UPDATE                                      = 0x0110,  //!< Driver update
  Cmd_CODE_UART_TEST_SEND_TEST                                   = 0x0111,  //!< Driver send test
  Cmd_CODE_UTIL_CMD_ADD                                          = 0x03E0,  //!< 汎用コマンドのバッファに指定サイズのデータを入れる
  Cmd_CODE_UTIL_CMD_SEND                                         = 0x03E1,  //!< 引数のCHに汎用コマンドのバッファにあるデータを送信する
  Cmd_CODE_UTIL_CMD_RESET                                        = 0x03E2,  //!< 汎用コマンドのバッファをクリアする

  Cmd_CODE_MAX
} CMD_CODE;

#endif
