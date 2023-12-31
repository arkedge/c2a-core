#pragma section REPRO
/**
 * @file
 * @brief 様々な Component Driver に統一的な機能を提供する
 * @note  Cmd としての機能は， ComponentDriverUtil App (TBA) で提供される
 */
#include "component_driver.h"
#include <stddef.h>

const CDS_StreamConfig* CDRV_get_cdssc(CDRV_ID id, uint8_t stream)
{
  const ComponentDriverSuper* cds;
  if (id >= CDRV_ID_MAX) return NULL;
  cds = CDRV_get_cds(id);
  if (cds == NULL) return NULL;
  return &(cds->stream_config[stream]);
}


#ifdef CDRV_USE_HAL_HANDLER_ID_UART
const UART_Config* CDRV_get_uart_config(CDRV_ID id)
{
  const ComponentDriverSuper* cds;
  cds = CDRV_get_cds(id);
  if (cds == NULL) return NULL;

  if (cds->hal_handler_id != HAL_HANDLER_ID_UART) return NULL;

  return (const UART_Config*)cds->hal_config;
}


const UART_Config* CDRV_get_uart_config_from_ch(uint8_t ch)
{
  CDRV_ID id;
  for (id = 0; id < CDRV_ID_MAX; id++)
  {
    const UART_Config* config = CDRV_get_uart_config(id);
    if (config == NULL) continue;
    if (config->ch == ch) return config;
  }
  return NULL;
}
#endif


#ifdef CDRV_USE_HAL_HANDLER_ID_I2C
const I2C_Config* CDRV_get_i2c_config(CDRV_ID id)
{
  const ComponentDriverSuper* cds;
  cds = CDRV_get_cds(id);
  if (cds == NULL) return NULL;

  if (cds->hal_handler_id != HAL_HANDLER_ID_I2C) return NULL;

  return (const I2C_Config*)cds->hal_config;
}


const I2C_Config* CDRV_get_i2c_config_from_ch(uint8_t ch)
{
  CDRV_ID id;
  for (id = 0; id < CDRV_ID_MAX; id++)
  {
    const I2C_Config* config = CDRV_get_i2c_config(id);
    if (config == NULL) continue;
    if (config->ch == ch) return config;
  }
  return NULL;
}
#endif


#ifdef CDRV_USE_HAL_HANDLER_ID_SPI
const SPI_Config* CDRV_get_spi_config(CDRV_ID id)
{
  const ComponentDriverSuper* cds;
  cds = CDRV_get_cds(id);
  if (cds == NULL) return NULL;

  if (cds->hal_handler_id != HAL_HANDLER_ID_SPI) return NULL;

  return (const SPI_Config*)cds->hal_config;
}


const SPI_Config* CDRV_get_spi_config_from_comm_ch(uint8_t comm_ch)
{
  CDRV_ID id;
  for (id = 0; id < CDRV_ID_MAX; id++)
  {
    const SPI_Config* config = CDRV_get_spi_config(id);
    if (config == NULL) continue;
    if (config->comm_ch == comm_ch) return config;
  }
  return NULL;
}
#endif


#ifdef CDRV_USE_HAL_HANDLER_ID_CCSDS
const CCSDS_Config* CDRV_get_ccsds_config(CDRV_ID id)
{
  const ComponentDriverSuper* cds;
  cds = CDRV_get_cds(id);
  if (cds == NULL) return NULL;

  if (cds->hal_handler_id != HAL_HANDLER_ID_CCSDS) return NULL;

  return (const CCSDS_Config*)cds->hal_config;
}


const CCSDS_Config* CDRV_get_ccsds_config_from_ch(uint8_t ch)
{
  CDRV_ID id;
  for (id = 0; id < CDRV_ID_MAX; id++)
  {
    const CCSDS_Config* config = CDRV_get_ccsds_config(id);
    if (config == NULL) continue;
    if (config->ch == ch) return config;
  }
  return NULL;
}
#endif

#pragma section
