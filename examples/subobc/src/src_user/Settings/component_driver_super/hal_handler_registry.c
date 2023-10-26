#pragma section REPRO
#include "hal_handler_registry.h"

#include <stddef.h>

int (*HAL_init_handlers[])(void* my_if) = { UART_init};

int (*HAL_rx_handlers[])(void* my_if, void* data_v, int buffer_size) = { UART_rx};

int (*HAL_tx_handlers[])(void* my_if, void* data_v, int data_size) = { UART_tx};

int (*HAL_reopen_handlers[])(void* my_if, int reason) = { UART_reopen};

#pragma section
