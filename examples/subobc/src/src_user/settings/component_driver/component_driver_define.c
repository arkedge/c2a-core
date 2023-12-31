#pragma section REPRO
/**
 * @file
 * @brief Component Driver の user 定義
 */
#include "component_driver_define.h"
#include <src_core/component_driver/component_driver.h>
#include <stddef.h>
#include "../../applications/component_service/csrv_mobc.h"


const ComponentDriverSuper* CDRV_get_cds(CDRV_ID id)
{
  switch (id)
  {
  case CDRV_ID_MOBC:
    return &mobc_driver->driver.super;

  default:
    return NULL;
  }
}

#pragma section
