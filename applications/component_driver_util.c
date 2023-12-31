#pragma section REPRO
/**
 * @file
 * @brief 様々な Component Driver に統一的な Cmd を提供する
 */
#include "component_driver_util.h"
#include "../component_driver/component_driver.h"
#include <stddef.h>
#include <string.h>

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT CDRV_UTIL_init_(void);

/**
 * @brief  CCP の digest を計算
 * @param  ccp: digest を計算する CCP
 * @return digest
 */
static uint16_t CDRV_UTIL_calc_digest_(const CommonCmdPacket* ccp);

static ComponentDriverUtil component_driver_util_;
const ComponentDriverUtil* const component_driver_util = &component_driver_util_;


AppInfo CDRV_UTIL_create_app(void)
{
  return AI_create_app_info("component_driver_util", CDRV_UTIL_init_, NULL);
}


static RESULT CDRV_UTIL_init_(void)
{
  memset(&component_driver_util_, 0x00, sizeof(ComponentDriverUtil));
  return RESULT_OK;
}


#pragma section
