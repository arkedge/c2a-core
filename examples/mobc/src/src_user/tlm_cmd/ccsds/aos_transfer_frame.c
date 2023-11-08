#pragma section REPRO
#include "vcdu.h"

#include <string.h> // for memcpy

static void AOSTF_set_common_hdr_(AOSTF* vcdu);
static void AOSTF_clear_spare_(AOSTF* vcdu);

void AOSTF_generate_byte_stream(const AOSTF* vcdu,
                               uint8_t byte_stream[AOSTF_LEN])
{
  memcpy(byte_stream, vcdu->header, AOSTF_HEADER_SIZE);
  byte_stream += AOSTF_HEADER_SIZE;

  M_PDU_generate_byte_stream(&(vcdu->m_pdu), byte_stream);
  byte_stream += M_PDU_LEN;

  memcpy(byte_stream, vcdu->trailer, AOSTF_TRAILER_SIZE);
}

void AOSTF_setup_fill_vcdu(AOSTF* vcdu)
{
  AOSTF_set_common_hdr_(vcdu);
  AOSTF_set_vcid(vcdu, AOSTF_VCID_FILL);
  M_PDU_setup_idle_m_pdu(&(vcdu->m_pdu));
}

void AOSTF_setup_realtime_vcdu_hdr(AOSTF* vcdu,
                                  uint32_t counter)
{
  AOSTF_set_common_hdr_(vcdu);
  AOSTF_set_vcid(vcdu, AOSTF_VCID_REALTIME);
  AOSTF_set_vcdu_counter(vcdu, counter);
}

void AOSTF_setup_replay_vcdu_hdr(AOSTF* vcdu,
                                uint32_t counter)
{
  AOSTF_set_common_hdr_(vcdu);
  AOSTF_set_replay_flag(vcdu, AOSTF_REPLAY_TRUE);
  AOSTF_set_vcid(vcdu, AOSTF_VCID_REPLAY);
  AOSTF_set_vcdu_counter(vcdu, counter);
}

static void AOSTF_set_common_hdr_(AOSTF* vcdu)
{
  AOSTF_set_ver(vcdu, AOSTF_VER_2);
  AOSTF_set_scid(vcdu, AOSTF_SCID_SAMPLE_SATELLITE); // FIXME: 仮
  AOSTF_set_replay_flag(vcdu, AOSTF_REPLAY_FALSE);
  AOSTF_clear_spare_(vcdu);
}

static void AOSTF_clear_spare_(AOSTF* vcdu)
{
  unsigned int pos = 5;
  uint8_t mask = 0x7f; // 01111111b

  vcdu->header[pos] &= (uint8_t)(~mask);
}

AOSTF_VER AOSTF_get_ver(const AOSTF* vcdu)
{
  unsigned int pos = 0;
  uint8_t mask = 0xc0; // 11000000b

  AOSTF_VER ver = (AOSTF_VER)((vcdu->header[pos] & mask) >> 6);

  switch (ver)
  {
  case AOSTF_VER_2:
    return ver;

  default:
    return AOSTF_VER_UNKNOWN;
  }
}

void AOSTF_set_ver(AOSTF* vcdu,
                  AOSTF_VER ver)
{
  unsigned int pos = 0;
  uint8_t mask = 0xc0; // 11000000b
  uint8_t val = (uint8_t)((ver << 6) & mask);

  vcdu->header[pos] &= (uint8_t)(~mask);
  vcdu->header[pos] |= val;
}

AOSTF_SCID AOSTF_get_scdi(const AOSTF* vcdu)
{
  unsigned int pos = 0;
  uint8_t mask1 = 0x3f; // 00111111b
  uint8_t mask2 = 0xc0; // 11000000b

  int scid = (vcdu->header[pos] & mask1);
  scid <<= 2;
  scid |= ((vcdu->header[pos + 1] & mask2) >> 6);

  switch (scid)
  {
  case AOSTF_SCID_SAMPLE_SATELLITE:
    return (AOSTF_SCID)scid;

  default:
    return AOSTF_SCID_UNKNOWN;
  }
}

void AOSTF_set_scid(AOSTF* vcdu,
                   AOSTF_SCID scid)
{
  unsigned int pos = 0;
  uint8_t mask1 = 0x3f; // 00111111b
  uint8_t mask2 = 0xc0; // 11000000b

  vcdu->header[pos] &= (uint8_t)(~mask1);
  vcdu->header[pos] |= (uint8_t)((scid >> 2) & mask1);
  vcdu->header[pos + 1] &= (uint8_t)(~mask2);
  vcdu->header[pos + 1] |= (uint8_t)((scid << 6) & mask2);
}

AOSTF_VCID AOSTF_get_vcid(const AOSTF* vcdu)
{
  unsigned int pos = 1;
  uint8_t mask = 0x3f; // 00111111b

  AOSTF_VCID vcid = (AOSTF_VCID)(vcdu->header[pos] & mask);

  switch (vcid)
  {
  case AOSTF_VCID_REALTIME: // FALLTHROUGH
  case AOSTF_VCID_REPLAY: // FALLTHROUGH
  case AOSTF_VCID_FILL: // FALLTHROUGH
    return vcid;

  default:
    return AOSTF_VCID_UNKNOWN;
  }
}

void AOSTF_set_vcid(AOSTF* vcdu,
                   AOSTF_VCID vcid)
{
  unsigned int pos = 1;
  uint8_t mask = 0x3f; // 00111111b

  vcdu->header[pos] &= (uint8_t)(~mask);
  vcdu->header[pos] |= (uint8_t)(vcid & mask);
}

uint32_t AOSTF_get_vcdu_counter(const AOSTF* vcdu)
{
  unsigned int pos = 2;

  uint32_t counter = vcdu->header[pos];
  counter <<= 8;
  counter |=  vcdu->header[pos + 1];
  counter <<= 8;
  counter |=  vcdu->header[pos + 2];

  return counter;
}

void AOSTF_set_vcdu_counter(AOSTF* vcdu,
                           uint32_t counter)
{
  unsigned int pos = 2;

  vcdu->header[pos] = (uint8_t)((counter >> 16) & 0xff);
  vcdu->header[pos + 1] = (uint8_t)((counter >> 8) & 0xff);
  vcdu->header[pos + 2] = (uint8_t)(counter & 0xff);
}

AOSTF_REPLAY_FLAG AOSTF_get_replay_flag(const AOSTF* vcdu)
{
  unsigned int pos = 5;
  uint8_t mask = 0x80; // 10000000b

  return (AOSTF_REPLAY_FLAG)((vcdu->header[pos] & mask) >> 7);
}

void AOSTF_set_replay_flag(AOSTF* vcdu,
                          AOSTF_REPLAY_FLAG flag)
{
  unsigned int pos = 5;
  uint8_t mask = 0x80; // 10000000b

  vcdu->header[pos] &= (uint8_t)(~mask);
  vcdu->header[pos] |= (uint8_t)((flag << 7) & mask);
}

uint32_t AOSTF_get_clcw(const AOSTF* vcdu)
{
  unsigned int pos = 0;

  uint32_t clcw = vcdu->trailer[pos];
  clcw <<= 8;
  clcw |= vcdu->trailer[pos + 1];
  clcw <<= 8;
  clcw |= vcdu->trailer[pos + 2];
  clcw <<= 8;
  clcw |= vcdu->trailer[pos + 3];

  return clcw;
}

void AOSTF_set_clcw(AOSTF* vcdu,
                   uint32_t clcw)
{
  unsigned int pos = 0;

  vcdu->trailer[pos] = (uint8_t)((clcw >> 24) & 0xff);
  vcdu->trailer[pos + 1] = (uint8_t)((clcw >> 16) & 0xff);
  vcdu->trailer[pos + 2] = (uint8_t)((clcw >> 8) & 0xff);
  vcdu->trailer[pos + 3] = (uint8_t)(clcw & 0xff);
}

uint32_t AOSTF_calc_next_counter(uint32_t prev)
{
  return (prev + 1) % AOSTF_COUNTER_MAX;
}
#pragma section
