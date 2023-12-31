#pragma section REPRO
/**
 * @file
 * @brief  AOBC の Component Service と AOBC の cmd dispatcher
 * @note   C2A 間通信のデモ用であり， AOBC も C2A 搭載を仮定する
 */

#include "csrv_aobc.h"
#include "../../component_driver/aocs/aobc.h"
#include "../../tlm_cmd/user_packet_handler.h"
#include <src_core/library/print.h>
#include <src_core/tlm_cmd/common_cmd_packet_util.h>
#include "../../settings/port_config.h"
#include "../../settings/component_driver/driver_buffer_define.h"
#include <src_core/library/result.h>

static RESULT CSRV_AOBC_init_(void);
static RESULT CSRV_AOBC_update_(void);

static RESULT CSRV_AOBC_cmd_dispatcher_init_(void);
static RESULT CSRV_AOBC_cmd_dispatcher_(void);

static AOBC_Driver aobc_driver_;
const AOBC_Driver* const aobc_driver = &aobc_driver_;

static CommandDispatcher CSRV_AOBC_cdis_;
const CommandDispatcher* const CSRV_AOBC_cdis = &CSRV_AOBC_cdis_;

// バッファ
static CDS_StreamRecBuffer CSRV_AOBC_rx_buffer_;
static uint8_t CSRV_AOBC_rx_buffer_allocation_[CDS_STREAM_REC_BUFFER_SIZE_DEFAULT];


AppInfo CSRV_AOBC_update(void)
{
  return AI_create_app_info("update_AOBC", CSRV_AOBC_init_, CSRV_AOBC_update_);
}


static RESULT CSRV_AOBC_init_(void)
{
  CDS_ERR_CODE ret1;
  CDS_INIT_ERR_CODE ret2;
  RESULT err = RESULT_OK;

  ret1 = CDS_init_stream_rec_buffer(&CSRV_AOBC_rx_buffer_,
                                    CSRV_AOBC_rx_buffer_allocation_,
                                    sizeof(CSRV_AOBC_rx_buffer_allocation_));
  if (ret1 != CDS_ERR_CODE_OK)
  {
    Printf("AOBC buffer init Failed ! %d \n", ret1);
    err = RESULT_ERR;
  }

  ret2 = AOBC_init(&aobc_driver_, PORT_CH_RS422_AOBC, &CSRV_AOBC_rx_buffer_);
  if (ret2 != CDS_INIT_OK)
  {
    Printf("AOBC init Failed ! %d \n", ret2);
    err = RESULT_ERR;
  }

  return err;
}


static RESULT CSRV_AOBC_update_(void)
{
  int ret;
  ret = AOBC_rec(&aobc_driver_);

  // [TODO]
  // 必要があればここに処理を
  (void)ret;
  return RESULT_OK;
}


AppInfo CSRV_AOBC_cmd_dispatcher(void)
{
  return AI_create_app_info("aobc_cdis",
                            CSRV_AOBC_cmd_dispatcher_init_,
                            CSRV_AOBC_cmd_dispatcher_);
}


static RESULT CSRV_AOBC_cmd_dispatcher_init_(void)
{
  return CDIS_init(&CSRV_AOBC_cdis_, &PH_aobc_cmd_list);
}


static RESULT CSRV_AOBC_cmd_dispatcher_(void)
{
  CDIS_dispatch_command(&CSRV_AOBC_cdis_);
  return RESULT_OK;
}


CCP_CmdRet CSRV_AOBC_dispatch_command(const CommonCmdPacket* packet)
{
  CDS_CMD_ERR_CODE ret;
  CommonCmdPacket* pckt = (CommonCmdPacket*)packet; // const_cast
  // ここで CCP_DEST_TYPE を宛先で受理できるように変更（なので const cast が発生している．．．）

  switch (CCP_get_dest_type(pckt))
  {
  case CCP_DEST_TYPE_TO_ME:       // FALL THROUGH
  case CCP_DEST_TYPE_TO_MOBC:     // CCP_DEST_TYPE_TO_APID の追加に伴い deprecated
    // MOBC のキューに溜まった後に実行されたもの
    // 配送先 OBC では MOBC 側の TL などの影響は受けないはずなので RTC へ変換
    CCP_set_exec_type(pckt, CCP_EXEC_TYPE_RT);
    break;
  default:
    // MOBC のキューに入らず直接転送
    // そのままの EXEC_TYPE で転送．なにもしない
    break;
  }

  // 配送先 OBC が最終到達地なので
  CCP_set_dest_type(pckt, CCP_DEST_TYPE_TO_ME);

  ret = AOBC_send_cmd(&aobc_driver_, pckt);
  // FIXME: ここも一旦握りつぶす（後で直す）
  return CDS_conv_cmd_err_to_ccp_cmd_ret(ret);
}


CCP_CmdRet Cmd_CSRV_AOBC_CDIS_CLEAR_ALL_REALTIME(const CommonCmdPacket* packet)
{
  (void)packet;

  CDIS_clear_command_list(&CSRV_AOBC_cdis_);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_CSRV_AOBC_CDIS_CLEAR_ERR_LOG(const CommonCmdPacket* packet)
{
  (void)packet;

  // 記録されたエラー情報を解除
  CDIS_clear_error_status(&CSRV_AOBC_cdis_);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
