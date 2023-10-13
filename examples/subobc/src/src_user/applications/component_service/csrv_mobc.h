/**
 * @file
 * @brief MOBC の DI． MOBC における GS との DI に相当する
 */
#ifndef CSRV_MOBC_H_
#define CSRV_MOBC_H_

#include "../../component_driver/etc/mobc.h"
#include <src_core/system/application_manager/app_info.h>
#include <src_core/tlm_cmd/common_cmd_packet.h>

extern const MOBC_Driver* const mobc_driver;           //!< MOBC driver

/**
 * @brief  MOBC update アプリ生成関数
 * @param  void
 * @return AppInfo
 */
AppInfo CSRV_MOBC_update(void);

/**
 * @brief  MOBC RT_TLM (Realtime Telemetry) PH (packet handler) アプリ生成関数
 * @param  void
 * @return AppInfo
 * @note   ひとまずテレメは RT_TLM のみ実装する
 */
AppInfo CSRV_MOBC_rt_tlm_packet_handler(void);

#endif
