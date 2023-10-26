#ifndef HAL_HANDLER_REGISTRY_H_
#define HAL_HANDLER_REGISTRY_H_


#include <src_core/hal/ccsds.h>
#include <src_core/hal/uart.h>

typedef enum
{
  CCSDS,
  UART,
  HAL_HANDLER_ID_MAX
} HAL_HANDLER_ID;

typedef enum
{
  IF_REOPEN_TLM_DISRUPTION = 100
} HAL_HANDLER_REOPEN_REASON;

extern  int     (*HAL_init_handlers[]  )(void* my_if);
extern  int     (*HAL_rx_handlers[]    )(void* my_if, void* data_v, int buffer_size);
extern  int     (*HAL_tx_handlers[]    )(void* my_if, void* data_v, int data_size);
extern  int     (*HAL_reopen_handlers[])(void* my_if, int reason);

#endif
