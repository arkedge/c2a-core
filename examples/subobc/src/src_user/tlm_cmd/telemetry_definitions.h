/**
 * @file
 * @brief テレメトリ定義
 * @note  このコードは自動生成されています！
 * @note  コード生成元 tlm-cmd-db:
 *          repository:    github.com/arkedge/c2a-core.git
 *          CSV files MD5: 7bd311e714ca9b903ba0bfe4bb4538ea
 * @note  コード生成パラメータ:
 *          db_prefix:             SAMPLE_AOBC
 *          tlm_id_range:          [0x00, 0x100]
 *          is_cmd_prefixed_in_db: 0
 *          input_file_encoding:   utf-8
 *          output_file_encoding:  utf-8
 */
#ifndef TELEMETRY_DEFINITIONS_H_
#define TELEMETRY_DEFINITIONS_H_

typedef enum
{
  Tlm_CODE_AOBC_AOBC = 0x90,
  Tlm_CODE_AOBC_HK = 0x91,

  TLM_CODE_MAX
} TLM_CODE;

#endif
