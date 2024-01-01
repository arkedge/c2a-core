/**
 * @file
 * @brief テレメトリ定義
 * @note  このコードは自動生成されています！
 * @note  コード生成元 tlm-cmd-db:
 *          repository:    github.com/arkedge/c2a-core.git
 *          CSV files MD5: 3a91627656a5aa6c359adf43e76aa20e
 * @note  コード生成パラメータ:
 *          db_prefix:             SAMPLE_MOBC
 *          tlm_id_range:          [0x00, 0x100]
 *          is_cmd_prefixed_in_db: 0
 *          input_file_encoding:   utf-8
 *          output_file_encoding:  utf-8
 */
#ifndef TELEMETRY_DEFINITIONS_H_
#define TELEMETRY_DEFINITIONS_H_

typedef enum
{
  Tlm_CODE_MOBC = 0x00,
  Tlm_CODE_MEM = 0x10,
  Tlm_CODE_TLM_MGR1 = 0x18,
  Tlm_CODE_TLM_MGR2 = 0x19,
  Tlm_CODE_TL = 0x20,
  Tlm_CODE_BL = 0x21,
  Tlm_CODE_TL_DIGEST = 0x22,
  Tlm_CODE_BCT_DIGEST = 0x23,
  Tlm_CODE_CCP_DUMP = 0x24,
  Tlm_CODE_CDIS = 0x25,
  Tlm_CODE_TF = 0x26,
  Tlm_CODE_CA = 0x27,
  Tlm_CODE_DCU = 0x2a,
  Tlm_CODE_CDRV_UTIL = 0x2d,
  Tlm_CODE_CDRV_UTIL_HAL_RX_DATA = 0x2e,
  Tlm_CODE_MM = 0x40,
  Tlm_CODE_AM = 0x45,
  Tlm_CODE_APP_TIME = 0x46,
  Tlm_CODE_EL = 0x56,
  Tlm_CODE_EL_TLOG = 0x57,
  Tlm_CODE_EL_CLOG = 0x58,
  Tlm_CODE_EH = 0x5a,
  Tlm_CODE_EH_RULE = 0x5b,
  Tlm_CODE_EH_LOG = 0x5c,
  Tlm_CODE_EH_INDEX = 0x5d,
  Tlm_CODE_GS = 0x78,
  Tlm_CODE_HK = 0xf0,
  Tlm_CODE_GIT_REV = 0xf1,
  Tlm_CODE_UART_TEST = 0xf8,

  TLM_CODE_MAX
} TLM_CODE;

#endif
