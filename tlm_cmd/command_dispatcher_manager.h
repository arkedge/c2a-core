/**
 * @file
 * @brief Command Dispatcher Manager
 * @note  様々な CDIS を一括管理する
 * @note  例えば，様々な CDIS のテレメトリをいちいち作るのはめんどくさい（とくに MOBC にある sub OBC 用の CDIS）ため，それを一括で提供する
 */
#ifndef COMMAND_DISPATCHER_MANAGER_H_
#define COMMAND_DISPATCHER_MANAGER_H_

#include "./common_cmd_packet.h"
#include "./command_dispatcher.h"
#include "../library/result.h"

#define CDIS_MGR_MAX_NUM_OF_CDIS    (7)    /*!< 保持する CDIS の最大数
                                                RT, TL * 4, GS, TDSP */

// CDIS_MGR_MAX_NUM_OF_CDIS の user オーバーライド
#include <src_user/Settings/tlm_cmd/command_dispatcher_manager_params.h>

/**
 * @struct CommandDispatcherManager
 * @brief  CommandDispatcherManager の AppInfo 構造体
 */
typedef struct
{
  const CommandDispatcher* cdises[CDIS_MGR_MAX_NUM_OF_CDIS];
  uint8_t num_of_cdis;
  uint8_t idx_for_tlm;
} CommandDispatcherManager;

extern const CommandDispatcherManager* const command_dispatcher_manager;


/**
 * @brief  Command Dispatcher Manager の初期化
 * @param  void
 * @return void
 */
void CDIS_MGR_initialize(void);

/**
 * @brief  cdis を cdis mgr に登録すし，登録した idx を返す
 * @note   CDIS_init から呼ばれることを想定
 * @note   同じアプリ内でも CDIS_init は複数回呼ばれる可能性があるので，重複登録はケアされる
 * @param[in]  cdis: 登録する cdis のポインタ
 * @param[out] cdis: 登録した idx
 * @retval RESULT_OK:  登録に成功 or すでに登録されている
 * @retval RESULT_ERR: 登録に失敗（サイズオーバー）
 */
RESULT CDIS_MGR_register_cdis(const CommandDispatcher* cdis, uint8_t* idx);

CCP_CmdRet Cmd_CDIS_MGR_SET_IDX_FOR_TLM(const CommonCmdPacket* packet);

#endif
