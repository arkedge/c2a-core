#pragma section REPRO
/**
 * @file
 * @brief Command Dispatcher Manager
 * @note  様々な CDIS を一括管理する
 * @note  例えば，様々な CDIS のテレメトリをいちいち作るのはめんどくさい（とくに MOBC にある sub OBC 用の CDIS）ため，それを一括で提供する
 */
#include "command_dispatcher_manager.h"
#include <stddef.h>
#include <string.h>
#include "./common_cmd_packet_util.h"

static CommandDispatcherManager command_dispatcher_manager_;
const CommandDispatcherManager* const command_dispatcher_manager = &command_dispatcher_manager_;


static void CDIS_MGR_initialize(void)
{
  uint8_t idx;
  memset(&command_dispatcher_manager_, 0x00, sizeof(command_dispatcher_manager_));
  for (idx = 0; idx < CDIS_MGR_MAX_NUM_OF_CDIS; ++idx)
  {
    command_dispatcher_manager_.cdises[idx] = NULL;
  }
}


RESULT CDIS_MGR_register_cdis(const CommandDispatcher* cdis)
{
  if (CDIS_MGR_find_cdis_(cdis) == RESULT_OK)
  {
    return RESULT_OK;
  }

  if (command_dispatcher_manager_.num_of_cdis >= CDIS_MGR_MAX_NUM_OF_CDIS)
  {
    return RESULT_ERR;
  }

  command_dispatcher_manager_.cdises[command_dispatcher_manager_.num_of_cdis] = cdis;
  command_dispatcher_manager_.num_of_cdis++;

  return RESULT_OK;
}


RESULT CDIS_MGR_find_cdis(const CommandDispatcher* cdis, uint8_t* idx)
{
  uint8_t i;
  for (i = 0; i < command_dispatcher_manager_.num_of_cdis; ++i)
  {
    if (command_dispatcher_manager_.cdises[i] == cdis)
    {
      *idx = i;
      return RESULT_OK;
    }
  }
  return RESULT_ERR;
}


CCP_CmdRet Cmd_CDIS_MGR_SET_IDX_FOR_TLM(const CommonCmdPacket* packet)
{
  uint8_t idx_for_tlm = CCP_get_param_from_packet(packet, 0, uint8_t);
  if (idx_for_tlm >= command_dispatcher_manager_.num_of_cdis)
  {
    CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, idx_for_tlm);
  }

  command_dispatcher_manager_.idx_for_tlm = idx_for_tlm;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
