#ifndef APP_MANAGER_H_
#define APP_MANAGER_H_

#include <stddef.h> // for size_t

#include "app_info.h"
#include "../../TlmCmd/common_cmd_packet.h"

#define AM_TLM_PAGE_SIZE (32)                               //!< AMのAppInfoテーブルの1テレメトリパケット(=1ページ)に格納されるAppInfo数（ページネーション用）
#define AM_TLM_PAGE_MAX (4)                                 //!< AMのAppInfoテーブルのページ数（ページネーション用）
#define AM_MAX_APPS (AM_TLM_PAGE_SIZE * AM_TLM_PAGE_MAX)    //!< 登録できる最大AppInfo数，これは登録できる最大アプリ数と等しい

#include <src_user/Settings/System/app_manager_params.h>

typedef struct
{
  AppInfo ais[AM_MAX_APPS];
  int page_no;
} AppManager;

typedef enum
{
  AM_SUCCESS,
  AM_INVALID_ID,
  AM_NOT_REGISTERED
} AM_ACK;

extern const AppManager* const app_manager;

void AM_initialize(void);

AM_ACK AM_register_ai(size_t id,
                      const AppInfo* ai);

void AM_initialize_all_apps(void);

CCP_CmdRet Cmd_AM_REGISTER_APP(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_AM_INITIALIZE_APP(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_AM_EXECUTE_APP(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_AM_SET_PAGE_FOR_TLM(const CommonCmdPacket* packet);

// 2019-07-18 追加
// min, max, prevのみ消す．initはそのまま
CCP_CmdRet Cmd_AM_CLEAR_APP_INFO(const CommonCmdPacket* packet);

#endif
