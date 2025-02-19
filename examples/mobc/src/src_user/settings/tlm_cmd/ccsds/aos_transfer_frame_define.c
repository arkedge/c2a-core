#pragma section REPRO
/**
 * @file
 * @brief AOS Transfer Frame のユーザー定義
 */
#include "aos_transfer_frame_define.h"

AOSTF_SCID AOSTF_get_scid_from_uint16(uint16_t scid)
{
  switch ((AOSTF_SCID)scid)
  {
  case AOSTF_SCID_SAMPLE_SATELLITE:
    return (AOSTF_SCID)scid;
  default:
    return AOSTF_SCID_UNKNOWN;
  }
}

#pragma section
