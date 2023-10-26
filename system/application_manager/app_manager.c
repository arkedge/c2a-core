#pragma section REPRO
/**
 * @file
 * @brief Application Manager
 * @note  C2A の App を管理する
 */
#include "app_manager.h"
#include <src_user/settings/sils_define.h>

#include <string.h>
#include "../../applications/nop.h"
#include "../event_manager/event_logger.h"
#include "../time_manager/time_manager.h"
#include "../watchdog_timer/watchdog_timer.h"
#include <src_user/tlm_cmd/command_definitions.h>
#include "../../library/print.h"
#include "../../tlm_cmd/common_cmd_packet_util.h"

static AM_ACK AM_initialize_app_(size_t id);
static AM_ACK AM_execute_app_(size_t id);

static AppManager app_manager_;
const AppManager* const app_manager = &app_manager_;


void AM_initialize(void)
{
  int i;

  for (i = 0; i < AM_MAX_APPS; ++i)
  {
    app_manager_.ais[i] = NOP_create_app();
  }

  // テレメトリページ番号を0に初期化。
  app_manager_.page_no = 0;
}


AM_ACK AM_register_ai(size_t id, const AppInfo* ai)
{
  if (id >= AM_MAX_APPS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_APP_MANAGER, AM_INVALID_ID, EL_ERROR_LEVEL_LOW, (uint32_t)id);
    return AM_INVALID_ID;
  }

  app_manager_.ais[id] = *ai;
  return AM_SUCCESS;
}


void AM_initialize_all_apps(void)
{
  size_t i;

  for (i = 0; i < AM_MAX_APPS; ++i)
  {
    AM_initialize_app_(i);
  }
}


CCP_CmdRet Cmd_AM_REGISTER_APP(const CommonCmdPacket* packet)
{
  AppInfo ai;
  size_t id = (size_t)CCP_get_param_from_packet(packet, 0, uint32_t);
  ai.initializer = (RESULT (*)(void))CCP_get_param_from_packet(packet, 1, uint32_t);
  ai.entry_point = (RESULT (*)(void))CCP_get_param_from_packet(packet, 2, uint32_t);

  ai.name = "SPECIAL";
  AI_clear_app_exec_time(&ai);

  switch (AM_register_ai(id, &ai))
  {
  case AM_SUCCESS:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case AM_INVALID_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  default:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


CCP_CmdRet Cmd_AM_INITIALIZE_APP(const CommonCmdPacket* packet)
{
  size_t id = (size_t)CCP_get_param_from_packet(packet, 0, uint32_t);

  switch (AM_initialize_app_(id))
  {
  case AM_SUCCESS:          // FALLTHROUGH
  case AM_NOT_REGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case AM_INVALID_ID:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  default:
    // AM_INIT_FAILED
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


static AM_ACK AM_initialize_app_(size_t id)
{
#ifndef SILS_FW
  ObcTime start, finish;
#endif
  RESULT ret;

  if (id >= AM_MAX_APPS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_APP_MANAGER, AM_INVALID_ID, EL_ERROR_LEVEL_LOW, (uint32_t)id);
    return AM_INVALID_ID;
  }
  else if (app_manager_.ais[id].initializer == NULL)
  {
    return AM_NOT_REGISTERED;
  }

#ifdef SILS_FW
  ret = app_manager_.ais[id].initializer();
#else
  start = TMGR_get_master_clock_from_boot();
  ret = app_manager_.ais[id].initializer();
  finish = TMGR_get_master_clock_from_boot();

  // 処理時間情報アップデート
  app_manager_.ais[id].init_duration = OBCT_diff_in_step(&start, &finish);
#endif

  WDT_clear_wdt();

  if (ret != RESULT_OK)
  {
    Printf("App: >>>>> %s Init Failed!!!!! <<<<<\n", app_manager_.ais[id].name);
    EL_record_event((EL_GROUP)EL_CORE_GROUP_APP_MANAGER, AM_INIT_FAILED, EL_ERROR_LEVEL_HIGH, (uint32_t)id);
    return AM_INIT_FAILED;
  }

  Printf("App: %s Init Complete !\n", app_manager_.ais[id].name);
  return AM_SUCCESS;
}


CCP_CmdRet Cmd_AM_EXECUTE_APP(const CommonCmdPacket* packet)
{
  size_t id = (size_t)CCP_get_param_from_packet(packet, 0, uint32_t);

  switch (AM_execute_app_(id))
  {
  case AM_SUCCESS:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  case AM_INVALID_ID:     // FALLTHROUGH
  case AM_NOT_REGISTERED:
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  default:
    // AM_EXEC_FAILED
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);
  }
}


static AM_ACK AM_execute_app_(size_t id)
{
#ifndef SILS_FW
  ObcTime start, finish;
#endif
  RESULT ret;

  if (id >= AM_MAX_APPS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_APP_MANAGER, AM_INVALID_ID, EL_ERROR_LEVEL_LOW, (uint32_t)id);
    return AM_INVALID_ID;
  }
  else if (app_manager_.ais[id].entry_point == NULL)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_APP_MANAGER, AM_NOT_REGISTERED, EL_ERROR_LEVEL_LOW, (uint32_t)id);
    return AM_NOT_REGISTERED;
  }

#ifdef SILS_FW
  ret = app_manager_.ais[id].entry_point();
#else
  start = TMGR_get_master_clock();
  ret = app_manager_.ais[id].entry_point();
  finish = TMGR_get_master_clock();

  // 処理時間情報アップデート
  app_manager_.ais[id].prev = OBCT_diff_in_step(&start, &finish);

  if (app_manager_.ais[id].max < app_manager_.ais[id].prev)
  {
    app_manager_.ais[id].max = app_manager_.ais[id].prev;
  }

  if (app_manager_.ais[id].min > app_manager_.ais[id].prev)
  {
    app_manager_.ais[id].min = app_manager_.ais[id].prev;
  }
#endif

  if (ret != RESULT_OK)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_APP_MANAGER, AM_EXEC_FAILED, EL_ERROR_LEVEL_HIGH, (uint32_t)id);
    return AM_EXEC_FAILED;
  }
  return AM_SUCCESS;
}


CCP_CmdRet Cmd_AM_SET_PAGE_FOR_TLM(const CommonCmdPacket* packet)
{
  uint8_t page = CCP_get_param_from_packet(packet, 0, uint8_t);

  if (page >= AM_TLM_PAGE_MAX)
  {
    // ページ番号がコマンドテーブル範囲外
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  }

  app_manager_.page_no = page;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_AM_CLEAR_APP_INFO(const CommonCmdPacket* packet)
{
  int i;
  (void)packet;

  for (i = 0; i < AM_MAX_APPS; ++i)
  {
    AI_clear_app_exec_time(&app_manager_.ais[i]);
  }

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


#pragma section
