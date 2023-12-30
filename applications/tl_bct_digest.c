#pragma section REPRO
/**
 * @file
 * @brief TL や BCT のコマンド列の digset を計算し， GS と FSW での整合性を確認するための App
 */
#include "tl_bct_digest.h"
#include <stddef.h>
#include <string.h>
#include "../tlm_cmd/command_dispatcher_manager.h"
#include "../system/time_manager/time_manager.h"

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT CCP_DUMP_init_(void);

static RESULT CCP_DUMP_cdis_(const CommandDispatcher* cdis);
static RESULT CCP_DUMP_bct_(void);


static CcpDump ccp_dump_;
const CcpDump* const ccp_dump = &ccp_dump_;


AppInfo CCP_DUMP_create_app(void)
{
  return AI_create_app_info("ccpdump", CCP_DUMP_init_, NULL);
}


static RESULT CCP_DUMP_init_(void)
{
  memset(&ccp_dump_, 0x00, sizeof(CcpDump));

  // 初回はなにもダンプしてないので
  ccp_dump_.dump.status = CCP_DUMP_STATUS_NOT_FOUND;
  return RESULT_OK;
}

#pragma section
