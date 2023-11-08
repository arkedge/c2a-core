/**
 * @file
 * @brief Ground Station との通信用の Driver
 */
#ifndef GS_H_
#define GS_H_

#include <src_core/tlm_cmd/packet_handler.h>
#include <src_core/component_driver/driver_super.h>
#include <src_core/hal/uart.h>
#include <src_core/hal/ccsds.h>
#include "../../hal/ccsds_user.h"
#include "../../tlm_cmd/ccsds/aostf.h"
#include "gs_validate.h"

#define GS_RX_HEADER_NUM (3)

/**
 * @enum  GS_PORT_TYPE
 * @brief GS の通信ポートは CCSDS と UART の 2 つある
 * @note  uint8_t を想定
 */
typedef enum
{
  GS_PORT_TYPE_CCSDS, //!< CCSDS
  GS_PORT_TYPE_UART,  //!< UART
  GS_PORT_TYPE_NUM    //!< port 数
} GS_PORT_TYPE;

/**
 * @struct GS_Info
 * @brief  ドライバーの各状態の記録
 */
typedef struct
{
  struct
  {
    CDS_ERR_CODE rec_status;                  //!< ComponentDriverSuper からの受信結果
    int ret_from_hal_rx;                      //!< UART or CCSDS からの返り値
    CCP_DEST_TYPE last_dest_type;             //!< 最後に受信したパケットの dest type
    cycle_t last_rec_time;                    //!< 最後に受信した時刻
    PH_ACK cmd_ack;                           //!< 受信した CMD の実行結果

    struct
    {
      TCTF_TYPE last_rec_tctf_type;             //!< 最後に受信した TC Transfer Frame のタイプ
      GS_VALIDATE_ERR ad_rec_status;            //!< AD CMD を受信したときの analyze 結果
      GS_VALIDATE_ERR bc_rec_status;            //!< BC CMD を受信したときの analyze 結果
      GS_VALIDATE_ERR bd_rec_status;            //!< BD CMD を受信したときの analyze 結果
    } tctf;
  } rx;

  struct
  {
    cycle_t send_cycle;                       //!< 最後に送信したときの時刻
    AOSTF_VCID vcid;                           //!< 送信した TLM のタイプ
    uint32_t aostf_counter;                    //!< AOSTF counter
  } tx;
} GS_Info;

/**
 * @struct GS_Driver
 * @brief  GroundStation Driver
 */
typedef struct
{
  // CCSDS 側の Driver
  struct
  {
    ComponentDriverSuper  super;
    CCSDS_Config ccsds_config;
  } driver_ccsds;

  // UART 側の Driver
  struct
  {
    ComponentDriverSuper super;
    UART_Config uart_config;
    uint8_t is_tlm_on;            //!< UART に TLM を流すかどうか, CCSDS では許容でも UART に TLM を送りすぎると詰まってしまうため
  } driver_uart;

  GS_Info info[GS_PORT_TYPE_NUM]; //!< CCSDS 用と UART 用 2 つ
  const GS_Info* latest_info;
  CCSDS_Info ccsds_info;
  GS_PORT_TYPE tlm_tx_port_type;
} GS_Driver;

/**
 * @brief Driver の初期化
 * @param[in] gs_driver:        ドライバー
 * @param[in] uart_ch:          有線通信時の CH
 * @param[in] ccsds_rx_buffers: CCSDS 用受信バッファ
 * @param[in] uart_rx_buffers:  UART 用受信バッファ
 * @return CDS_INIT_ERR_CODE
 */
CDS_INIT_ERR_CODE GS_init(GS_Driver* gs_driver,
                          uint8_t uart_ch,
                          CDS_StreamRecBuffer* ccsds_rx_buffers[CDS_STREAM_MAX],
                          CDS_StreamRecBuffer* uart_rx_buffers[CDS_STREAM_MAX]);

/**
 * @brief 地上から CMD を受信する. 形式は TC Transer Frame
 * @param[in] gs_driver: ドライバー
 * @return CDS_REC_ERR_CODE
 */
CDS_REC_ERR_CODE GS_rec_tctf(GS_Driver* gs_driver);

/**
 * @brief 地上に向けて TLM を送信. 形式は AOSTF
 * @note  CDS_send_general_cmd が使われているが, これは DS は MOBC コンポ間を想定しているため, MOBC から見るとコンポに cmd を送信している様に見える, が 今回は MOBC から地上に TLM を送信している
 * @note TLM 送信, 形式は AOSTF
 * @param[in] gs_driver: ドライバー
 * @param[in] aostf:      送信する AOSTF. 場合によってはそのまま DS に渡すので， local変数ではなくstaticな変数を渡すこと
 * @return CDS_CMD_ERR_CODE: 送信結果
 */
CDS_CMD_ERR_CODE GS_send_aostf(GS_Driver* gs_driver, const AOSTF* aostf);

#endif
