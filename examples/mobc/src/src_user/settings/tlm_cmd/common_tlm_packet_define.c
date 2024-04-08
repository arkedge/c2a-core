#pragma section REPRO
/**
 * @file
 * @brief  CTP 型の user 定義
 */
#include <src_core/tlm_cmd/common_packet/common_tlm_packet.h>
#include <src_core/tlm_cmd/ccsds/space_packet_protocol/tlm_space_packet.h>

void CTP_set_global_time(CommonTlmPacket* packet)
{
  // 何を設定するかはユーザー定義
  // TMGR_get_curret_unixtime() で現在の unixtime を入れたり, gps 時刻 を入れたり, など
  TSP_set_global_time(packet, 0.0);
}

#pragma section
