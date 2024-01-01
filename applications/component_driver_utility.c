#pragma section REPRO
/**
 * @file
 * @brief 様々な Component Driver に統一的な Cmd を提供する
 */
#include "component_driver_utility.h"
#include "../tlm_cmd/common_cmd_packet_util.h"
#include "../component_driver/component_driver.h"
#include "../component_driver/driver_super.h"
#include <stddef.h>
#include <string.h>

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT CDRV_UTIL_init_(void);

/**
 * @brief  CDS_hal_rx の wrapper
 * @param[in]  p_super:     ComponentDriverSuper 構造体へのポインタ
 * @param[out] buffer:      受信データ格納先へのポインタ
 * @param[in]  buffer_size: 受信データ格納先のデータサイズ
 * @return CCP_CmdRet
 */
static CCP_CmdRet CDRV_UTIL_hal_rx_(ComponentDriverSuper* cds, uint8_t* buffer, int buffer_size);

/**
 * @brief  CDS_hal_tx の wrapper
 * @param[in]  p_super:     ComponentDriverSuper 構造体へのポインタ
 * @param[in]  buffer:      送信データ格納先へのポインタ
 * @param[in]  buffer_size: 送信データサイズ
 * @return CCP_CmdRet
 */
static CCP_CmdRet CDRV_UTIL_hal_tx_(ComponentDriverSuper* cds, const uint8_t* data, int data_size);

/**
 * @brief  hal handler の返り値 int を uint32_t に "適当に" 変換する
 * @note   hal handler の返り値は user 依存なので，あくまで適当な変換であることに注意する
 * @param  hal_handler_ret: hal handler の返り値
 * @return uint32_t に変換された値
 */
static uint32_t CDRV_UTIL_conv_hal_handler_ret_to_u32_(int hal_handler_ret);

static ComponentDriverUtility component_driver_utility_;
const ComponentDriverUtility* const component_driver_utility = &component_driver_utility_;


AppInfo CDRV_UTIL_create_app(void)
{
  return AI_create_app_info("component_driver_utility", CDRV_UTIL_init_, NULL);
}


static RESULT CDRV_UTIL_init_(void)
{
  memset(&component_driver_utility_, 0x00, sizeof(ComponentDriverUtility));
  return RESULT_OK;
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_ENABLE_MONITOR_FOR_RX_DISRUPTION(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);

  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSC_enable_monitor_for_rx_disruption(cds);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_DISABLE_MONITOR_FOR_RX_DISRUPTION(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);

  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSC_disable_monitor_for_rx_disruption(cds);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_SET_TIME_THRESHOLD_FOR_RX_DISRUPTION(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint32_t time_threshold = CCP_get_param_from_packet(packet, 1, uint32_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);

  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSC_set_time_threshold_for_rx_disruption(cds, time_threshold);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_ENABLE(const CommonCmdPacket* packet)
{
  CDS_StreamConfig* cdssc;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t cds_stream = CCP_get_param_from_packet(packet, 1, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (cds_stream >= CDS_STREAM_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);

  cdssc = (CDS_StreamConfig*)CDRV_get_cdssc(cdrv_id, cds_stream); // const_cast
  if (cdssc == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSSC_enable(cdssc);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_DISABLE(const CommonCmdPacket* packet)
{
  CDS_StreamConfig* cdssc;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t cds_stream = CCP_get_param_from_packet(packet, 1, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (cds_stream >= CDS_STREAM_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);

  cdssc = (CDS_StreamConfig*)CDRV_get_cdssc(cdrv_id, cds_stream); // const_cast
  if (cdssc == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSSC_disable(cdssc);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_ENABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet)
{
  CDS_StreamConfig* cdssc;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t cds_stream = CCP_get_param_from_packet(packet, 1, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (cds_stream >= CDS_STREAM_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);

  cdssc = (CDS_StreamConfig*)CDRV_get_cdssc(cdrv_id, cds_stream); // const_cast
  if (cdssc == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSSC_enable_monitor_for_tlm_disruption(cdssc);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_DISABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet)
{
  CDS_StreamConfig* cdssc;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t cds_stream = CCP_get_param_from_packet(packet, 1, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (cds_stream >= CDS_STREAM_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);

  cdssc = (CDS_StreamConfig*)CDRV_get_cdssc(cdrv_id, cds_stream); // const_cast
  if (cdssc == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSSC_disable_monitor_for_tlm_disruption(cdssc);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_SET_TIME_THRESHOLD_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet)
{
  CDS_StreamConfig* cdssc;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t cds_stream = CCP_get_param_from_packet(packet, 1, uint8_t);
  uint32_t time_threshold = CCP_get_param_from_packet(packet, 2, uint32_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (cds_stream >= CDS_STREAM_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);

  cdssc = (CDS_StreamConfig*)CDRV_get_cdssc(cdrv_id, cds_stream); // const_cast
  if (cdssc == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSSC_set_time_threshold_for_tlm_disruption(cdssc, time_threshold);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_HAL_INIT(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  component_driver_utility_.hal.last.ret_from_hal_handler = CDS_hal_init(cds);
  if (component_driver_utility_.hal.last.ret_from_hal_handler != 0)
  {
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT,
                            CDRV_UTIL_conv_hal_handler_ret_to_u32_(component_driver_utility_.hal.last.ret_from_hal_handler));
  }

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_HAL_RX(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  return CDRV_UTIL_hal_rx_(cds, component_driver_utility_.hal.rx_buffer, CDRV_UTIL_HAX_RX_BUFFER_SIZE);
}


CCP_CmdRet Cmd_CDRV_UTIL_HAL_TX(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint16_t data_len = CCP_get_raw_param_len(packet);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  return CDRV_UTIL_hal_tx_(cds, CCP_get_raw_param_head(packet), data_len);
}


CCP_CmdRet Cmd_CDRV_UTIL_HAL_REOPEN(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  int32_t reason = CCP_get_param_from_packet(packet, 0, int32_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  component_driver_utility_.hal.last.ret_from_hal_handler = CDS_hal_reopen(cds, reason);
  if (component_driver_utility_.hal.last.ret_from_hal_handler != 0)
  {
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT,
                            CDRV_UTIL_conv_hal_handler_ret_to_u32_(component_driver_utility_.hal.last.ret_from_hal_handler));
  }

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}



CCP_CmdRet Cmd_CDRV_UTIL_HAL_RX_TO_RAM(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint32_t buffer_adr = CCP_get_param_from_packet(packet, 0, uint32_t);
  int32_t buffer_size = CCP_get_param_from_packet(packet, 0, int32_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (buffer_size <= 0) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);
  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  return CDRV_UTIL_hal_rx_(cds, (uint8_t*)buffer_adr, buffer_size);
}



CCP_CmdRet Cmd_CDRV_UTIL_HAL_TX_FROM_RAM(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint32_t data_adr = CCP_get_param_from_packet(packet, 0, uint32_t);
  int32_t data_size = CCP_get_param_from_packet(packet, 0, int32_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (data_size <= 0) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);
  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  return CDRV_UTIL_hal_tx_(cds,(uint8_t*)data_adr, data_size);
}


CCP_CmdRet Cmd_CDRV_UTIL_HAL_CLEAR_RX_BUFFER(const CommonCmdPacket* packet)
{
  (void)packet;
  memset(component_driver_utility_.hal.rx_buffer, 0x00, CDRV_UTIL_HAX_RX_BUFFER_SIZE);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_SET_TLM(const CommonCmdPacket* packet)
{
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t cds_stream = CCP_get_param_from_packet(packet, 1, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  if (cds_stream >= CDS_STREAM_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 2);

  component_driver_utility_.tlm.cdrv_id = cdrv_id;
  component_driver_utility_.tlm.cds_stream = cds_stream;

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


static CCP_CmdRet CDRV_UTIL_hal_rx_(ComponentDriverSuper* cds, uint8_t* buffer, int buffer_size)
{
  component_driver_utility_.hal.last.data = buffer;
  component_driver_utility_.hal.last.data_size = buffer_size;
  component_driver_utility_.hal.last.ret_from_hal_handler = CDS_hal_rx(cds, buffer, buffer_size);

  if (component_driver_utility_.hal.last.ret_from_hal_handler < 0)
  {
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT,
                            CDRV_UTIL_conv_hal_handler_ret_to_u32_(component_driver_utility_.hal.last.ret_from_hal_handler));
  }

  return CCP_make_cmd_ret(CCP_EXEC_SUCCESS, (uint32_t)component_driver_utility_.hal.last.ret_from_hal_handler);
}


static CCP_CmdRet CDRV_UTIL_hal_tx_(ComponentDriverSuper* cds, const uint8_t* data, int data_size)
{
  component_driver_utility_.hal.last.data = data;
  component_driver_utility_.hal.last.data_size = data_size;
  component_driver_utility_.hal.last.ret_from_hal_handler = CDS_hal_tx(cds, data, data_size);

  if (component_driver_utility_.hal.last.ret_from_hal_handler != 0)
  {
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT,
                            CDRV_UTIL_conv_hal_handler_ret_to_u32_(component_driver_utility_.hal.last.ret_from_hal_handler));
  }

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


static uint32_t CDRV_UTIL_conv_hal_handler_ret_to_u32_(int hal_handler_ret)
{
  if (hal_handler_ret < 0)
  {
    hal_handler_ret = 0x10000000 - hal_handler_ret;
  }
  return (uint32_t)hal_handler_ret;
}

#pragma section
