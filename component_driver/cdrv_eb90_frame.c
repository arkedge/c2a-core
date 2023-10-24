#pragma section REPRO
/**
 * @file
 * @brief コンポ間通信などで標準的に使う データリンク層 の EB90 Frame
 */
#include "cdrv_eb90_frame.h"
#include "../library/endian.h"
#include "../library/crc.h"

const uint8_t CDRV_EB90_FRAME_kStx[CDRV_EB90_FRAME_STX_SIZE] = {CDRV_EB90_FRAME_STX_1ST_BYTE, CDRV_EB90_FRAME_STX_2ND_BYTE};
const uint8_t CDRV_EB90_FRAME_kEtx[CDRV_EB90_FRAME_ETX_SIZE] = {CDRV_EB90_FRAME_ETX_1ST_BYTE, CDRV_EB90_FRAME_ETX_2ND_BYTE};


uint16_t CDRV_EB90_FRAME_get_packet_length(const CDS_StreamConfig* p_stream_config)
{
  uint16_t length;
  ENDIAN_memcpy(&length, &(CDSSC_get_rx_frame(p_stream_config)[2]), CDRV_EB90_FRAME_LEN_SIZE);
  return length;
}


const uint8_t* CDRV_EB90_FRAME_get_packet_head(const CDS_StreamConfig* p_stream_config)
{
  return &(CDSSC_get_rx_frame(p_stream_config)[CDRV_EB90_FRAME_HEADER_SIZE]);
}


uint16_t CDRV_EB90_FRAME_get_crc(const CDS_StreamConfig* p_stream_config)
{
  uint16_t crc;
  uint16_t offset = CDRV_EB90_FRAME_HEADER_SIZE + CDRV_EB90_FRAME_get_packet_length(p_stream_config);
  ENDIAN_memcpy(&crc, &(CDSSC_get_rx_frame(p_stream_config)[offset]), CDRV_EB90_FRAME_CRC_SIZE);
  return crc;
}


uint8_t CDRV_EB90_FRAME_is_valid_crc(const CDS_StreamConfig* p_stream_config)
{
  uint16_t len = CDRV_EB90_FRAME_get_packet_length(p_stream_config);
  const uint8_t* head = CDRV_EB90_FRAME_get_packet_head(p_stream_config);
  return (EB90_FRAME_calc_crc(head, len + CDRV_EB90_FRAME_CRC_SIZE) == 0) ? 1 : 0;
}


uint16_t EB90_FRAME_calc_crc(const uint8_t* data, size_t len)
{
  // CRC-16/CCITT-FALSE (CRC-16/AUTOSAR, CRC-16/IBM-3740 とも)
  // https://reveng.sourceforge.io/crc-catalogue/16.htm
  return CRC_calc_crc_16_ccitt_left(0xffff, data, len, 0);
}

#pragma section
