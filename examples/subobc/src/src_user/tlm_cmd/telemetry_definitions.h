/**
 * @file
 * @brief テレメトリ定義
 * @note  このコードは自動生成されています！
 * @note  コード生成 db commit hash: ec4ce770d07fb3fa6824f36bd8d5253895174240
 * @note  コード生成パラメータ:
 *          path_to_src:           ../examples/subobc/src/
 *          path_to_db:            ../examples/subobc/tlm-cmd-db/
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
