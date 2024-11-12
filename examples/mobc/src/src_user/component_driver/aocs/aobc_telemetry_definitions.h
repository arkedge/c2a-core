/**
 * @file
 * @brief テレメトリ定義
 * @note  このコードは自動生成されています！
 * @note  コード生成元 tlm-cmd-db:
 *          repository:     github.com/arkedge/c2a-core.git
 *          CSV files MD5:  c3bd02684f15db9b881c871cd79547d3
 *          db commit hash: caa8d4ca59a37460e7ef348bd58ee8a95bfd092c
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
#ifndef AOBC_TELEMETRY_DEFINITIONS_H_
#define AOBC_TELEMETRY_DEFINITIONS_H_

typedef enum
{
  AOBC_Tlm_CODE_AOBC_AOBC = 0x90,
  AOBC_Tlm_CODE_AOBC_HK = 0x91,

  AOBC_TLM_CODE_MAX
} AOBC_TLM_CODE;

#endif
