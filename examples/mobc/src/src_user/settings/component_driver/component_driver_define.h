/**
 * @file
 * @brief Component Driver の user 定義
 */
#ifndef COMPONENT_DRIVER_DEFINE_H_
#define COMPONENT_DRIVER_DEFINE_H_

/**
 * @enum  CDRV_ID
 * @brief Component Driver のリスト． CDS ごとに定義する．
 * @note  uint8_tを想定
 */
typedef enum
{
  CDRV_ID_AOBC,
  CDRV_ID_GS_CCSDS,
  CDRV_ID_GS_UART,
  CDRV_ID_UART_TEST,
  CDRV_ID_MAX
} CDRV_ID;


#define CDRV_USE_HAL_HANDLER_ID_UART    //!< HAL_HANDLER_ID_UART を使うか
#define CDRV_USE_HAL_HANDLER_ID_CCSDS   //!< HAL_HANDLER_ID_CCSDS を使うか
#define CDRV_USE_HAL_HANDLER_ID_I2C     //!< HAL_HANDLER_ID_I2C を使うか
#define CDRV_USE_HAL_HANDLER_ID_SPI     //!< HAL_HANDLER_ID_SPI を使うか

#endif
