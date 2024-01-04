/**
 * @file
 * @brief CDIS などの PacketList や BCT の CCP をダンプする (GS から内容を確認できるようにする) ための App
 */
#ifndef CCP_DUMP_H_
#define CCP_DUMP_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"
#include "../tlm_cmd/block_command_table.h"
#include "../system/time_manager/obc_time.h"

/**
 * @enum   CCP_DUMP_TARGET
 * @note   uint8_t を想定
 * @brief  ダンプした CCP のダンプ元
 */
typedef enum
{
  CCP_DUMP_TARGET_CDIS,   //!< CDIS
  CCP_DUMP_TARGET_BCT     //!< BCT
} CCP_DUMP_TARGET;

/**
 * @enum   CCP_DUMP_STATUS
 * @note   uint8_t を想定
 * @brief  ダンプ結果
 */
typedef enum
{
  CCP_DUMP_STATUS_OK,         //!< ダンプ成功
  CCP_DUMP_STATUS_NOT_FOUND   //!< CCP が存在せず
} CCP_DUMP_STATUS;

/**
 * @struct CCP_DUMP_CdisDump
 * @brief  CDIS のダンプ
 */
typedef struct
{
  uint8_t  cdis_idx;          //!< CommandDispatcher.idx
  uint16_t queue_idx;         //!< CDIS の queue の何番目の CCP か
} CCP_DUMP_CdisDump;

/**
 * @struct CCP_DUMP_BctDump
 * @brief  BCT のダンプ
 */
typedef struct
{
  BCT_Pos  pos;               //!< ダンプする BCT の位置
} CCP_DUMP_BctDump;

/**
 * @struct CcpDump
 * @brief  CcpDump の AppInfo 構造体
 */
typedef struct
{
  struct
  {
    CCP_DUMP_CdisDump cdis;
    CCP_DUMP_BctDump  bct;
  } info;                       //!< ダンブ情報（ダンプするパラメタ）
  struct
  {
    CommonCmdPacket ccp;        //!< ダンプした CCP
    CCP_DUMP_TARGET target;     //!< ダンプ元
    CCP_DUMP_STATUS status;     //!< ダンプ結果
    ObcTime dump_time;          //!< ダンプした時刻
  } dump;
} CcpDump;

extern const CcpDump* const ccp_dump;

AppInfo CCP_DUMP_create_app(void);

CCP_CmdRet Cmd_CCP_DUMP_CDIS(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_CCP_DUMP_BCT(const CommonCmdPacket* packet);

#endif
