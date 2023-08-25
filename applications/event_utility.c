#pragma section REPRO
/**
 * @file
 * @brief イベント処理の定期実行のためのApp
 * @note  実質的には event_handler を定期実行するだけ
 */
#include "event_utility.h"
#include "../system/event_manager/event_handler.h"
#include "../TlmCmd/common_cmd_packet_util.h"
#include "../library/result.h"

#include <stddef.h> // for NULL

static RESULT EVENT_UTIL_init_(void);
static RESULT EVENT_UTIL_update_(void);

static EventUtility event_utility_;
const EventUtility* const event_utility = &event_utility_;

AppInfo EVENT_UTIL_create_app(void)
{
  return AI_create_app_info("event_util", EVENT_UTIL_init_, EVENT_UTIL_update_);
}

static RESULT EVENT_UTIL_init_(void)
{
  event_utility_.is_enabled_eh_execution = 1;
  return RESULT_OK;
}

static RESULT EVENT_UTIL_update_(void)
{
  if (event_utility_.is_enabled_eh_execution)
  {
    EH_execute();
  }
  return RESULT_OK;
}

CCP_CmdRet Cmd_EVENT_UTIL_ENABLE_EH_EXEC(const CommonCmdPacket* packet)
{
  (void)packet;
  event_utility_.is_enabled_eh_execution = 1;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_EVENT_UTIL_DISABLE_EH_EXEC(const CommonCmdPacket* packet)
{
  (void)packet;
  event_utility_.is_enabled_eh_execution = 0;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_EVENT_UTIL_EXEC_EH(const CommonCmdPacket* packet)
{
  (void)packet;
  EH_execute();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
