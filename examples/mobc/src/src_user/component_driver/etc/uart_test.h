/**
 * @file
 * @brief テスト用UARTのDriver
 */
#ifndef UART_TEST_H_
#define UART_TEST_H_

#include <src_core/hal/uart.h>
#include <src_core/component_driver/driver_super.h>

/**
 * @struct UART_TEST_Info
 * @brief
 */
typedef struct
{
  uint8_t flag;
} UART_TEST_Info;

/**
 * @struct  UART_TEST_Driver
 * @brief
 */
typedef struct
{
  struct
  {
    ComponentDriverSuper super;                  //!< ComponentDriverSuper class
    UART_Config uart_config;           //!< RS422 class
  } driver;
  UART_TEST_Info info;
} UART_TEST_Driver;


// 基本関数

/**
 * @brief  UART_TEST初期化
 *
 *         UART_TEST_Driver構造体のポインタを渡すことでポートを初期化し，UART_TEST_Driverの各メンバも初期化する
 * @param  uart_test_driver: 初期化するUART_TEST_Driver構造体へのポインタ
 * @param  ch:               UART_TESTが接続されているUARTポート番号
 * @param  rx_buffers:       受信バッファ
 * @return CDS_INIT_ERR_CODE
 */
CDS_INIT_ERR_CODE UART_TEST_init(UART_TEST_Driver* uart_test_driver, uint8_t ch, CDS_StreamRecBuffer* rx_buffers[CDS_STREAM_MAX]);

/**
 * @brief  UART_TESTのデータ（テレメ）受信
 * @param  uart_test_driver: UART_TEST_Driver構造体へのポインタ
 * @return CDS_REC_ERR_CODE
 */
CDS_REC_ERR_CODE UART_TEST_rec(UART_TEST_Driver* uart_test_driver);

/**
 * @brief  UART_TESTへのコマンド送信
 * @param  uart_test_driver: UART_TEST_Driver構造体へのポインタ
 * @param  id:               Cmd id
 * @return CDS_CMD_ERR_CODE
 */
CDS_CMD_ERR_CODE UART_TEST_send(UART_TEST_Driver* uart_test_driver, uint8_t id);

#endif
