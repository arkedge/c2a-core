/**
 * @file
 * @brief TL や BCT のコマンド列の digset を計算し， GS と FSW での整合性を確認するための App
 */
#ifndef TL_BCT_DIGSET_H_
#define TL_BCT_DIGSET_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"
#include "../tlm_cmd/block_command_table.h"
#include "../tlm_cmd/packet_handler.h"
#include "../system/time_manager/obc_time.h"
#include "./timeline_command_dispatcher_id_define.h"

#define TL_BCT_DIGSET_TL_DIGEST_PAGE_SIZE   (TL_TLM_PAGE_SIZE * 2)    //!< TL の digest のページサイズ
// TL_BCT_DIGSET_TL_DIGEST_PAGE_SIZE の user オーバーライド（設定）
#include <src_user/settings/applications/tl_bct_digest_params.h>


/**
 * @struct TL_BCT_DIGSET_TlDigest
 * @brief  TL の digest
 */
typedef struct
{
  struct
  {
    TLCD_ID tl_id;            //!< TL の ID
    uint8_t page_no;          //!< TL_BCT_DIGSET_TL_DIGEST_PAGE_SIZE のページネーション
    ObcTime time_stamp;       //!< digest 計算時刻
  } info;
  uint16_t digests[TL_BCT_DIGSET_TL_DIGEST_PAGE_SIZE];  //!< TL の digest
} TL_BCT_DIGSET_TlDigest;

/**
 * @struct TL_BCT_DIGSET_BctDigest
 * @brief  BCT の digest
 */
typedef struct
{
  struct
  {
    bct_id_t block;           //!< BCT block
    ObcTime time_stamp;       //!< digest 計算時刻
  } info;
  uint16_t digests[BCT_MAX_CMD_NUM];  //!< BCT の digest
} TL_BCT_DIGSET_BctDigest;

/**
 * @struct CcpDump
 * @brief  CcpDump の AppInfo 構造体
 */
typedef struct
{
  TL_BCT_DIGSET_TlDigest  tl;   //!< TL の digest
  TL_BCT_DIGSET_BctDigest bct;  //!< BCT の digest
} TlBctDigest;

extern const TlBctDigest* const tl_bct_digest;

AppInfo TL_BCT_DIGSET_create_app(void);

CCP_CmdRet Cmd_TL_BCT_DIGSET_TL(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_TL_BCT_DIGSET_BCT(const CommonCmdPacket* packet);

#endif
