#pragma section REPRO
#include "c2a_core_main.h"

#include "./library/git_revision.h"
#include "./library/print.h"
#include "./library/endian.h"
#include "./system/task_manager/task_dispatcher.h"
#include "./system/application_manager/app_manager.h"
#include "./system/event_manager/event_manager.h"
#include "./system/time_manager/time_manager.h"
#include "./system/mode_manager/mode_manager.h"
#include "./system/watchdog_timer/watchdog_timer.h"
#include "./tlm_cmd/packet_handler.h"
#include "./tlm_cmd/block_command_table.h"
#include "./tlm_cmd/command_analyze.h"
#include "./tlm_cmd/telemetry_frame.h"

#include <src_user/applications/app_registry.h>
#include <src_user/settings/build_settings.h>

// git revisionをコードに埋め込む
const char GIT_REV_CORE[41]         = GIT_REVISION_C2A_CORE;
const uint32_t GIT_REV_CORE_SHORT   = GIT_REVISION_C2A_CORE_SHORT;
const char GIT_REV_USER[41]         = GIT_REVISION_C2A_USER;
const uint32_t GIT_REV_USER_SHORT   = GIT_REVISION_C2A_USER_SHORT;

void C2A_core_init(void)
{
  // エンディアンの設定が正しいかの確認
#ifdef IS_LITTLE_ENDIAN
  if (ENDIAN_detect_system_endian() != ENDIAN_TYPE_LITTLE)
#else
  if (ENDIAN_detect_system_endian() != ENDIAN_TYPE_BIG)
#endif
  {
    Printf("WARNING: ENDIAN MISMATCH BETWEEN BUILD SETTINGS AND RUNTIME!\n");
  }
  else
  {
    Printf("C2A_init: Endian OK.\n");
  }

  // C2A の初期化
  CA_initialize();            // Cmd Analyze
  Printf("C2A_init: CA_initialize done.\n");
  TF_initialize();            // TLM frame
  Printf("C2A_init: TF_initialize done.\n");
  PH_init();                  // Packet Handler
  Printf("C2A_init: PH_init done.\n");
  EM_initialize();            // Event Manager．App Managerより先に初期化するべき
  Printf("C2A_init: EM_initialize done.\n");
  AM_initialize();            // App Manager
  Printf("C2A_init: AM_initialize done.\n");
  AR_load_initial_settings(); // App Registry
  Printf("C2A_init: AR_load_initial_settings done.\n");
  AM_initialize_all_apps();   // App Managerに登録されてる全アプリの初期化
  Printf("C2A_init: AM_initialize_all_apps done.\n");
  BCT_initialize();           // Block Cmd Table
                              // BC_load_defaults() もここで呼ばれる
  Printf("C2A_init: BCT_initialize done.\n");
  MM_initialize();            // Mode Manager
                              // ここでSTART UP to INITIALのモード遷移も行われる
  Printf("C2A_init: MM_initialize done.\n");
  TDSP_initialize();          // Task Dispatcher
                              // MM初期化よりもあとに行われる必要がある
  Printf("C2A_init: TDSP_initialize done.\n");
  // DebugOutInit();             // Debugのinit          // LVTTL UART ch1での出力．邪魔なのでPrintfの中身とともに無効化 (2019-04-09)
  // Printf("C2A_init: DebugOutInit done.\n");

  TMGR_down_initializing_flag();
}

void C2A_core_main(void)
{
  // ここでいうtask dispatcherは，TL0とかではなく，
  // task listのBlockCommandをdispatchしている．
  // TL0などのPLをdispatchしているのは，tlc_dispatcher @ App/timeline_command_dispatcher である．
  // なお，PL (packetList) のPL_infoとは，順次dispatchしていくパケットを時系列にならべたlinked listである．
  TDSP_execute_pl_as_task_list();
  WDT_clear_wdt();
}

#pragma section
