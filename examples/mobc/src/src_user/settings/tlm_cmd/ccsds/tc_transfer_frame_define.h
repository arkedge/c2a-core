/**
 * @file
 * @brief TC Transfer Frame のユーザー定義
 * @note  tc_transfer_frame.h などから include されることを前提
 */
#ifndef TC_TRANSFER_FRAME_DEFINE_H_
#define TC_TRANSFER_FRAME_DEFINE_H_

#include <stdint.h>

/**
 * @enum  TCTF_SCID
 * @brief Spacecraft ID
 * @note  10 bit
 */
typedef enum
{
  TCTF_SCID_SAMPLE_SATELLITE = 0x157, // SCID for command of sample satellite
  TCTF_SCID_UNKNOWN
} TCTF_SCID;

#define TCTD_SCID_MY_ID (TCTF_SCID_SAMPLE_SATELLITE)    //!< 自分の SCID

/**
 * @brief  バイト列から TCTF_SCID を取得
 * @param  scid: TCTF_SCID 候補の uint16_t
 * @note   不正な入力のときは TCTF_SCID_UNKNOWN を返す
 * @return TCTF_SCID
 */
TCTF_SCID TCTF_get_scid_from_uint16(uint16_t scid);

#endif
