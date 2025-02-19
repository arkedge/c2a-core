/**
 * @file
 * @brief AOS Transfer Frame のユーザー定義
 * @note  aos_transfer_frame.h などから include されることを前提
 */
#ifndef AOS_TRANSFER_FRAME_DEFINE_H_
#define AOS_TRANSFER_FRAME_DEFINE_H_

#include <stdint.h>

/**
 * @enum  AOSTF_SCID
 * @brief Spacecraft ID
 * @note  10 bit
 */
typedef enum
{
  AOSTF_SCID_SAMPLE_SATELLITE = 0x000,
  AOSTF_SCID_UNKNOWN
} AOSTF_SCID;

#define AOSTF_SCID_MY_ID  (AOSTF_SCID_SAMPLE_SATELLITE)   //!< 自分の SCID

/**
 * @brief  バイト列から AOSTF_SCID を取得
 * @param  scid: AOSTF_SCID 候補の uint16_t
 * @note   不正な入力のときは AOSTF_SCID_UNKNOWN を返す
 * @return AOSTF_SCID
 */
AOSTF_SCID AOSTF_get_scid_from_uint16(uint16_t scid);

#endif
