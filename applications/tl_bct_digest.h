/**
 * @file
 * @brief TL や BCT のコマンド列の digest を計算し， GS と FSW での整合性を確認するための App
 */
#ifndef TL_BCT_DIGEST_H_
#define TL_BCT_DIGEST_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"
#include "../tlm_cmd/block_command_table.h"
#include "../tlm_cmd/packet_handler.h"
#include "../system/time_manager/obc_time.h"
#include "./timeline_command_dispatcher_id_define.h"

#define TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE   (TL_TLM_PAGE_SIZE * 2)    /*!< TL の digest のページサイズ．
                                                                           計算にはそれなりの時間がかかるので，テレメサイズだけでなく OBC の計算リソースと相談して決める． */
// TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE の user オーバーライド（設定）
#include <src_user/settings/applications/tl_bct_digest_params.h>


/**
 * @enum   TL_BCT_DIGEST_STATUS
 * @note   uint8_t を想定
 * @brief  digest 計算結果
 */
typedef enum
{
  TL_BCT_DIGEST_STATUS_OK,         //!< 計算成功
  TL_BCT_DIGEST_STATUS_PARAM_ERR,  //!< パラメタエラー
  TL_BCT_DIGEST_STATUS_NO_CCP,     //!< digest を計算すべき CCP はなし
  TL_BCT_DIGEST_STATUS_UNKNOWN     //!< 不明なエラー
} TL_BCT_DIGEST_STATUS;

/**
 * @struct TL_BCT_DIGEST_TlDigest
 * @brief  TL の digest
 */
typedef struct
{
  struct
  {
    TLCD_ID tl_id;                //!< TL の ID
    uint8_t page_no;              //!< TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE のページネーション
    TL_BCT_DIGEST_STATUS status;  //!< digest 計算結果
    uint16_t queued;              //!< 該当の TL に保存されたコマンド数
    ObcTime time_stamp;           //!< digest 計算時刻
    uint8_t digests_num;          //!< 計算した digests の数
  } info;
  uint16_t digests[TL_BCT_DIGEST_TL_DIGEST_PAGE_SIZE];  //!< TL の digest
} TL_BCT_DIGEST_TlDigest;

/**
 * @struct TL_BCT_DIGEST_BctDigest
 * @brief  BCT の digest
 */
typedef struct
{
  struct
  {
    bct_id_t block;               //!< BCT block
    TL_BCT_DIGEST_STATUS status;  //!< digest 計算結果
    ObcTime time_stamp;           //!< digest 計算時刻
    uint8_t digests_num;          //!< 計算した digests の数（＝ BCT に登録されたコマンド数）
  } info;
  uint16_t digests[BCT_MAX_CMD_NUM];  //!< BCT の digest
} TL_BCT_DIGEST_BctDigest;

/**
 * @struct CcpDump
 * @brief  CcpDump の AppInfo 構造体
 */
typedef struct
{
  TL_BCT_DIGEST_TlDigest  tl;   //!< TL の digest
  TL_BCT_DIGEST_BctDigest bct;  //!< BCT の digest
} TlBctDigest;

extern const TlBctDigest* const tl_bct_digest;

AppInfo TL_BCT_DIGEST_create_app(void);

CCP_CmdRet Cmd_TL_BCT_DIGEST_TL(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_TL_BCT_DIGEST_BCT(const CommonCmdPacket* packet);

#endif
