/**
 * @file
 * @brief CCSDS で規定される M_PDU (Multiplexing Protocol Data Unit) の実装
 * @note  packet 構造
 *        |---------+-------+-------+--------------------------|
 *        | Pos     | Pos   | Size  | Name                     |
 *        | [octet] | [bit] | [bit] |                          |
 *        |---------+-------+-------+--------------------------|
 *        | === Header ========================================|
 *        |---------+-------+-------+--------------------------|
 *        |       0 |     0 |     5 | Reserved Spare           |
 *        |       0 |     5 |    11 | First Header Pointer     |
 *        |---------+-------+-------+--------------------------|
 *        | === Packet Zone ===================================|
 *        |---------+-------+-------+--------------------------|
 *        |       2 |     0 |     * | いくつかの Space Packet  |
 *        |---------+-------+-------+--------------------------|
 */
#ifndef MULTIPLEXING_PROTOCOL_DATA_UNIT_H_
#define MULTIPLEXING_PROTOCOL_DATA_UNIT_H_

#include <stddef.h>
#include <stdint.h>

#define M_PDU_HEADER_SIZE (2u)
#define M_PDU_DATA_SIZE (432u)
#define M_PDU_LEN (M_PDU_HEADER_SIZE + M_PDU_DATA_SIZE)

typedef struct
{
  uint8_t header[M_PDU_HEADER_SIZE];
  uint8_t data[M_PDU_DATA_SIZE];
} M_PDU;

typedef enum
{
  M_PDU_PTR_NO_HDR = 0x07ff, // 11111111111b
  M_PDU_PTR_IDLE = 0x7fe // 11111111110b
} M_PDU_PTR;

void M_PDU_generate_byte_stream(const M_PDU* m_pdu, uint8_t byte_stream[M_PDU_LEN]);

void M_PDU_setup_idle_m_pdu(M_PDU* m_pdu);

uint16_t M_PDU_get_1st_hdr_ptr(const M_PDU* m_pdu);

void M_PDU_set_1st_hdr_ptr(M_PDU* m_pdu, uint16_t ptr);

const uint8_t* M_PDU_get_data_head(const M_PDU* m_pdu);

void M_PDU_set_data(M_PDU* m_pdu, const uint8_t* data, size_t offset, size_t len);

#endif
