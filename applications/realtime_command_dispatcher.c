#pragma section REPRO
#include "realtime_command_dispatcher.h"
#include "../TlmCmd/packet_handler.h"
#include "../TlmCmd/common_cmd_packet_util.h"
#include "../Library/result.h"

static CommandDispatcher realtime_command_dispatcher_;
const CommandDispatcher* const realtime_command_dispatcher = &realtime_command_dispatcher_;

static RESULT RTCD_init_(void);
static RESULT RTCD_dispatch_(void);

AppInfo RTCD_create_app(void)
{
  return AI_create_app_info("realtime_command_dispatcher", RTCD_init_, RTCD_dispatch_);
}

static RESULT RTCD_init_(void)
{
  return CDIS_init(&realtime_command_dispatcher_, &PH_rt_cmd_list);
}

static RESULT RTCD_dispatch_(void)
{
  CDIS_dispatch_command(&realtime_command_dispatcher_);
  return RESULT_OK;
}

CCP_CmdRet Cmd_RTCD_CLEAR_ALL_REALTIME(const CommonCmdPacket* packet)
{
  (void)packet;

  CDIS_clear_command_list(&realtime_command_dispatcher_);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_RTCD_CLEAR_ERR_LOG(const CommonCmdPacket* packet)
{
  (void)packet;

  // 記録されたエラー情報を解除。
  CDIS_clear_error_status(&realtime_command_dispatcher_);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}
#pragma section
