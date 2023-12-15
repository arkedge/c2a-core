/**
 * @file
 * @brief テレメトリバッファー（テレメ中継）
 * @note  このコードは自動生成されています！
 * @note  コード生成 tlm-cmd-db:
 *          repository:     arkedge/c2a-core
 *          db commit hash: 11b08aacb01a9a39c6807928b35628e2c946369d
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
#ifndef AOBC_TELEMETRY_BUFFER_H_
#define AOBC_TELEMETRY_BUFFER_H_

#include "./aobc_telemetry_definitions.h"
#include <src_core/component_driver/driver_super.h>
#include <src_core/tlm_cmd/common_tlm_packet.h>
#include <src_core/tlm_cmd/telemetry_frame.h>

typedef struct AOBC_Driver AOBC_Driver;

#define AOBC_MAX_TLM_NUM (256)

typedef struct
{
  CommonTlmPacket packet;   //!< 最新のテレメパケットを保持
  uint8_t is_null_packet;   //!< 一度でもテレメを受信しているか？（空配列が読み出されるのを防ぐため）
} AOBC_TlmBufferElem;

typedef struct
{
  AOBC_TlmBufferElem tlm[AOBC_MAX_TLM_NUM];   //!< TLM ID ごとに保持
} AOBC_TlmBuffer;

void AOBC_init_tlm_buffer(AOBC_Driver* aobc_driver);

CDS_ERR_CODE AOBC_buffer_tlm_packet(CDS_StreamConfig* p_stream_config, AOBC_Driver* aobc_driver);

TF_TLM_FUNC_ACK AOBC_pick_up_tlm_buffer(const AOBC_Driver* aobc_driver, AOBC_TLM_CODE tlm_id, uint8_t* packet, uint16_t* len, uint16_t max_len);

#endif
