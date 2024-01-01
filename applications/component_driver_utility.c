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


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_ENABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet)
{
  ComponentDriverSuper* cds;
  CDRV_ID cdrv_id = (CDRV_ID)CCP_get_param_from_packet(packet, 0, uint8_t);

  if (cdrv_id >= CDRV_ID_MAX) return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);

  cds = (ComponentDriverSuper*)CDRV_get_cds(cdrv_id);    // const_cast
  if (cds == NULL) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  CDSC_enable_monitor_for_rx_disruption(cds);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_DISABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet)
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

#pragma section
