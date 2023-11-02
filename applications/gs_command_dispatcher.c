#pragma section REPRO
/**
 * @file
 * @brief 地上局から発行された RTC (Real Time Cmd) を実行する
 */

#include "gs_command_dispatcher.h"
#include "../tlm_cmd/packet_handler.h"
#include "../tlm_cmd/common_cmd_packet_util.h"
#include "../library/result.h"

static CommandDispatcher gs_command_dispatcher_;
const CommandDispatcher* const gs_command_dispatcher = &gs_command_dispatcher_;

/**
 * @brief  GSCD App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT GSCD_init_(void);

/**
 * @brief  GSCD App 実行関数
 *
 *         PH_add_gs_cmd_ にて， GS からの RTC が gs_command_dispatcher に紐付けられたコマンドキュー PH_gs_cmd_list に push back される．
 *         そのキューから１つコマンドを取り出し実行する
 * @param  void
 * @return RESULT
 */
static RESULT GSCD_dispatch_(void);


AppInfo GSCD_create_app(void)
{
  return AI_create_app_info("gs_command_dispatcher", GSCD_init_, GSCD_dispatch_);
}

static RESULT GSCD_init_(void)
{
  return CDIS_init(&gs_command_dispatcher_, &PH_gs_cmd_list);
}

static RESULT GSCD_dispatch_(void)
{
  CDIS_dispatch_command(&gs_command_dispatcher_);
  return RESULT_OK;
}

CCP_CmdRet Cmd_GSCD_CLEAR_ERR_LOG(const CommonCmdPacket* packet)
{
  (void)packet;

  // 記録されたエラー情報をクリア
  CDIS_clear_error_status(&gs_command_dispatcher_);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
