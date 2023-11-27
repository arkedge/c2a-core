/**
 * @file
 * @brief CCSDS の APID を定義する
 * @note  common_tlm_cmd_packet.h などから include されることを前提
 */
#ifndef APID_DEFINE_H_
#define APID_DEFINE_H_

#include <stdint.h>

/**
 * @enum   APID
 * @brief  Application Process ID
 * @note   11bit
 * @note   命名規則:
 *           Tlm: APID_TLM_{送信元}_TO_{受信先}_VIA_{経由地}
 *           Cmd: APID_CMD_{送信元}_TO_{受信先}_VIA_{経由地}
 *           ただし，Tlm の TO_SGS，Cmd の SGS は省略できる
 *           例:
 *             APID_CMD_TO_MOBC (SGS を省略)
 *             APID_TLM_AOBC (TO_SGS を省略．パスが単一しかありえないので， VIA_MOBCも省略)
 *             APID_TLM_CAMERA_TO_XGS_VIA_MIF (全部記載)
 *           注意:
 *             同一コンポからアプリケーション等を区別して APID を発行したい場合 (eg; HK テレメ，ミッションデータ，Sバンドデータ，X バンドデータ，など) は {送信元} にそれを識別する命名ができる
 *             APID_TLM_AOBC, APID_TLM_AOBC_STT_IMG, など
 *           詳細: https://github.com/arkedge/c2a-core/issues/186#issuecomment-1798685321
 */
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


/**
 * @brief  バイト列から APID を取得
 * @param  apid: APID 候補の uint16_t
 * @note   不正な入力のときは APID_UNKNOWN を返す
 * @return APID
 */
APID APID_get_apid_from_uint16(uint16_t apid);

#endif
