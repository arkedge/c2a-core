#pragma section REPRO
/**
 * @file
 * @brief CCSDS の APID を定義する
 * @note  common_tlm_cmd_packet.h などから include されることを前提
 */
#include "apid_define.h"
#include "../common_tlm_packet_define.h"

APID APID_get_apid_from_uint16(uint16_t apid)
{
  switch ((APID)apid)
  {
  case APID_CMD_TO_MOBC:  // FALLTHROUGH
  case APID_CMD_TO_AOBC:  // FALLTHROUGH
  case APID_CMD_TO_TOBC:  // FALLTHROUGH
  case APID_TLM_TCAL:     // FALLTHROUGH
  case APID_TLM_MOBC:     // FALLTHROUGH
  case APID_TLM_AOBC:     // FALLTHROUGH
  case APID_TLM_TOBC:     // FALLTHROUGH
  case APID_TLM_DUMP:     // FALLTHROUGH
  case APID_FILL_PKT:
    return (APID)apid;

  default:
    return APID_UNKNOWN;
  }
}

#pragma section
