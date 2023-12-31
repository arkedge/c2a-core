/**
 * @file
 * @brief 様々な Component Driver に統一的な機能を提供する
 * @note  Cmd としての機能は， ComponentDriverUtil App (TBA) で提供される
 */
#ifndef COMPONENT_DRIVER_H_
#define COMPONENT_DRIVER_H_

// ここで， user ごとの Component Driver のリストである， CDRV_ID を定義する．これは CDS ごとに定義する．
// また，利用する HAL_HANDLER_ID (eg; CDRV_USE_HAL_HANDLER_ID_UART) を定義する．
#include <src_user/settings/component_driver/component_driver_define.h>

#include "driver_super.h"
#include "../hal/uart.h"
#include "../hal/i2c.h"
#include "../hal/spi.h"
#include "../hal/ccsds.h"

// ###### ComponentDriverSuper 向け関数 ######

/**
 * @brief 各 CDR pointer 取得関数
 * @note  `src_user/settings/component_driver/component_driver_define.c` で実装する
 * @param[in] id: CDRV_ID
 * @retval const ComponentDriverSuper*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const ComponentDriverSuper* CDRV_get_cds(CDRV_ID id);

/**
 * @brief 各 CDS_StreamConfig pointer 取得関数
 * @param[in] id: CDRV_ID
 * @param[in] stream: 各 CDRV の stream
 * @retval const CDS_StreamConfig*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const CDS_StreamConfig* CDRV_get_cdssc(CDRV_ID id, uint8_t stream);


// ###### 各 HAL HANDLER 向け関数 ######
// `c2a-core/hal` にある HAL に関しては，ここで宣言し，定義を `component_driver.c` で実装する．
// core に含まれない HAL に関しては， `src_user/settings/component_driver/component_driver_define.{c,h}` などで宣言 & 実装する．

#ifdef CDRV_USE_HAL_HANDLER_ID_UART
/**
 * @brief 各 UART_Config pointer 取得関数
 * @param[in] id: CDRV_ID
 * @retval const UART_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const UART_Config* CDRV_get_uart_config(CDRV_ID id);

/**
 * @brief 各 UART_Config pointer 取得関数
 * @param[in] ch: UART_Config.ch
 * @retval const UART_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const UART_Config* CDRV_get_uart_config_from_ch(uint8_t ch);
#endif

#ifdef CDRV_USE_HAL_HANDLER_ID_I2C
/**
 * @brief 各 I2C_Config pointer 取得関数
 * @param[in] id: CDRV_ID
 * @retval const I2C_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const I2C_Config* CDRV_get_i2c_config(CDRV_ID id);

/**
 * @brief 各 I2C_Config pointer 取得関数
 * @param[in] ch: I2C_Config.ch
 * @retval const I2C_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const I2C_Config* CDRV_get_i2c_config_from_ch(uint8_t ch);
#endif

#ifdef CDRV_USE_HAL_HANDLER_ID_SPI
/**
 * @brief 各 SPI_Config pointer 取得関数
 * @param[in] id: CDRV_ID
 * @retval const SPI_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const SPI_Config* CDRV_get_spi_config(CDRV_ID id);

/**
 * @brief 各 SPI_Config pointer 取得関数
 * @param[in] comm_ch: SPI_Config.comm_ch
 * @retval const SPI_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const SPI_Config* CDRV_get_spi_config_from_comm_ch(uint8_t comm_ch);
#endif

#ifdef CDRV_USE_HAL_HANDLER_ID_CCSDS
/**
 * @brief 各 CCSDS_Config pointer 取得関数
 * @param[in] id: CDRV_ID
 * @retval const CCSDS_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const CCSDS_Config* CDRV_get_ccsds_config(CDRV_ID id);

/**
 * @brief 各 CCSDS_Config pointer 取得関数
 * @param[in] ch: CCSDS_Config.ch
 * @retval const CCSDS_Config*: 取得するポインタ
 * @retval NULL: 不正な引数 or 見つからない
 */
const CCSDS_Config* CDRV_get_ccsds_config_from_ch(uint8_t ch);
#endif

#endif
