#ifndef AOS_TRANSFER_FRAME_H_
#define AOS_TRANSFER_FRAME_H_

#include "m_pdu.h"

#define AOSTF_HEADER_SIZE   (6u)
#define AOSTF_TRAILER_SIZE  (4u)
#define AOSTF_LEN           (AOSTF_HEADER_SIZE + M_PDU_LEN + AOSTF_TRAILER_SIZE)
#define AOSTF_COUNTER_MAX   (0x01000000) // 24bit長

typedef struct
{
  uint8_t header[AOSTF_HEADER_SIZE];
  M_PDU m_pdu;
  uint8_t trailer[AOSTF_TRAILER_SIZE];
} AOSTF;

typedef enum
{
  AOSTF_VER_2 = 1, // 01b: Version2
  AOSTF_VER_UNKNOWN
} AOSTF_VER;

typedef enum
{
  AOSTF_SCID_SAMPLE_SATELLITE = 0x00,
  AOSTF_SCID_UNKNOWN
} AOSTF_SCID;

typedef enum
{
  AOSTF_VCID_REALTIME = 0x01, // 000001b: Realtime Transfer Frame
  AOSTF_VCID_REPLAY = 0x02, // 000010b: Stored Transfer Frame
  AOSTF_VCID_FILL = 0x3f, // 111111b: Fill Transfer Frame
  AOSTF_VCID_UNKNOWN
} AOSTF_VCID;

typedef enum
{
  AOSTF_REPLAY_FALSE = 0, // 0b: Realtime Transfer Frame
  AOSTF_REPLAY_TRUE = 1 // 1b: Replay Transfer Frame
} AOSTF_REPLAY_FLAG;

void AOSTF_generate_byte_stream(const AOSTF* aostf, uint8_t byte_stream[AOSTF_LEN]);

void AOSTF_setup_fill_aostf(AOSTF* aostf);

void AOSTF_setup_realtime_aostf_hdr(AOSTF* aostf, uint32_t counter);

void AOSTF_setup_replay_aostf_hdr(AOSTF* aostf, uint32_t counter);

AOSTF_VER AOSTF_get_ver(const AOSTF* aostf);

void AOSTF_set_ver(AOSTF* aostf, AOSTF_VER ver);

AOSTF_SCID AOSTF_get_scdi(const AOSTF* aostf);

void AOSTF_set_scid(AOSTF* aostf, AOSTF_SCID scid);

AOSTF_VCID AOSTF_get_vcid(const AOSTF* aostf);

void AOSTF_set_vcid(AOSTF* aostf, AOSTF_VCID vcid);

uint32_t AOSTF_get_aostf_counter(const AOSTF* aostf);

void AOSTF_set_aostf_counter(AOSTF* aostf, uint32_t counter);

AOSTF_REPLAY_FLAG AOSTF_get_replay_flag(const AOSTF* aostf);

void AOSTF_set_replay_flag(AOSTF* aostf, AOSTF_REPLAY_FLAG flag);

uint32_t AOSTF_get_clcw(const AOSTF* aostf);

void AOSTF_set_clcw(AOSTF* aostf, uint32_t clcw);

uint32_t AOSTF_calc_next_counter(uint32_t prev);

#endif
