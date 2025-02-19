/**
 * @file
 * @brief CCSDS で規定される AOS Transfer Frame の実装
 * @note  packet 構造
 *        |---------+-------+-------+--------------------------|
 *        | Pos     | Pos   | Size  | Name                     |
 *        | [octet] | [bit] | [bit] |                          |
 *        |---------+-------+-------+--------------------------|
 *        | === Primary Header ================================|
 *        |---------+-------+-------+--------------------------|
 *        |       0 |     0 |     2 | Version Number           |
 *        |       0 |     2 |     8 | Spacecraft ID (SCID)     |
 *        |       1 |     2 |     6 | Virtual Channel ID       |
 *        |       2 |     0 |    24 | Virtual Channel          |
 *        |         |       |       |           Frame Count    |
 *        |       5 |     0 |     1 | Replay Flag              |
 *        |       5 |     1 |     1 | VC Frame Count           |
 *        |         |       |       |           Usage Flag     |
 *        |       5 |     2 |     2 | SCID Extension           |
 *        |       5 |     4 |     4 | VC Frame Count Cycle     |
 *        |---------+-------+-------+--------------------------|
 *        | === User Data Field ===============================|
 *        |---------+-------+-------+--------------------------|
 *        |       6 |     0 |     * | M_PDU など               |
 *        |---------+-------+-------+--------------------------|
 *        | === Trailer =======================================|
 *        |---------+-------+-------+--------------------------|
 *        |   N - 4 |     0 |    32 | CLCW                     |
 *        |---------+-------+-------+--------------------------|
 *
 *        Primary Header:
 *          Version Number と Spacecraft ID をまとめて Master Channel ID と呼ぶ
 *            以下をまとめて Signaling Field と呼ぶ
 *          Frame Header Error Control がある Option もある
 *        Transfer Frame Insert Zone:
 *          Primary Header の後に Transfer Frame Insert Zone がある Option もある
 *        Trailer:
 *          ここでの定義以外にさまざまな Option がある
 */
#ifndef AOS_TRANSFER_FRAME_H_
#define AOS_TRANSFER_FRAME_H_

#include "multiplexing_protocol_data_unit.h"

#define AOSTF_HEADER_SIZE   (6u)
#define AOSTF_TRAILER_SIZE  (4u)
#define AOSTF_LEN           (AOSTF_HEADER_SIZE + M_PDU_LEN + AOSTF_TRAILER_SIZE)
#define AOSTF_COUNTER_MAX   (0x01000000) // 24bit長

// ここで AOSTF_SCID, AOSTF_SCID_MY_ID を定義する
// AOSTF_SCID_UNKNOWN は必須
/* 例
typedef enum
{
  AOSTF_SCID_SAMPLE_SATELLITE = 0x00,
  AOSTF_SCID_UNKNOWN
} AOSTF_SCID;
*/
#include <src_user/settings/tlm_cmd/ccsds/aos_transfer_frame_define.h>

typedef struct
{
  uint8_t                      header[AOSTF_HEADER_SIZE];
  MultiplexingProtocolDataUnit m_pdu;
  uint8_t                      trailer[AOSTF_TRAILER_SIZE];
} AosTransferFrame;

typedef enum
{
  AOSTF_VER_2 = 1, // 01b: Version2
  AOSTF_VER_UNKNOWN
} AOSTF_VER;

typedef enum
{
  AOSTF_VCID_REALTIME = 0x01, // 000001b: Realtime Transfer Frame
  AOSTF_VCID_REPLAY   = 0x02, // 000010b: Stored Transfer Frame
  AOSTF_VCID_FILL     = 0x3f, // 111111b: Fill Transfer Frame
  AOSTF_VCID_UNKNOWN
} AOSTF_VCID;

typedef enum
{
  AOSTF_REPLAY_FALSE = 0, // 0b: Realtime Transfer Frame
  AOSTF_REPLAY_TRUE   = 1 // 1b: Replay Transfer Frame
} AOSTF_REPLAY_FLAG;

void AOSTF_generate_byte_stream(const AosTransferFrame* aostf, uint8_t byte_stream[AOSTF_LEN]);

void AOSTF_setup_fill_aostf(AosTransferFrame* aostf);

void AOSTF_setup_realtime_aostf_hdr(AosTransferFrame* aostf, uint32_t counter);

void AOSTF_setup_replay_aostf_hdr(AosTransferFrame* aostf, uint32_t counter);

AOSTF_VER AOSTF_get_ver(const AosTransferFrame* aostf);

void AOSTF_set_ver(AosTransferFrame* aostf, AOSTF_VER ver);

AOSTF_SCID AOSTF_get_scid(const AosTransferFrame* aostf);

void AOSTF_set_scid(AosTransferFrame* aostf, AOSTF_SCID scid);

AOSTF_VCID AOSTF_get_vcid(const AosTransferFrame* aostf);

void AOSTF_set_vcid(AosTransferFrame* aostf, AOSTF_VCID vcid);

uint32_t AOSTF_get_aostf_counter(const AosTransferFrame* aostf);

void AOSTF_set_aostf_counter(AosTransferFrame* aostf, uint32_t counter);

AOSTF_REPLAY_FLAG AOSTF_get_replay_flag(const AosTransferFrame* aostf);

void AOSTF_set_replay_flag(AosTransferFrame* aostf, AOSTF_REPLAY_FLAG flag);

uint32_t AOSTF_get_clcw(const AosTransferFrame* aostf);

void AOSTF_set_clcw(AosTransferFrame* aostf, uint32_t clcw);

uint32_t AOSTF_calc_next_counter(uint32_t prev);

#endif
