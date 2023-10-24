#pragma section REPRO
/**
 * @file
 * @brief コンポ間通信などで標準的に使う ネットワーク層 の EB90 Packet
 * @note  一般的には，データリンク層は EB90 Frame を使うことを想定
 * @note  Tlm か Cmd かはコンテキストで読み替える
 * @note  データリンク層は CDS_StreamConfig.data_link_layer_ で規定する
 */
#include "cdrv_eb90_packet.h"
#include "cdrv_eb90_frame.h"
#include "../library/endian.h"

// FIXME: CDS_StreamConfig.data_link_layer_ をちゃんと見る！

uint32_t CDRV_EB90_PACKET_get_version(const CDS_StreamConfig* p_stream_config)
{
  // TODO: 現状は Version 番号が 8bit のもののみしか対応していない
  return (uint32_t)(CDRV_EB90_FRAME_get_packet_head(p_stream_config)[0]);
}


uint32_t CDRV_EB90_PACKET_get_count(const CDS_StreamConfig* p_stream_config)
{
  const uint32_t version = CDRV_EB90_PACKET_get_version(p_stream_config);

  if (version == 1)
  {
    return (uint32_t)(CDRV_EB90_FRAME_get_packet_head(p_stream_config)[1]);
  }
  else
  {
    return 0;
  }
}


uint32_t CDRV_EB90_PACKET_get_id(const CDS_StreamConfig* p_stream_config)
{
  const uint32_t version = CDRV_EB90_PACKET_get_version(p_stream_config);

  if (version == 1)
  {
    uint16_t id;
    ENDIAN_memcpy(&id, &(CDRV_EB90_FRAME_get_packet_head(p_stream_config)[2]), sizeof(uint16_t));
    return (uint32_t)id;
  }
  else
  {
    return 0;
  }
}


const uint8_t* CDRV_EB90_PACKET_get_user_data_head(const CDS_StreamConfig* p_stream_config)
{
  const uint32_t version = CDRV_EB90_PACKET_get_version(p_stream_config);

  if (version == 1)
  {
    return &(CDRV_EB90_FRAME_get_packet_head(p_stream_config)[4]);
  }
  else
  {
    return NULL;
  }
}

#pragma section
