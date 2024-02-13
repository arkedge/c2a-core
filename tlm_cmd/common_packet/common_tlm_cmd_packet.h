/**
 * @file
 * @brief  CTCP 関連基本関数の宣言
 * @note   CTCP: CommonTlmCmdPacket
 * @note   CTP:  CommonTlmPacket
 * @note   CCP:  CommonCmdPacket
 */
#ifndef COMMON_TLM_CMD_PACKET_H_
#define COMMON_TLM_CMD_PACKET_H_

#include <stdint.h>
// ここで， CTCP_MAX_LEN, CommonTlmCmdPacket として使うパケット型を指定する
#include <src_user/settings/tlm_cmd/common_packet/common_tlm_cmd_packet_define.h>
#include "./common_tlm_packet.h"
#include "./common_cmd_packet.h"

// ここで APID を定義する．APID_UNKNOWN, APID_FILL_PKT は必須とする．
//   命名規則:
//     Tlm: APID_TLM_{送信元}_TO_{受信先}_VIA_{経由地}
//     Cmd: APID_CMD_{送信元}_TO_{受信先}_VIA_{経由地}
//     ただし，Tlm の TO_SGS，Cmd の SGS は省略できる
//     例:
//       APID_CMD_TO_MOBC (SGS を省略)
//       APID_TLM_AOBC (TO_SGS を省略．パスが単一しかありえないので， VIA_MOBCも省略)
//       APID_TLM_CAMERA_TO_XGS_VIA_MIF (全部記載)
//     注意:
//       同一コンポからアプリケーション等を区別して APID を発行したい場合 (eg; HK テレメ，ミッションデータ，Sバンドデータ，X バンドデータ，など) は {送信元} にそれを識別する命名ができる
//       APID_TLM_AOBC, APID_TLM_AOBC_STT_IMG, など
//     詳細: https://github.com/arkedge/c2a-core/issues/186#issuecomment-1798685321
/* 例
typedef enum
{
  APID_CMD_TO_MOBC = 0x210,   //!< 01000010000b: APID for MOBC 宛の CMD
  APID_CMD_TO_AOBC = 0x211,   //!< 01000010001b: APID for AOBC 宛の CMD
  APID_CMD_TO_TOBC = 0x212,   //!< 01000010010b: APID for TOBC 宛の CMD
  APID_TLM_TCAL = 0x410,      //!< 10000010000b: APID for TIME CARIBLATION TLM (FIXME: 現在まともに使ってない)
  APID_TLM_MOBC = 0x510,      //!< 10100010000b: APID for MOBC で生成される TLM
  APID_TLM_AOBC = 0x511,      //!< 10100010001b: APID for AOBC で生成される TLM
  APID_TLM_TOBC = 0x512,      //!< 10100010002b: APID for TOBC で生成される TLM
  APID_TLM_DUMP = 0x710,      //!< 11100010000b: APID for DUMP TLM (FIXME: 現在まともに使ってない)
  APID_FILL_PKT = 0x7ff,      //!< 11111111111b: APID for FILL PACKET
  APID_UNKNOWN
} APID;
*/
// FIXME: CCSDS JAXA 標準になおす
// FIXME: APID は Space Packet なので， CTCP にあるのは不適切？ 抽象化してもいいかも
#include <src_user/settings/tlm_cmd/ccsds/apid_define.h>


/**
 * @enum   CTCP_PACKET_TYPE
 * @brief  tlm (CTP) or cmd (CCP) を規定
 * @note   uint8_tを想定
 */
typedef enum
{
  CTCP_PACKET_TYPE_CMD,
  CTCP_PACKET_TYPE_TLM,
  CTCP_PACKET_TYPE_UNKNOWN
} CTCP_PACKET_TYPE;


/**
 * @brief  有効なパケットかチェックする
 * @note   NULL チェックも行う
 * @note   OBC の外から来たパケットな可能性もあるので，このC2Aで扱えるサイズかもチェックする
 * @param  packet: CTCP
 * @retval 1: True
 * @retval 0: False
 */
int CTCP_is_valid_packet(const CommonTlmCmdPacket* packet);

/**
 * @brief  tlm か cmd かを判断
 * @param  packet: CTCP
 * @return CTCP_PACKET_TYPE
 */
CTCP_PACKET_TYPE CTCP_get_packet_type(const CommonTlmCmdPacket* packet);

/**
 * @brief  パケット長を取得
 * @param  packet: CTCP
 * @return packet len
 */
uint16_t CTCP_get_packet_len(const CommonTlmCmdPacket* packet);

/**
 * @brief  APID を取得
 * @param  packet: CTCP
 * @return APID
 */
APID CTCP_get_apid(const CommonTlmCmdPacket* packet);

/**
 * @brief  APID を設定
 * @param[in,out] packet: CTCP
 * @param[in]     apid: APID
 * @return void
 */
void CTCP_set_apid(CommonTlmCmdPacket* packet, APID apid);

/**
 * @brief  CommonTlmCmdPacket をコピー
 * @note   user 実装次第だが，高速化のためすすべてではなく必要最低限しかコピーしない場合もある
 * @param[in,out] dest: コピー先 CTCP
 * @param[in]     src:  コピー元 CTCP
 * @return void
 */
void CTCP_copy_packet(CommonTlmCmdPacket* dest, const CommonTlmCmdPacket* src);

/**
 * @brief  CTCP を CTP に変換（キャスト）
 * @note   NULL が返る可能性があるので，呼び出し元でしっかりアサーションすること！
 * @param[in] ctcp: CTCP
 * @retval CTP: 変換成功時
 * @retval NULL: エラー時
 */
const CommonTlmPacket* CTCP_convert_to_ctp(const CommonTlmCmdPacket* ctcp);

/**
 * @brief  CTP を CTCP に変換（キャスト）
 * @note   NULL が返る可能性があるので，呼び出し元でしっかりアサーションすること！
 * @param[in] ctcp: CTP
 * @retval CTCP: 変換成功時
 * @retval NULL: エラー時
 */
const CommonTlmCmdPacket* CTCP_convert_from_ctp(const CommonTlmPacket* ctp);

/**
 * @brief  CTCP を CCP に変換（キャスト）
 * @note   NULL が返る可能性があるので，呼び出し元でしっかりアサーションすること！
 * @param[in] ctcp: CTCP
 * @retval CCP: 変換成功時
 * @retval NULL: エラー時
 */
const CommonCmdPacket* CTCP_convert_to_ccp(const CommonTlmCmdPacket* ctcp);

/**
 * @brief  CCP を CTCP に変換（キャスト）
 * @note   NULL が返る可能性があるので，呼び出し元でしっかりアサーションすること！
 * @param[in] ctcp: CCP
 * @retval CTCP: 変換成功時
 * @retval NULL: エラー時
 */
const CommonTlmCmdPacket* CTCP_convert_from_ccp(const CommonCmdPacket* ccp);

#endif
