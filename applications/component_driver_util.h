/**
 * @file
 * @brief 様々な Component Driver に統一的な Cmd を提供する
 */
#ifndef COMPONENT_DRIVER_UTIL_H_
#define COMPONENT_DRIVER_UTIL_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"
#include "../component_driver/component_driver.h"


/**
 * @struct ComponentDriverUtil
 * @brief  ComponentDriverUtil の AppInfo 構造体
 */
typedef struct
{
  struct
  {
    CDRV_ID cdrv_id;       //!< CDRV_ID
    uint8_t cds_stream;    //!< CDS stream
  } tlm;      //!< CDS のメンバを tlm でダウンリンクするための情報
} ComponentDriverUtil;

extern const ComponentDriverUtil* const component_driver_util;

AppInfo CDRV_UTIL_create_app(void);

CCP_CmdRet Cmd_CDRV_UTIL_SET_TLM(const CommonCmdPacket* packet);

#endif
