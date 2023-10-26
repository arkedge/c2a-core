#pragma section REPRO
#include "hal_handler_registry.h"

#include <stddef.h>

int (*HAL_init_handlers[])(void* config) = { CCSDS_init, UART_init };

int (*HAL_rx_handlers[])(void* config, void* buffer, int buffer_size) = { CCSDS_rx, UART_rx };

int (*HAL_tx_handlers[])(void* config, void* data, int data_size) = { CCSDS_tx, UART_tx };

int (*HAL_reopen_handlers[])(void* config, int reason) = { CCSDS_reopen, UART_reopen };

#pragma section
