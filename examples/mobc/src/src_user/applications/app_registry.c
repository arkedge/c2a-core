#pragma section REPRO
#include "app_registry.h"

#include <src_core/system/application_manager/app_manager.h>

#include "app_headers.h"

static AM_ACK add_application_(size_t id,
                               AppInfo (*app_creator)(void));

void AR_load_initial_settings(void)
{
  add_application_(AR_NOP, NOP_create_app);
  add_application_(AR_CSRV_GS_CMD_PH, CSRV_GS_cmd_packet_handler_app);
  add_application_(AR_CSRV_GS_RT_TLM_PH, CSRV_GS_rt_tlm_packet_handler_app);
  add_application_(AR_CSRV_GS_RP_TLM_PH, CSRV_GS_rp_tlm_packet_handler_app);
  add_application_(AR_CSRV_UART_TEST, UART_TEST_update);
  add_application_(AR_GSC_DISPATCHER, GSCD_create_app);
  add_application_(AR_RTC_DISPATCHER, RTCD_create_app);
  add_application_(AR_TLC_DISPATCHER_GS, TLCD_gs_create_app);
  add_application_(AR_TLC_DISPATCHER_BC, TLCD_bc_create_app);
  add_application_(AR_TLC_DISPATCHER_TLM, TLCD_tlm_create_app);
#ifdef TLCD_ENABLE_MISSION_TL
  add_application_(AR_TLC_DISPATCHER_MIS, TLCD_mis_create_app);
#endif
  add_application_(AR_CSRV_AOBC, CSRV_AOBC_update);
  add_application_(AR_CSRV_AOBC_CDIS, CSRV_AOBC_cmd_dispatcher);
  add_application_(AR_EVENT_UTILITY, EVENT_UTIL_create_app);
  add_application_(AR_MEM_DUMP, MEM_create_app);
  add_application_(AR_CCP_DUMP, CCP_DUMP_create_app);
  add_application_(AR_TL_BCT_DIGEST, TL_BCT_DIGEST_create_app);
  add_application_(AR_TELEMETRY_MANAGER, TLM_MGR_create_app);
  add_application_(AR_DIVIDED_CMD_UTILITY, DCU_create_app);
  add_application_(AR_CDRV_UTIL, CDRV_UTIL_create_app);
  add_application_(AR_UTILITY_CMD, UTIL_CMD_create_app);
  // add_application_(AR_UTILITY_COUNTER, UTIL_COUNTER_create_app);
  add_application_(AR_APP_DEMO_CALCULATOR, APP_CALC_create_app);
  add_application_(AR_APP_DBG_FLUSH_SCREEN, APP_DBG_flush_screen);
  add_application_(AR_APP_DBG_PRINT_TIMESTAMP, APP_DBG_print_time_stamp);
  add_application_(AR_APP_DBG_PRINT_CMD_STATUS, APP_DBG_print_cmd_status);
  add_application_(AR_APP_DBG_PRINT_EVENT_LOGGER0, APP_DBG_print_event_logger0);
  add_application_(AR_APP_DBG_PRINT_EVENT_LOGGER1, APP_DBG_print_event_logger1);
  add_application_(AR_APP_DBG_PRINT_EVENT_HANDLER, APP_DBG_print_event_handler);
  add_application_(AR_APP_DBG_PRINT_GIT_REV, APP_DBG_print_git_rev);
}

static AM_ACK add_application_(size_t id,
                               AppInfo (*app_creator)(void))
{
  AppInfo ai = app_creator();
  return AM_register_ai(id, &ai);
}

#pragma section
