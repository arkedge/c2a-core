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
  case APID_MOBC_CMD:   // FALLTHROUGH
  case APID_AOBC_CMD:   // FALLTHROUGH
  case APID_TOBC_CMD:   // FALLTHROUGH
  case APID_TCAL_TLM:   // FALLTHROUGH
  case APID_MOBC_TLM:   // FALLTHROUGH
  case APID_AOBC_TLM:   // FALLTHROUGH
  case APID_TOBC_TLM:   // FALLTHROUGH
  case APID_DUMP_TLM:   // FALLTHROUGH
  case APID_FILL_PKT:
    return (APID)apid;

  default:
    return APID_UNKNOWN;
  }
}

#pragma section
