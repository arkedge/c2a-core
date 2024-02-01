#pragma section REPRO
#include "memory_dump.h"

#include <string.h> // for memcpy

#include "../system/time_manager/time_manager.h"
#include "../tlm_cmd/packet_handler.h"
#include "../tlm_cmd/common_cmd_packet_util.h"
#include "../tlm_cmd/common_tlm_packet.h"
// TODO: いずれは Space Packet への依存をなくしたい (CTP の依存だけにしたい)
#include "../tlm_cmd/ccsds/space_packet_protocol/space_packet.h"
#include "../tlm_cmd/ccsds/space_packet_protocol/tlm_space_packet.h"
#include "../library/endian.h"
#include "../library/result.h"

#define MEM_MAX_CTP_DATA_SIZE   (CTP_MAX_LEN - SP_PRM_HDR_LEN - TSP_SND_HDR_LEN)  //!< 1 パケットでダンプできる最大サイズ
#define MEM_MAX_DATA_SIZE       (0x00800000)      /*!< 最大ダンプ幅は 16MB
                                                       FIXME: CTCP 改修前の ADU 分割における ADUサイズ制約ままなので，見直すべき */

// ここで MEM_DUMP_TLM_APID, MEM_DUMP_TLM_ID を定義する
#include <src_user/settings/applications/memory_dump_define.h>

/**
 * @enum   MEM_ACK
 * @brief  MEM 操作の返り値
 * @note   uint8_t を想定
 */
typedef enum
{
  MEM_ACK_SUCCESS,
  MEM_ACK_NO_DATA
} MEM_ACK;

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT MEM_init_(void);

/**
 * @brief blob tlm として，メモリ領域をダウンリンクする
 * @param[in] dest_flags: Dest Flags
 * @param[in] dest_info:  Dest Info
 * @param[in] dump_num:   テレメ送出回数
 * @return CCP_CmdRet
 */
static CCP_CmdRet MEM_dump_as_blob_tlm_(ctp_dest_flags_t dest_flags,
                                        uint8_t dest_info,
                                        uint8_t dump_num);

/**
 * @brief blob tlm を生成する
 * @param[out] ctp:        生成するパケット
 * @param[in]  dest_flags: Dest Flags
 * @param[in]  dest_info:  Dest Info
 * @return MEM_ACK
 */
static MEM_ACK MEM_form_packet_(CommonTlmPacket* ctp,
                                ctp_dest_flags_t dest_flags,
                                uint8_t dest_info);

/**
 * @brief 生成するパケットの Sequence Flag を判定
 * @param[in] internal: 内部状態変数
 * @return SP_SEQ_FLAG
 */
static SP_SEQ_FLAG MEM_judge_seq_flag_(const MEM_Internal* internal);


/**
 * @brief 生成するパケットの header を判定
 * @param[out] ctp:        生成するパケット
 * @param[in]  dest_flags: Dest Flags
 * @param[in]  dest_info:  Dest Info
 * @param[in]  seq_flag:   Dest SP_SEQ_FLAG
 * @param[in]  data_len:   CTP 内部のデータサイズの長さ
 * @return void
 */
static void MEM_set_ctp_header_(CommonTlmPacket* ctp,
                                ctp_dest_flags_t dest_flags,
                                uint8_t dest_info,
                                SP_SEQ_FLAG seq_flag,
                                uint16_t data_len);

/**
 * @brief 次のパケットで用いる Sequence Count を取得
 * @param  void
 * @return Sequence Count
 */
static uint16_t MEM_get_next_seq_count_(void);

/**
 * @brief パケットを送出
 * @param[in] ctp:      送出するパケット
 * @param[in] dump_num: テレメ送出回数
 * @return void
 */
static void MEM_send_packet_(const CommonTlmPacket* ctp, uint8_t dump_num);


static MemoryDump memory_dump_;
const MemoryDump* const memory_dump = &memory_dump_;


AppInfo MEM_create_app(void)
{
  return AI_create_app_info("memdump", MEM_init_, NULL);
}


static RESULT MEM_init_(void)
{
  memset(&memory_dump_, 0x00, sizeof(MemoryDump));
  return RESULT_OK;
}


static CCP_CmdRet MEM_dump_as_blob_tlm_(ctp_dest_flags_t dest_flags,
                                        uint8_t dest_info,
                                        uint8_t dump_num)
{
  MEM_ACK ack;
  CommonTlmPacket ctp;

  if (dump_num >= 8)
  {
    // FIXME: 要検討？
    // TG_generate_tlm_ と合わせる？
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  }

  // 設定値にもとづき送出すべきパケットを構築
  ack = MEM_form_packet_(&ctp, dest_flags, dest_info);

  switch (ack)
  {
  case MEM_ACK_SUCCESS:
    // 生成したパケットを送出し、Counter を更新
    MEM_send_packet_(&ctp, dump_num);
    ++memory_dump_.internal.data_seq;
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);

  case MEM_ACK_NO_DATA:
    // すでに全領域ダンプ済みなら何もせず終了
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);

  default:
    // それ以外のエラーはないはず
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT, (uint32_t)ack);
  }
}


static MEM_ACK MEM_form_packet_(CommonTlmPacket* ctp,
                                ctp_dest_flags_t dest_flags,
                                uint8_t dest_info)
{
  const MEM_Settings* settings = &memory_dump_.settings;
  uint32_t rp;      //!< Read Pointer
  uint32_t len;
  SP_SEQ_FLAG seq_flag;

  rp = settings->begin + (memory_dump_.internal.data_seq * MEM_MAX_CTP_DATA_SIZE);
  if (rp >= settings->end)
  {
    // RP が終端に達していれば何もせず終了
    return MEM_ACK_NO_DATA;
  }

  // 残りダンプ長を計算
  len = settings->end - rp;
  if (len > MEM_MAX_CTP_DATA_SIZE)
  {
    // 1 パケットのダンプ幅を超える場合はダンプ幅に押さえる
    len = MEM_MAX_CTP_DATA_SIZE;
  }

  // Sequence Flag の種別判定
  seq_flag = MEM_judge_seq_flag_(&memory_dump_.internal);

  // ヘッダ設定
  MEM_set_ctp_header_(ctp, dest_flags, dest_info, seq_flag, len);

  // ダンプデータをコピー
  memcpy(CTP_get_user_data_head(ctp), (const void*)rp, len);

  return MEM_ACK_SUCCESS;
}


static SP_SEQ_FLAG MEM_judge_seq_flag_(const MEM_Internal* internal)
{
  if (internal-> max_data_seq == 0)
  {
    // ダンプ幅が最大パケット長以下なら Standalone packet
    return SP_SEQ_FLAG_SINGLE;
  }

  // ダンプ幅が最大パケット長を超えるなら複数パケット
  if (internal->data_seq == 0)
  {
    return SP_SEQ_FLAG_FIRST;
  }
  else if (internal->data_seq == internal->max_data_seq)
  {
    return SP_SEQ_FLAG_LAST;
  }
  else
  {
    return SP_SEQ_FLAG_CONT;
  }
}


static void MEM_set_ctp_header_(CommonTlmPacket* ctp,
                                ctp_dest_flags_t dest_flags,
                                uint8_t dest_info,
                                SP_SEQ_FLAG seq_flag,
                                uint16_t data_len)
{
  // Primary Header
  TSP_setup_primary_hdr(ctp, MEM_DUMP_TLM_APID, MEM_get_next_seq_count_(), data_len + SP_PRM_HDR_LEN + TSP_SND_HDR_LEN);
  TSP_set_seq_flag(ctp, seq_flag);

  // Secondary Header
  TSP_set_2nd_hdr_ver(ctp, TSP_2ND_HDR_VER_1);
  TSP_set_board_time(ctp, (uint32_t)(TMGR_get_master_total_cycle()));
  CTP_set_id(ctp, MEM_DUMP_TLM_ID);
  // FIXME: 他の時刻も入れる
  CTP_set_global_time(ctp);
  CTP_set_on_board_subnet_time(ctp);
  CTP_set_dest_flags(ctp, dest_flags);
  TSP_set_dest_info(ctp, dest_info);
}


static uint16_t MEM_get_next_seq_count_(void)
{
  // インクリメントした値を返すため初期値は 0xffff とする
  static uint16_t counter_ = 0xffff;
  ++counter_;
  // Sequence Count は 14 bit
  return 0x3fff & counter_;
}


static void MEM_send_packet_(const CommonTlmPacket* ctp, uint8_t dump_num)
{
  uint8_t i;

  for (i = 0; i < dump_num; ++i)
  {
    PH_analyze_tlm_packet(ctp);
  }
}


CCP_CmdRet Cmd_MEM_SET_REGION(const CommonCmdPacket* packet)
{
  uint32_t begin = CCP_get_param_from_packet(packet, 0, uint32_t);
  uint32_t end   = CCP_get_param_from_packet(packet, 1, uint32_t);
  uint32_t data_size;

  if (begin > end)
  {
    // 領域の開始と終了の大小関係が逆の場合は異常終了
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT, 0);
  }

  data_size = end - begin;
  if (data_size > MEM_MAX_DATA_SIZE)
  {
    // 指定ダンプ幅が最大量を超えている場合は異常終了。
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, 1);
  }

  memory_dump_.settings.begin = begin;
  memory_dump_.settings.end = end;
  memory_dump_.internal.data_size = data_size;
  memory_dump_.internal.data_seq = 0;
  memory_dump_.internal.max_data_seq = (uint16_t)(memory_dump_.internal.data_size / MEM_MAX_CTP_DATA_SIZE);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_MEM_DUMP_REGION(const CommonCmdPacket* packet)
{
  ctp_dest_flags_t dest_flags = (ctp_dest_flags_t)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t dest_info = CCP_get_param_from_packet(packet, 1, uint8_t);
  uint8_t dump_num = CCP_get_param_from_packet(packet, 2, uint8_t);

  return MEM_dump_as_blob_tlm_(dest_flags, dest_info, dump_num);
}


CCP_CmdRet Cmd_MEM_DUMP_SINGLE(const CommonCmdPacket* packet)
{
  CommonTlmPacket ctp;
  ctp_dest_flags_t dest_flags = (ctp_dest_flags_t)CCP_get_param_from_packet(packet, 0, uint8_t);
  uint8_t dest_info = CCP_get_param_from_packet(packet, 1, uint8_t);
  uint8_t dump_num = CCP_get_param_from_packet(packet, 2, uint8_t);
  uint32_t begin = CCP_get_param_from_packet(packet, 3, uint32_t);

  if (dump_num >= 8)
  {
    // FIXME: 要検討？
    // TG_generate_tlm_ と合わせる？
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  }

  // ヘッダ設定
  MEM_set_ctp_header_(&ctp, dest_flags, dest_info, SP_SEQ_FLAG_SINGLE, MEM_MAX_CTP_DATA_SIZE);
  // ダンプデータをコピー
  memcpy(CTP_get_user_data_head(&ctp), (const void*)begin, MEM_MAX_CTP_DATA_SIZE);
  // テレメ送出
  MEM_send_packet_(&ctp, dump_num);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_MEM_LOAD(const CommonCmdPacket* packet)
{
  uint32_t adr = CCP_get_param_from_packet(packet, 0, uint32_t);
  uint16_t data_len = CCP_get_raw_param_len(packet);

  // 指定した開始アドレスから始まる領域にデータを書き込み
  memcpy((void*)adr, CCP_get_raw_param_head(packet), data_len);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_MEM_SET_DESTINATION_FOR_COPY(const CommonCmdPacket* packet)
{
  uint32_t dest = CCP_get_param_from_packet(packet, 0, uint32_t);

  if ((dest >= memory_dump_.settings.begin) && (dest < memory_dump_.settings.end))
  {
    // 宛先アドレスが領域内部に含まれる場合
    // これを認めると処理が複雑になるので禁止する
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  }

  // 宛先アドレスを設定し，RP を領域先頭に合わせる
  memory_dump_.settings.copy_dest = dest;
  memory_dump_.internal.copy_rp = memory_dump_.settings.begin;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_MEM_COPY_REGION(const CommonCmdPacket* packet)
{
  uint32_t copy_width = CCP_get_param_from_packet(packet, 0, uint32_t);
  uint32_t wp;    //!< Write Pointer


  if (memory_dump_.internal.copy_rp >= memory_dump_.settings.end)
  {
    // 既に領域全体の読み出しが完了している場合
    // 処理は行わず正常終了する
    return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
  }

  if ((memory_dump_.internal.copy_rp + copy_width) > memory_dump_.settings.end)
  {
    // 指定コピー幅でコピーすると領域外に出てしまう場合
    // コピー結果が領域終端に一致するようコピー幅をクリップ
    copy_width = memory_dump_.settings.end - memory_dump_.internal.copy_rp;
  }

  wp = memory_dump_.settings.copy_dest + (memory_dump_.internal.copy_rp - memory_dump_.settings.begin);

  // 指定されたコピー幅だけ領域をコピーし，RP を更新
  memcpy((uint8_t*)wp, (const uint8_t*)memory_dump_.internal.copy_rp, copy_width);
  memory_dump_.internal.copy_rp += copy_width;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
