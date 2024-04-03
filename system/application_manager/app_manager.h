/**
 * @file
 * @brief Application Manager
 * @note  C2A の App を管理する
 */
#ifndef APP_MANAGER_H_
#define APP_MANAGER_H_

#include <stddef.h>
#include "app_info.h"
#include "../../tlm_cmd/common_packet/common_cmd_packet.h"

#define AM_TLM_PAGE_SIZE (16)                               //!< AMのAppInfoテーブルの1テレメトリパケット(=1ページ)に格納されるAppInfo数（ページネーション用）
#define AM_TLM_PAGE_MAX (8)                                 //!< AMのAppInfoテーブルのページ数（ページネーション用）
#define AM_MAX_APPS (AM_TLM_PAGE_SIZE * AM_TLM_PAGE_MAX)    //!< 登録できる最大AppInfo数，これは登録できる最大アプリ数と等しい

#include <src_user/settings/system/app_manager_params.h>

/**
 * @struct AppManager
 * @brief  AppManager の Info 構造体
 */
typedef struct
{
  AppInfo ais[AM_MAX_APPS];
  int page_no;
} AppManager;

/**
 * @enum   AM_ACK
 * @note   uint8_t を想定
 * @brief  AM エラーコード
 */
typedef enum
{
  AM_SUCCESS,
  AM_INVALID_ID,
  AM_NOT_REGISTERED,
  AM_INIT_FAILED,
  AM_EXEC_FAILED
} AM_ACK;

extern const AppManager* const app_manager;

/**
 * @brief  App Manager の初期化
 * @param  void
 * @return void
 */
void AM_initialize(void);

/**
 * @brief  AM に App を登録する
 * @param  id: 登録する App の ID
 * @param  ai: 登録する App の AppInfo
 * @retval AM_SUCCESS: 成功
 * @retval AM_INVALID_ID: ID の不正
 */
AM_ACK AM_register_ai(size_t id, const AppInfo* ai);

/**
 * @brief  AM に登録されたすべての App を初期化する
 * @param  void
 * @return void
 */
void AM_initialize_all_apps(void);

CCP_CmdRet Cmd_AM_REGISTER_APP(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_AM_INITIALIZE_APP(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_AM_EXECUTE_APP(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_AM_SET_PAGE_FOR_TLM(const CommonCmdPacket* packet);

// 2019-07-18 追加
// min, max, prevのみ消す．initはそのまま
CCP_CmdRet Cmd_AM_CLEAR_APP_INFO(const CommonCmdPacket* packet);

#endif
