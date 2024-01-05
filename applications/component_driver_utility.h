/**
 * @file
 * @brief 様々な Component Driver に統一的な Cmd を提供する
 */
#ifndef COMPONENT_DRIVER_UTILITY_H_
#define COMPONENT_DRIVER_UTILITY_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"
#include "../component_driver/component_driver.h"

#define CDRV_UTIL_HAX_RX_BUFFER_SIZE  (256)   //!< Cmd_CDRV_UTIL_HAL_RX で受信するデータのバッファサイズ

// CDRV_UTIL_HAX_RX_BUFFER_SIZE の user オーバーライド
#include <src_user/settings/applications/component_driver_utility_params.h>

/**
 * @struct ComponentDriverUtility
 * @brief  ComponentDriverUtility の AppInfo 構造体
 */
typedef struct
{
  struct
  {
    uint8_t rx_buffer[CDRV_UTIL_HAX_RX_BUFFER_SIZE];  //!< Cmd_CDRV_UTIL_HAL_RX で受信するデータのバッファ
    struct
    {
      const uint8_t* data;            //!< データアクセスのポインタ
      int32_t  data_size;             //!< data size
      int32_t  ret_from_hal_handler;  //!< hal handler の返り値
    } last;   //!< 最後の hal 操作の情報
  } hal;      //!< HAL low level cmd 用
  struct
  {
    CDRV_ID cdrv_id;       //!< CDRV_ID
    uint8_t cds_stream;    //!< CDS stream
  } tlm;      //!< CDS のメンバを tlm でダウンリンクするための情報
} ComponentDriverUtility;

extern const ComponentDriverUtility* const component_driver_utility;

AppInfo CDRV_UTIL_create_app(void);


CCP_CmdRet Cmd_CDRV_UTIL_CDSC_ENABLE_MONITOR_FOR_RX_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSC_DISABLE_MONITOR_FOR_RX_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSC_SET_TIME_THRESHOLD_FOR_RX_DISRUPTION(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_ENABLE(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_DISABLE(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_ENABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_DISABLE_MONITOR_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_CDSSC_SET_TIME_THRESHOLD_FOR_TLM_DISRUPTION(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CDRV_UTIL_HAL_INIT(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_HAL_RX(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_HAL_TX(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_HAL_REOPEN(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_HAL_RX_TO_RAM(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_HAL_TX_FROM_RAM(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CDRV_UTIL_HAL_CLEAR_RX_BUFFER(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_CDRV_UTIL_SET_TLM(const CommonCmdPacket* packet);

#endif
