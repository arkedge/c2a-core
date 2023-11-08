#pragma section REPRO
#include "aostf.h"

#include <string.h> // for memcpy

static void AOSTF_set_common_hdr_(AOSTF* aostf);
static void AOSTF_clear_spare_(AOSTF* aostf);

void AOSTF_generate_byte_stream(const AOSTF* aostf,
                               uint8_t byte_stream[AOSTF_LEN])
{
  memcpy(byte_stream, aostf->header, AOSTF_HEADER_SIZE);
  byte_stream += AOSTF_HEADER_SIZE;

  M_PDU_generate_byte_stream(&(aostf->m_pdu), byte_stream);
  byte_stream += M_PDU_LEN;

  memcpy(byte_stream, aostf->trailer, AOSTF_TRAILER_SIZE);
}

void AOSTF_setup_fill_aostf(AOSTF* aostf)
{
  AOSTF_set_common_hdr_(aostf);
  AOSTF_set_vcid(aostf, AOSTF_VCID_FILL);
  M_PDU_setup_idle_m_pdu(&(aostf->m_pdu));
}

void AOSTF_setup_realtime_aostf_hdr(AOSTF* aostf,
                                  uint32_t counter)
{
  AOSTF_set_common_hdr_(aostf);
  AOSTF_set_vcid(aostf, AOSTF_VCID_REALTIME);
  AOSTF_set_aostf_counter(aostf, counter);
}

void AOSTF_setup_replay_aostf_hdr(AOSTF* aostf,
                                uint32_t counter)
{
  AOSTF_set_common_hdr_(aostf);
  AOSTF_set_replay_flag(aostf, AOSTF_REPLAY_TRUE);
  AOSTF_set_vcid(aostf, AOSTF_VCID_REPLAY);
  AOSTF_set_aostf_counter(aostf, counter);
}

static void AOSTF_set_common_hdr_(AOSTF* aostf)
{
  AOSTF_set_ver(aostf, AOSTF_VER_2);
  AOSTF_set_scid(aostf, AOSTF_SCID_SAMPLE_SATELLITE); // FIXME: 仮
  AOSTF_set_replay_flag(aostf, AOSTF_REPLAY_FALSE);
  AOSTF_clear_spare_(aostf);
}

static void AOSTF_clear_spare_(AOSTF* aostf)
{
  unsigned int pos = 5;
  uint8_t mask = 0x7f; // 01111111b

  aostf->header[pos] &= (uint8_t)(~mask);
}

AOSTF_VER AOSTF_get_ver(const AOSTF* aostf)
{
  unsigned int pos = 0;
  uint8_t mask = 0xc0; // 11000000b

  AOSTF_VER ver = (AOSTF_VER)((aostf->header[pos] & mask) >> 6);

  switch (ver)
  {
  case AOSTF_VER_2:
    return ver;

  default:
    return AOSTF_VER_UNKNOWN;
  }
}

void AOSTF_set_ver(AOSTF* aostf,
                  AOSTF_VER ver)
{
  unsigned int pos = 0;
  uint8_t mask = 0xc0; // 11000000b
  uint8_t val = (uint8_t)((ver << 6) & mask);

  aostf->header[pos] &= (uint8_t)(~mask);
  aostf->header[pos] |= val;
}

AOSTF_SCID AOSTF_get_scdi(const AOSTF* aostf)
{
  unsigned int pos = 0;
  uint8_t mask1 = 0x3f; // 00111111b
  uint8_t mask2 = 0xc0; // 11000000b

  int scid = (aostf->header[pos] & mask1);
  scid <<= 2;
  scid |= ((aostf->header[pos + 1] & mask2) >> 6);

  switch (scid)
  {
  case AOSTF_SCID_SAMPLE_SATELLITE:
    return (AOSTF_SCID)scid;

  default:
    return AOSTF_SCID_UNKNOWN;
  }
}

void AOSTF_set_scid(AOSTF* aostf,
                   AOSTF_SCID scid)
{
  unsigned int pos = 0;
  uint8_t mask1 = 0x3f; // 00111111b
  uint8_t mask2 = 0xc0; // 11000000b

  aostf->header[pos] &= (uint8_t)(~mask1);
  aostf->header[pos] |= (uint8_t)((scid >> 2) & mask1);
  aostf->header[pos + 1] &= (uint8_t)(~mask2);
  aostf->header[pos + 1] |= (uint8_t)((scid << 6) & mask2);
}

AOSTF_VCID AOSTF_get_vcid(const AOSTF* aostf)
{
  unsigned int pos = 1;
  uint8_t mask = 0x3f; // 00111111b

  AOSTF_VCID vcid = (AOSTF_VCID)(aostf->header[pos] & mask);

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

void AOSTF_set_vcid(AOSTF* aostf,
                   AOSTF_VCID vcid)
{
  unsigned int pos = 1;
  uint8_t mask = 0x3f; // 00111111b

  aostf->header[pos] &= (uint8_t)(~mask);
  aostf->header[pos] |= (uint8_t)(vcid & mask);
}

uint32_t AOSTF_get_aostf_counter(const AOSTF* aostf)
{
  unsigned int pos = 2;

  uint32_t counter = aostf->header[pos];
  counter <<= 8;
  counter |=  aostf->header[pos + 1];
  counter <<= 8;
  counter |=  aostf->header[pos + 2];

  return counter;
}

void AOSTF_set_aostf_counter(AOSTF* aostf,
                           uint32_t counter)
{
  unsigned int pos = 2;

  aostf->header[pos] = (uint8_t)((counter >> 16) & 0xff);
  aostf->header[pos + 1] = (uint8_t)((counter >> 8) & 0xff);
  aostf->header[pos + 2] = (uint8_t)(counter & 0xff);
}

AOSTF_REPLAY_FLAG AOSTF_get_replay_flag(const AOSTF* aostf)
{
  unsigned int pos = 5;
  uint8_t mask = 0x80; // 10000000b

  return (AOSTF_REPLAY_FLAG)((aostf->header[pos] & mask) >> 7);
}

void AOSTF_set_replay_flag(AOSTF* aostf,
                          AOSTF_REPLAY_FLAG flag)
{
  unsigned int pos = 5;
  uint8_t mask = 0x80; // 10000000b

  aostf->header[pos] &= (uint8_t)(~mask);
  aostf->header[pos] |= (uint8_t)((flag << 7) & mask);
}

uint32_t AOSTF_get_clcw(const AOSTF* aostf)
{
  unsigned int pos = 0;

  uint32_t clcw = aostf->trailer[pos];
  clcw <<= 8;
  clcw |= aostf->trailer[pos + 1];
  clcw <<= 8;
  clcw |= aostf->trailer[pos + 2];
  clcw <<= 8;
  clcw |= aostf->trailer[pos + 3];

  return clcw;
}

void AOSTF_set_clcw(AOSTF* aostf,
                   uint32_t clcw)
{
  unsigned int pos = 0;

  aostf->trailer[pos] = (uint8_t)((clcw >> 24) & 0xff);
  aostf->trailer[pos + 1] = (uint8_t)((clcw >> 16) & 0xff);
  aostf->trailer[pos + 2] = (uint8_t)((clcw >> 8) & 0xff);
  aostf->trailer[pos + 3] = (uint8_t)(clcw & 0xff);
}

uint32_t AOSTF_calc_next_counter(uint32_t prev)
{
  return (prev + 1) % AOSTF_COUNTER_MAX;
}
#pragma section
