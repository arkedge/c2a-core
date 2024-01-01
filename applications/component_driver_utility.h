/**
 * @file
 * @brief 様々な Component Driver に統一的な Cmd を提供する
 */
#ifndef COMPONENT_DRIVER_UTILITY_H_
#define COMPONENT_DRIVER_UTILITY_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"
#include "../component_driver/component_driver.h"


/**
 * @struct ComponentDriverUtility
 * @brief  ComponentDriverUtility の AppInfo 構造体
 */
typedef struct
{
  struct
  {
    CDRV_ID cdrv_id;       //!< CDRV_ID
    uint8_t cds_stream;    //!< CDS stream
  } tlm;      //!< CDS のメンバを tlm でダウンリンクするための情報
} ComponentDriverUtility;

extern const ComponentDriverUtility* const component_driver_utility;

AppInfo CDRV_UTIL_create_app(void);


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_ENABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSC_DISABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSC_SET_TIME_THRESHOLD_FOR_RX_DISRUPTION(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_ENABLE(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_DISABLE(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_ENABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_DISABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_SET_TIME_THRESHOLD_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CDRV_UTIL_SET_TLM(const CommonCmdPacket* packet);

#endif
