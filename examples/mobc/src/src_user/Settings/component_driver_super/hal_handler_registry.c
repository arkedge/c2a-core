#pragma section REPRO
#include "hal_handler_registry.h"

#include <stddef.h>

int (*HAL_init_handlers[])(void* my_if) = { CCSDS_init, UART_init};

int (*HAL_rx_handlers[])(void* my_if, void* data_v, int buffer_size) = { CCSDS_rx, UART_rx};

int (*HAL_tx_handlers[])(void* my_if, void* data_v, int data_size) = { CCSDS_tx, UART_tx};

int (*HAL_reopen_handlers[])(void* my_if, int reason) = { CCSDS_reopen, UART_reopen};

#pragma section
