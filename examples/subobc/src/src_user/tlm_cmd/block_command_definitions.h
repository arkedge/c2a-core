/**
 * @file
 * @brief ブロックコマンド定義
 * @note  このコードは自動生成されています！
 * @note  コード生成 tlm-cmd-db:
 *          repository:    arkedge/c2a-core
 *          db hash (MD5): a68cdf10ca467fd750b1d988780fcb5b
 * @note  コード生成パラメータ:
 *          db_prefix:             SAMPLE_AOBC
 *          tlm_id_range:          [0x00, 0x100]
 *          is_cmd_prefixed_in_db: 0
 *          input_file_encoding:   utf-8
 *          output_file_encoding:  utf-8
 */
#ifndef BLOCK_COMMAND_DEFINITIONS_H_
#define BLOCK_COMMAND_DEFINITIONS_H_

// 登録されるBlockCommandTableのblock番号を規定
typedef enum
{

  // Block Cmds for Mode Transition (シーケンスリスト)
  // ./src_user/settings/modes/transitions/ で定義
  BC_SL_START_UP_TO_INITIAL = 0,
  BC_SL_NOP  = 17,

  // Block Cmds for TaskList (タスクリスト) : 286-300
  // ./src_user/settings/modes/task_lists/ で定義
  BC_TL_START_UP = 20,
  BC_TL_INITIAL = 21,

  // Block Cmds for Elements (App Rotator, Combinar)
  // ./src_user/settings/modes/task_lists/elements/ で定義
  BC_AR_DEBUG_DISPLAY_INI = 40,
  BC_AR_GS_RELATED_PROCESS = 44,
  BC_AC_TLM_CMD_HIRATE = 50,

  // ==== 各系領域 ====
  // ./C2A/tlm_cmd/normal_block_command_definition/で定義
  // イベントハンドラはBC_EH_など，接頭辞を適切につけること！

  // CDH:60-79
  BC_HK_CYCLIC_TLM = 60,
  BC_RESERVED_FOR_HK = 77,    // EM電気試験でのコマンドファイルとのバッティングを防ぐ

  // COMM:90-99

  // ==== 地上からupする“のみ”領域 ====
  // C2Aでは使用しない

  // ==== 追加領域 ====

  // BCT MAX : 382

  BC_ID_MAX    // BCT 自体のサイズは BCT_MAX_BLOCKS で規定
} BC_DEFAULT_ID;

void BC_load_defaults(void);

#endif
