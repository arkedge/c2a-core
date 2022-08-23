#pragma section REPRO
/**
 * @file
 * @brief コンポ間通信などで標準的に使う データリンク層 の EB90 Frame
 */
#include "eb90_frame_for_driver_super.h"
#include "../../Library/endian_memcpy.h"
#include "../../Library/crc.h"

const uint8_t EB90_FRAME_kStx[EB90_FRAME_STX_SIZE] = {EB90_FRAME_STX_1ST_BYTE, EB90_FRAME_STX_2ND_BYTE};
const uint8_t EB90_FRAME_kEtx[EB90_FRAME_ETX_SIZE] = {EB90_FRAME_ETX_1ST_BYTE, EB90_FRAME_ETX_2ND_BYTE};


uint16_t EB90_FRAME_get_packet_length_from_dssc(const DS_StreamConfig* p_stream_config)
{
  uint16_t length;
  endian_memcpy(&length, &(DSSC_get_rx_frame(p_stream_config)[2]), EB90_FRAME_LEN_SIZE);
  return length;
}


const uint8_t* EB90_FRAME_get_packet_head_from_dssc(const DS_StreamConfig* p_stream_config)
{
  return &(DSSC_get_rx_frame(p_stream_config)[EB90_FRAME_HEADER_SIZE]);
}


uint16_t EB90_FRAME_get_crc_from_dssc(const DS_StreamConfig* p_stream_config)
{
  uint16_t crc;
  uint16_t offset = EB90_FRAME_HEADER_SIZE + EB90_FRAME_get_packet_length_from_dssc(p_stream_config);
  endian_memcpy(&crc, &(DSSC_get_rx_frame(p_stream_config)[offset]), EB90_FRAME_CRC_SIZE);
  return crc;
}


uint8_t EB90_FRAME_is_valid_crc_of_dssc(const DS_StreamConfig* p_stream_config)
{
  // TODO: CRC を IBM から CCITT に変えてから実装する
  return 1;
}


uint16_t EB90_FRAME_calc_crc(const uint8_t* data, size_t len)
{
  return crc_16_ibm_right(0x0000, (unsigned char*)data, len, 0);    // FIXME: キャストなくしたい: https://github.com/ut-issl/c2a-core/issues/392
}

#pragma section