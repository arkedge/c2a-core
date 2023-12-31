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
  CDRV_ID_MOBC,
  CDRV_ID_MAX
} CDRV_ID;

#endif
