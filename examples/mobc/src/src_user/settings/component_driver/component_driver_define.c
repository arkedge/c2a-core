#pragma section REPRO
/**
 * @file
 * @brief Component Driver の user 定義
 */
#include "component_driver_define.h"
#include <src_core/component_driver/component_driver.h>
#include <stddef.h>
#include "../../applications/component_service/csrv_aobc.h"
#include "../../applications/component_service/csrv_gs.h"
#include "../../applications/component_service/csrv_uart_test.h"


const ComponentDriverSuper* CDRV_get_cds(CDRV_ID id)
{
  switch (id)
  {
  case CDRV_ID_AOBC:
    return &aobc_driver->driver.super;

  case CDRV_ID_GS_CCSDS:
    return &gs_driver->driver_ccsds.super;

  case CDRV_ID_GS_UART:
    return &gs_driver->driver_uart.super;

  case CDRV_ID_UART_TEST:
    return &uart_test_driver->driver.super;

  default:
    return NULL;
  }
}

#pragma section
