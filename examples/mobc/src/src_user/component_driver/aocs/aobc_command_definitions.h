/**
 * @file
 * @brief コマンド定義
 * @note  このコードは自動生成されています！
 * @note  コード生成元 tlm-cmd-db:
 *          repository:     github.com/arkedge/c2a-core.git
 *          CSV files MD5:  7bd311e714ca9b903ba0bfe4bb4538ea
 *          db commit hash: 812f8f19d6afb61f5dbe8ff4412e074926afc3ac
 * @note  コード生成パラメータ:
 *          name:                    AOBC
 *          db_prefix:               SAMPLE_AOBC
 *          tlm_id_range:            [0x90, 0xc0]
 *          is_cmd_prefixed_in_db:   0
 *          input_file_encoding:     utf-8
 *          max_tlm_num:             256
 *          driver_path:             aocs/
 *          driver_type:             AOBC_Driver
 *          driver_name:             aobc_driver
 *          code_when_tlm_not_found: aobc_driver->info.comm.rx_err_code = AOBC_RX_ERR_CODE_TLM_NOT_FOUND;
 */
#ifndef AOBC_COMMAND_DEFINITIONS_H_
#define AOBC_COMMAND_DEFINITIONS_H_

typedef enum
{
  AOBC_Cmd_CODE_NOP                                               = 0x0000,  //!< ダミーコマンド
  AOBC_Cmd_CODE_TMGR_SET_TIME                                     = 0x0001,  //!< MOBC時刻設定コマンド
  AOBC_Cmd_CODE_TMGR_UPDATE_UNIXTIME                              = 0x0002,  //!< MOBC UNIXTIME修正コマンド
  AOBC_Cmd_CODE_TMGR_SET_UTL_UNIXTIME_EPOCH                       = 0x0003,  //!< UTL_cmdで用いるunixtimeの紀元を変更する (utl_unixtime_epoch [s])
  AOBC_Cmd_CODE_TMGR_SET_CYCLE_CORRECTION                         = 0x0004,  //!< CYCLES_PER_SECの補正倍率を変更する@@ 初期値は1.0
  AOBC_Cmd_CODE_TMGR_RESET_CYCLE_CORRECTION                       = 0x0005,  //!< CYCLES_PER_SECの補正倍率を1.0に初期化する
  AOBC_Cmd_CODE_TMGR_CLEAR_UNIXTIME_INFO                          = 0x0006,  //!< unixtime_info_ を初期化する
  AOBC_Cmd_CODE_AM_REGISTER_APP                                   = 0x0007,  //!< アプリ登録コマンド
  AOBC_Cmd_CODE_AM_INITIALIZE_APP                                 = 0x0008,  //!< アプリ初期化コマンド
  AOBC_Cmd_CODE_AM_EXECUTE_APP                                    = 0x0009,  //!< アプリ実行コマンド
  AOBC_Cmd_CODE_AM_SET_PAGE_FOR_TLM                               = 0x000A,  //!< テレメトリ生成用ページ番号設定
  AOBC_Cmd_CODE_AM_CLEAR_APP_INFO                                 = 0x000B,  //!< アプリ実行時間計測の初期化
  AOBC_Cmd_CODE_MM_SET_MODE_LIST                                  = 0x000C,  //!< モード定義リスト設定コマンド
  AOBC_Cmd_CODE_MM_SET_TRANSITION_TABLE                           = 0x000D,  //!< モード遷移定義テーブル書き換えコマンド
  AOBC_Cmd_CODE_MM_START_TRANSITION                               = 0x000E,  //!< モード遷移開始コマンド
  AOBC_Cmd_CODE_MM_FINISH_TRANSITION                              = 0x000F,  //!< モード遷移終了通知コマンド
  AOBC_Cmd_CODE_MM_UPDATE_TRANSITION_TABLE_FOR_TLM                = 0x0010,  //!< モードのTLMテーブル情報更新
  AOBC_Cmd_CODE_TDSP_SET_TASK_LIST                                = 0x0011,  //!< タスクリスト設定コマンド
  AOBC_Cmd_CODE_TLCD_CLEAR_ALL_TIMELINE                           = 0x0012,  //!< 全TLC登録解除
  AOBC_Cmd_CODE_TLCD_CLEAR_TIMELINE_AT                            = 0x0013,  //!< TI指定TLC登録解除
  AOBC_Cmd_CODE_TLCD_DEPLOY_BLOCK                                 = 0x0014,  //!< BLC展開 
  AOBC_Cmd_CODE_TLCD_CLEAR_ERR_LOG                                = 0x0015,  //!< 最新のコマンド実行異常記録をクリア
  AOBC_Cmd_CODE_TLCD_SET_SOE_FLAG                                 = 0x0016,  //!< 異常時実行停止機能設定(ライン番号@@ フラグ)
  AOBC_Cmd_CODE_TLCD_SET_LOUT_FLAG                                = 0x0017,  //!< 実行停止機能設定(ライン番号@@ フラグ)
  AOBC_Cmd_CODE_TLCD_SET_ID_FOR_TLM                               = 0x0018,  //!< テレメで降ろすTL_IDの設定とTLテレメの更新
  AOBC_Cmd_CODE_TLCD_SET_PAGE_FOR_TLM                             = 0x0019,  //!< テレメトリ生成用ページ番号設定
  AOBC_Cmd_CODE_CDIS_MGR_SET_IDX_FOR_TLM                          = 0x001A,  //!< テレメで降ろす CDIS の idx
  AOBC_Cmd_CODE_GENERATE_TLM                                      = 0x001B,  //!< TLMパケット生成 (deprecated)
  AOBC_Cmd_CODE_TG_GENERATE_TLM                                   = 0x001C,  //!< 自身の OBC のテレメを生成
  AOBC_Cmd_CODE_TG_GENERATE_HP_TLM                                = 0x001D,  //!< HP テレメを生成 (事実上，現在使われていない）
  AOBC_Cmd_CODE_TG_GENERATE_RT_TLM                                = 0x001E,  //!< RT テレメを生成
  AOBC_Cmd_CODE_TG_GENERATE_ST_TLM                                = 0x001F,  //!< ST テレメを生成
  AOBC_Cmd_CODE_TG_FORWARD_TLM                                    = 0x0020,  //!< sub OBC のテレメを転送
  AOBC_Cmd_CODE_TG_FORWARD_AS_HP_TLM                              = 0x0021,  //!< HP テレメとして sub OBC のテレメを転送 (事実上，現在使われていない）
  AOBC_Cmd_CODE_TG_FORWARD_AS_RT_TLM                              = 0x0022,  //!< RT テレメとして sub OBC のテレメを転送
  AOBC_Cmd_CODE_TG_FORWARD_AS_ST_TLM                              = 0x0023,  //!< ST テレメとして sub OBC のテレメを転送
  AOBC_Cmd_CODE_BCT_CLEAR_BLOCK                                   = 0x0024,  //!< BLC登録解除コマンド
  AOBC_Cmd_CODE_BCT_SET_BLOCK_POSITION                            = 0x0025,  //!< BLC操作ポインタ設定コマンド
  AOBC_Cmd_CODE_BCT_COPY_BCT                                      = 0x0026,  //!< BCT の内容を別の場所にコピー
  AOBC_Cmd_CODE_BCT_OVERWRITE_CMD                                 = 0x0027,  //!< BCTの内容を上書き
  AOBC_Cmd_CODE_BCT_FILL_NOP                                      = 0x0028,  //!< テレメ運用時の10個BCをNOP埋めする．（NOP個数）
  AOBC_Cmd_CODE_BCE_ACTIVATE_BLOCK                                = 0x0029,  //!< BLC有効化コマンド
  AOBC_Cmd_CODE_BCE_ACTIVATE_BLOCK_BY_ID                          = 0x002A,  //!< BCを（無条件に）有効化
  AOBC_Cmd_CODE_BCE_INACTIVATE_BLOCK_BY_ID                        = 0x002B,  //!< BCを（無条件に）無効化
  AOBC_Cmd_CODE_BCE_ROTATE_BLOCK                                  = 0x002C,  //!< BLC回転コマンド
  AOBC_Cmd_CODE_BCE_COMBINE_BLOCK                                 = 0x002D,  //!< BLC融合コマンド
  AOBC_Cmd_CODE_BCE_TIMELIMIT_COMBINE_BLOCK                       = 0x002E,  //!< 時間制限付きBLC融合コマンド
  AOBC_Cmd_CODE_BCE_RESET_ROTATOR_INFO                            = 0x002F,  //!< カウンタ類リセット
  AOBC_Cmd_CODE_BCE_RESET_COMBINER_INFO                           = 0x0030,  //!< カウンタ類リセット
  AOBC_Cmd_CODE_BCE_SET_ROTATE_INTERVAL                           = 0x0031,  //!< BC回転時の回転間隔周期を設定する
  AOBC_Cmd_CODE_GSCD_CLEAR_ERR_LOG                                = 0x0032,  //!< 最新のコマンド実行異常記録をクリア
  AOBC_Cmd_CODE_RTCD_CLEAR_ALL_REALTIME                           = 0x0033,  //!< 全RTC登録解除
  AOBC_Cmd_CODE_RTCD_CLEAR_ERR_LOG                                = 0x0034,  //!< 最新のコマンド実行異常記録をクリア
  AOBC_Cmd_CODE_EL_INIT                                           = 0x0035,  //!< EL初期化
  AOBC_Cmd_CODE_EL_CLEAR_LOG_ALL                                  = 0x0036,  //!< 全ログ削除
  AOBC_Cmd_CODE_EL_CLEAR_LOG_BY_ERR_LEVEL                         = 0x0037,  //!< エラーレベル指定ログ削除
  AOBC_Cmd_CODE_EL_CLEAR_STATISTICS                               = 0x0038,  //!< 統計情報削除
  AOBC_Cmd_CODE_EL_CLEAR_TLOG                                     = 0x0039,  //!< TLog削除
  AOBC_Cmd_CODE_EL_CLEAR_CLOG                                     = 0x003A,  //!< CLog削除
  AOBC_Cmd_CODE_EL_RECORD_EVENT                                   = 0x003B,  //!< イベント記録
  AOBC_Cmd_CODE_EL_TLOG_SET_PAGE_FOR_TLM                          = 0x003C,  //!< TLogのダウンリンクテレメ設定
  AOBC_Cmd_CODE_EL_CLOG_SET_PAGE_FOR_TLM                          = 0x003D,  //!< CLogのダウンリンクテレメ設定
  AOBC_Cmd_CODE_EL_INIT_LOGGING_SETTINGS                          = 0x003E,  //!< ログ設定を初期化
  AOBC_Cmd_CODE_EL_ENABLE_LOGGING                                 = 0x003F,  //!< ロギングを有効化
  AOBC_Cmd_CODE_EL_DISABLE_LOGGING                                = 0x0040,  //!< ロギングを無効化
  AOBC_Cmd_CODE_EL_ENABLE_LOGGING_ALL                             = 0x0041,  //!< すべてのロギングを有効化
  AOBC_Cmd_CODE_EL_DISABLE_LOGGING_ALL                            = 0x0042,  //!< すべてのロギングを無効化
  AOBC_Cmd_CODE_EL_ENABLE_TLOG_OVERWRITE                          = 0x0043,  //!< TLogのログ上書きを有効化
  AOBC_Cmd_CODE_EL_DISABLE_TLOG_OVERWRITE                         = 0x0044,  //!< TLogのログ上書きを無効化
  AOBC_Cmd_CODE_EL_ENABLE_TLOG_OVERWRITE_ALL                      = 0x0045,  //!< すべてのTLogのログ上書きを有効化
  AOBC_Cmd_CODE_EL_DISABLE_TLOG_OVERWRITE_ALL                     = 0x0046,  //!< すべてのTLogのログ上書きを無効化
  AOBC_Cmd_CODE_EH_INIT                                           = 0x0047,  //!< EHの初期化
  AOBC_Cmd_CODE_EH_CLEAR_ALL_RULE                                 = 0x0048,  //!< EHのすべてのルールをクリア
  AOBC_Cmd_CODE_EH_LOAD_DEFAULT_RULE                              = 0x0049,  //!< EHのデフォルトルールを読み込み
  AOBC_Cmd_CODE_EH_SET_REGISTER_RULE_EVENT_PARAM                  = 0x004A,  //!< 登録するEHルールのイベントパラメータを設定
  AOBC_Cmd_CODE_EH_SET_REGISTER_RULE_CONDITION_PARAM              = 0x004B,  //!< 登録するEHルールの対応条件パラメタを設定
  AOBC_Cmd_CODE_EH_REGISTER_RULE                                  = 0x004C,  //!< 設定したパラメタでEHルールを登録
  AOBC_Cmd_CODE_EH_DELETE_RULE                                    = 0x004D,  //!< 指定したルールを削除
  AOBC_Cmd_CODE_EH_INIT_RULE                                      = 0x004E,  //!< 指定したルールを初期化（ルールを有効化し，ルールカウンタも初期化）
  AOBC_Cmd_CODE_EH_INIT_RULE_FOR_MULTI_LEVEL                      = 0x004F,  //!< 指定したルールを初期化（ルールを有効化し，ルールカウンタも初期化）（多段EH対応用）
  AOBC_Cmd_CODE_EH_ACTIVATE_RULE                                  = 0x0050,  //!< 指定したルールを有効化
  AOBC_Cmd_CODE_EH_INACTIVATE_RULE                                = 0x0051,  //!< 指定したルールを無効化
  AOBC_Cmd_CODE_EH_ACTIVATE_RULE_FOR_MULTI_LEVEL                  = 0x0052,  //!< 指定したルールを有効化（多段EH対応用）
  AOBC_Cmd_CODE_EH_INACTIVATE_RULE_FOR_MULTI_LEVEL                = 0x0053,  //!< 指定したルールを無効化（多段EH対応用）
  AOBC_Cmd_CODE_EH_SET_RULE_COUNTER                               = 0x0054,  //!< EH_Rule の counter をセット
  AOBC_Cmd_CODE_EH_CLEAR_RULE_COUNTER                             = 0x0055,  //!< EH_Rule の counter を0 クリア
  AOBC_Cmd_CODE_EH_CLEAR_RULE_COUNTER_BY_EVENT                    = 0x0056,  //!< EH_Rule の counter を EL_Event 指定で 0 クリア
  AOBC_Cmd_CODE_EH_CLEAR_LOG                                      = 0x0057,  //!< EHの対応ログをクリア
  AOBC_Cmd_CODE_EH_SET_MAX_RESPONSE_NUM                           = 0x0058,  //!< 一度のEH実行で対応するイベントの最大数を設定
  AOBC_Cmd_CODE_EH_SET_MAX_CHECK_EVENT_NUM                        = 0x0059,  //!< 一度のEH実行でチェックするイベントの最大数を設定
  AOBC_Cmd_CODE_EH_SET_MAX_MULTI_LEVEL_NUM                        = 0x005A,  //!< 設定可能な最大段数を設定
  AOBC_Cmd_CODE_EH_SET_PAGE_OF_RULE_TABLE_FOR_TLM                 = 0x005B,  //!< EHルールテーブルのテレメページの設定
  AOBC_Cmd_CODE_EH_SET_PAGE_OF_RULE_SORTED_IDX_FOR_TLM            = 0x005C,  //!< EHルールのソート済みテーブルのテレメページの設定
  AOBC_Cmd_CODE_EH_SET_PAGE_OF_LOG_TABLE_FOR_TLM                  = 0x005D,  //!< EHログテーブルのテレメページの設定
  AOBC_Cmd_CODE_EH_SET_TARGET_ID_OF_RULE_TABLE_FOR_TLM            = 0x005E,  //!< EHテレメに表示するルールのID設定
  AOBC_Cmd_CODE_EH_MATCH_EVENT_COUNTER_TO_EL                      = 0x005F,  //!< イベント発生を検知するためのEHカウンタをELカウンタに強制定期に合わせる
  AOBC_Cmd_CODE_EH_INIT_RULE_BY_EVENT_GROUP                       = 0x0060,  //!< Cmd_EH_INIT_RULE の by_event_group コマンド．想定外のものまで操作されないか注意すること
  AOBC_Cmd_CODE_EH_INIT_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL       = 0x0061,  //!< Cmd_EH_INIT_RULE_FOR_MULTI_LEVEL の by_event_group コマンド．想定外のものまで操作されないか注意すること
  AOBC_Cmd_CODE_EH_ACTIVATE_RULE_BY_EVENT_GROUP                   = 0x0062,  //!< Cmd_EH_ACTIVATE_RULE の by_event_group コマンド．想定外のものまで操作されないか注意すること
  AOBC_Cmd_CODE_EH_INACTIVATE_RULE_BY_EVENT_GROUP                 = 0x0063,  //!< Cmd_EH_INACTIVATE_RULE の by_event_group コマンド．想定外のものまで操作されないか注意すること
  AOBC_Cmd_CODE_EH_ACTIVATE_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL   = 0x0064,  //!< Cmd_EH_ACTIVATE_RULE_FOR_MULTI_LEVEL の by_event_group コマンド．想定外のものまで操作されないか注意すること
  AOBC_Cmd_CODE_EH_INACTIVATE_RULE_BY_EVENT_GROUP_FOR_MULTI_LEVEL = 0x0065,  //!< Cmd_EH_INACTIVATE_RULE_FOR_MULTI_LEVEL の by_event_group コマンド．想定外のものまで操作されないか注意すること
  AOBC_Cmd_CODE_EVENT_UTIL_ENABLE_EH_EXEC                         = 0x0066,  //!< EHの実行を有効に
  AOBC_Cmd_CODE_EVENT_UTIL_DISABLE_EH_EXEC                        = 0x0067,  //!< EHの実行を無効に
  AOBC_Cmd_CODE_EVENT_UTIL_EXEC_EH                                = 0x0068,  //!< EHを単発で実行
  AOBC_Cmd_CODE_TF_INIT                                           = 0x0069,  //!< TFの初期化
  AOBC_Cmd_CODE_CA_INIT                                           = 0x006A,  //!< CAの初期化
  AOBC_Cmd_CODE_TF_REGISTER_TLM                                   = 0x006B,  //!< テレメトリ登録コマンド
  AOBC_Cmd_CODE_CA_REGISTER_CMD                                   = 0x006C,  //!< コマンド登録コマンド
  AOBC_Cmd_CODE_TF_SET_PAGE_FOR_TLM                               = 0x006D,  //!< テレメトリ定義テーブルページ番号設定
  AOBC_Cmd_CODE_CA_SET_PAGE_FOR_TLM                               = 0x006E,  //!< コマンド定義テーブルページ番号設定
  AOBC_Cmd_CODE_WDT_INIT                                          = 0x0100,  //!< WDT初期化．WDTがアプリイニシャライザを持たなくなったので追加
  AOBC_Cmd_CODE_WDT_ENABLE                                        = 0x0101,  //!< WDT有効化
  AOBC_Cmd_CODE_WDT_DISABLE                                       = 0x0102,  //!< WDT無効化
  AOBC_Cmd_CODE_WDT_STOP_CLEAR                                    = 0x0103,  //!< WDTのカウンタクリアを止める＝リセット実行
  AOBC_Cmd_CODE_WDT_START_CLEAR                                   = 0x0104,  //!< WDTのカウンタクリアを開始する

  AOBC_Cmd_CODE_MAX
} AOBC_CMD_CODE;

#endif
