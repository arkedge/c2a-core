#pragma section REPRO
/**
 * @file
 * @brief TC Transfer Frame のユーザー定義
 */
#include "tc_transfer_frame_define.h"

TCTF_SCID TCTF_get_scid_from_uint16(uint16_t scid)
{
  switch ((TCTF_SCID)scid)
  {
  case TCTF_SCID_SAMPLE_SATELLITE:
    return (TCTF_SCID)scid;
  default:
    return TCTF_SCID_UNKNOWN;
  }
}

#pragma section
