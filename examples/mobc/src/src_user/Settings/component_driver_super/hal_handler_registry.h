#ifndef HAL_HANDLER_REGISTRY_H_
#define HAL_HANDLER_REGISTRY_H_


#include <src_core/hal/ccsds.h>
#include <src_core/hal/uart.h>

typedef enum
{
  HAL_HANDLER_ID_CCSDS,
  HAL_HANDLER_ID_UART,
  HAL_HANDLER_ID_MAX
} HAL_HANDLER_ID;

typedef enum
{
  IF_REOPEN_TLM_DISRUPTION = 100
} HAL_HANDLER_REOPEN_REASON;

extern  int     (*HAL_init_handlers[]  )(void* config);
extern  int     (*HAL_rx_handlers[]    )(void* config, void* buffer, int buffer_size);
extern  int     (*HAL_tx_handlers[]    )(void* config, void* data, int data_size);
extern  int     (*HAL_reopen_handlers[])(void* config, int reason);

#endif
