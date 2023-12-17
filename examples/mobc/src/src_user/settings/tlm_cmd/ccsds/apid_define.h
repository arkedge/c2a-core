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
 * @note   命名規則は tlm_cmd/common_tlm_cmd_packet.h を参照
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
  APID_TLM_DUMP = 0x710,      //!< 11100010000b: APID for DUMP TLM
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
