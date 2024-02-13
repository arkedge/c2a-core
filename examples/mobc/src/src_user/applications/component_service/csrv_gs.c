#pragma section REPRO
/**
 * @file
 * @brief GS Driver のインスタンス化
 */
#include "csrv_gs.h"
#include <src_core/tlm_cmd/packet_handler.h>
#include <src_core/tlm_cmd/common_packet/common_cmd_packet_util.h>
#include <src_core/library/print.h>
#include <src_core/library/result.h>
#include "../../component_driver/com/gs_validate.h"
#include "../../settings/port_config.h"
#include "../../settings/component_driver/driver_buffer_define.h"

static RESULT CSRV_GS_init_(void);

// 以下 init と update の定義
static RESULT CSRV_GS_cmd_packet_handler_app_init_(void);
static RESULT CSRV_GS_cmd_packet_handler_app_(void);

static RESULT CSRV_GS_rt_tlm_packet_handler_app_init_(void);
static RESULT CSRV_GS_rt_tlm_packet_handler_app_(void);
static RESULT CSRV_GS_rp_tlm_packet_handler_app_init_(void);
static RESULT CSRV_GS_rp_tlm_packet_handler_app_(void);

static void CSRV_GS_set_t2m_flush_interval_(cycle_t flush_interval, CSRV_GS_TlmPacketHandler* gs_tlm_packet_handler);

static GS_Driver gs_driver_;
const GS_Driver* const gs_driver = &gs_driver_;

static CSRV_GS_TlmPacketHandler CSRV_GS_rt_tlm_packet_handler_;
const CSRV_GS_TlmPacketHandler* const CSRV_GS_rt_tlm_packet_handler = &CSRV_GS_rt_tlm_packet_handler_;
static CSRV_GS_TlmPacketHandler CSRV_GS_rp_tlm_packet_handler_;
const CSRV_GS_TlmPacketHandler* const CSRV_GS_rp_tlm_packet_handler = &CSRV_GS_rp_tlm_packet_handler_;

// バッファ
static CDS_StreamRecBuffer CSRV_GS_ccsds_rx_buffer_[GS_RX_HEADER_NUM];
static uint8_t CSRV_GS_ccsds_rx_buffer_allocation_[GS_RX_HEADER_NUM][CDS_STREAM_REC_BUFFER_SIZE_DEFAULT];
static CDS_StreamRecBuffer CSRV_GS_uart_rx_buffer_[GS_RX_HEADER_NUM];
static uint8_t CSRV_GS_uart_rx_buffer_allocation_[GS_RX_HEADER_NUM][CDS_STREAM_REC_BUFFER_SIZE_DEFAULT];


static RESULT CSRV_GS_init_(void)
{
  int stream;
  CDS_INIT_ERR_CODE ret;
  CDS_StreamRecBuffer* ccsds_rx_buffers[CDS_STREAM_MAX];
  CDS_StreamRecBuffer* uart_rx_buffers[CDS_STREAM_MAX];
  CDS_nullify_stream_rec_buffers(ccsds_rx_buffers);
  CDS_nullify_stream_rec_buffers(uart_rx_buffers);

  // GS_RX_HEADER_NUM > CDS_STREAM_MAX のアサーションは gs.c でやっているのでここではしない
  for (stream = 0; stream < GS_RX_HEADER_NUM; ++stream)
  {
    CDS_ERR_CODE ret1;
    CDS_ERR_CODE ret2;
    ret1 = CDS_init_stream_rec_buffer(&CSRV_GS_ccsds_rx_buffer_[stream],
                                      CSRV_GS_ccsds_rx_buffer_allocation_[stream],
                                      sizeof(CSRV_GS_ccsds_rx_buffer_allocation_[stream]));
    ret2 = CDS_init_stream_rec_buffer(&CSRV_GS_uart_rx_buffer_[stream],
                                      CSRV_GS_uart_rx_buffer_allocation_[stream],
                                      sizeof(CSRV_GS_uart_rx_buffer_allocation_[stream]));
    if (ret1 != CDS_ERR_CODE_OK || ret2 != CDS_ERR_CODE_OK)
    {
      Printf("GS buffer init Failed ! %d, %d \n", ret1, ret2);
      return RESULT_ERR;
    }
    ccsds_rx_buffers[stream] = &CSRV_GS_ccsds_rx_buffer_[stream];
    uart_rx_buffers[stream]  = &CSRV_GS_uart_rx_buffer_[stream];
  }

  ret = GS_init(&gs_driver_, PORT_CH_RS422_MOBC_EXT, ccsds_rx_buffers, uart_rx_buffers);

  if (ret != CDS_INIT_OK)
  {
    Printf("!! GS Init Error %d !!\n", ret);
    return RESULT_ERR;
  }

  return RESULT_OK;
}

AppInfo CSRV_GS_cmd_packet_handler_app(void)
{
  return AI_create_app_info("GS_CMD", CSRV_GS_cmd_packet_handler_app_init_, CSRV_GS_cmd_packet_handler_app_);
}

AppInfo CSRV_GS_rt_tlm_packet_handler_app(void)
{
  return AI_create_app_info("GS_RT_TLM", CSRV_GS_rt_tlm_packet_handler_app_init_, CSRV_GS_rt_tlm_packet_handler_app_);
}

AppInfo CSRV_GS_rp_tlm_packet_handler_app(void)
{
  return AI_create_app_info("GS_RP_TLM", CSRV_GS_rp_tlm_packet_handler_app_init_, CSRV_GS_rp_tlm_packet_handler_app_);
}

static RESULT CSRV_GS_cmd_packet_handler_app_init_(void)
{
  CSRV_GS_init_();
  return RESULT_OK;
}

static RESULT CSRV_GS_cmd_packet_handler_app_(void)
{
  GS_rec_tctf(&gs_driver_);
  // TODO: エラー処理
  return RESULT_OK;
}

static RESULT CSRV_GS_rt_tlm_packet_handler_app_init_(void)
{
  T2M_initialize(&CSRV_GS_rt_tlm_packet_handler_.tc_packet_to_m_pdu);
  return RESULT_OK;
}

static RESULT CSRV_GS_rt_tlm_packet_handler_app_(void)
{
  int i;

  // 本当なら max(今の FIFO の空き, 残り時間で実行可能な数) とかしたい
  for (i = 0; i < CCSDS_FIFO_SIZE; ++i)
  {
    T2M_ACK ack = T2M_form_m_pdu(&CSRV_GS_rt_tlm_packet_handler_.tc_packet_to_m_pdu,
                                 &PH_rt_tlm_list,
                                 &CSRV_GS_rt_tlm_packet_handler_.aostf.m_pdu);
    if (ack != T2M_SUCCESS) return RESULT_OK;

    // Realtime AOSTF カウンタの設定
    AOSTF_setup_realtime_aostf_hdr(&CSRV_GS_rt_tlm_packet_handler_.aostf, CSRV_GS_rt_tlm_packet_handler_.aostf_counter);
    CSRV_GS_rt_tlm_packet_handler_.aostf_counter = AOSTF_calc_next_counter(CSRV_GS_rt_tlm_packet_handler_.aostf_counter);

    // CLCW の設定
    // CMD の VCID と TLM の VCID は独立で関係がない
    // TLM の VCID 種別（Realtime, Replay)によらず CLCW を設定して良い
    // CLCW が対応する CMD の VCID は CLCW の内部で指定される
    AOSTF_set_clcw(&CSRV_GS_rt_tlm_packet_handler_.aostf, GS_form_clcw());

    // 完成した AOSTF を RT AOSTF として送出
    GS_send_aostf(&gs_driver_, &CSRV_GS_rt_tlm_packet_handler_.aostf);
  }

  return RESULT_OK;
}

static RESULT CSRV_GS_rp_tlm_packet_handler_app_init_(void)
{
  T2M_initialize(&CSRV_GS_rp_tlm_packet_handler_.tc_packet_to_m_pdu);
  return RESULT_OK;
}

static RESULT CSRV_GS_rp_tlm_packet_handler_app_(void)
{
  int i;

  // 本当なら max(今の FIFO の空き, 残り時間で実行可能な数) とかしたい
  for (i = 0; i < CCSDS_FIFO_SIZE; ++i)
  {
    T2M_ACK ack = T2M_form_m_pdu(&CSRV_GS_rp_tlm_packet_handler_.tc_packet_to_m_pdu,
                                 &PH_rp_tlm_list,
                                 &CSRV_GS_rp_tlm_packet_handler_.aostf.m_pdu);
    if (ack != T2M_SUCCESS) return RESULT_OK;

    // Replay AOSTF カウンタの設定
    AOSTF_setup_replay_aostf_hdr(&CSRV_GS_rp_tlm_packet_handler_.aostf, CSRV_GS_rp_tlm_packet_handler_.aostf_counter);
    CSRV_GS_rp_tlm_packet_handler_.aostf_counter = AOSTF_calc_next_counter(CSRV_GS_rp_tlm_packet_handler_.aostf_counter);

    // CLCW の設定
    // CMD の VCIDと TLM の VCID は独立で関係がない
    // TLM の VCID 種別（Realtime, Replay) によらず CLCW を設定して良い
    // CLCW が対応する CMD の VCID は CLCW の内部で指定される
    AOSTF_set_clcw(&CSRV_GS_rp_tlm_packet_handler_.aostf, GS_form_clcw());

    // 完成した AOSTF を RP AOSTF として送出
    GS_send_aostf(&gs_driver_, &CSRV_GS_rp_tlm_packet_handler_.aostf);
  }

  return RESULT_OK;
}

static void CSRV_GS_set_t2m_flush_interval_(cycle_t flush_interval, CSRV_GS_TlmPacketHandler* gs_tlm_packet_handler)
{
  gs_tlm_packet_handler->tc_packet_to_m_pdu.flush_interval = flush_interval;
}

CCP_CmdRet Cmd_CSRV_GS_DRIVER_RESET(const CommonCmdPacket* packet)
{
  (void)packet;
  if (CSRV_GS_init_() != RESULT_OK) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_CONTEXT);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_SET_RT_FLUSH_INTERVAL(const CommonCmdPacket* packet)
{
  cycle_t flush_interval;
  ENDIAN_memcpy(&flush_interval, CCP_get_param_head(packet), sizeof(cycle_t));

  CSRV_GS_set_t2m_flush_interval_(flush_interval, &CSRV_GS_rt_tlm_packet_handler_);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_SET_RP_FLUSH_INTERVAL(const CommonCmdPacket* packet)
{
  cycle_t flush_interval;
  ENDIAN_memcpy(&flush_interval, CCP_get_param_head(packet), sizeof(cycle_t));

  CSRV_GS_set_t2m_flush_interval_(flush_interval, &CSRV_GS_rp_tlm_packet_handler_);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_SET_FARM_PW(const CommonCmdPacket* packet)
{
  uint8_t pw = CCP_get_param_head(packet)[0];
  if (pw < 1 || pw > 127) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  GS_set_farm_pw(pw);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_SET_INFO(const CommonCmdPacket* packet)
{
  uint8_t which = CCP_get_param_head(packet)[0];
  if (which >= GS_PORT_TYPE_NUM) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  gs_driver_.latest_info = &gs_driver_.info[(GS_PORT_TYPE)which];
  gs_driver_.tlm_tx_port_type = (GS_PORT_TYPE)which;

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_CCSDS_GET_BUFFER(const CommonCmdPacket* packet)
{
  (void)packet;
  gs_driver_.ccsds_info.buffer_num = CCSDS_get_buffer_num();

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_CCSDS_SET_RATE(const CommonCmdPacket* packet)
{
  uint32_t ui_rate = (uint32_t)CCP_get_param_head(packet)[0];
  if (ui_rate == 0) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);

  CCSDS_set_rate(ui_rate, &gs_driver_.driver_ccsds.ccsds_config);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_UART_TLM_ON(const CommonCmdPacket* packet)
{
  (void)packet;
  gs_driver_.driver_uart.is_tlm_on = 1;

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_CSRV_GS_UART_TLM_OFF(const CommonCmdPacket* packet)
{
  (void)packet;
  gs_driver_.driver_uart.is_tlm_on = 0;

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
