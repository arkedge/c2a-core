/**
 * @file mock_hal_handler_registry.c
 * @brief HAL ハンドラのモック実装
 */
#include "mock_hal_handler_registry.h"
#include <string.h>

static uint8_t mock_rx_buffer[4096];
static int mock_rx_len = 0;
static int mock_rx_pos = 0;

static uint8_t mock_tx_buffer[4096];
static int mock_tx_count = 0;
static int mock_last_tx_size = 0;

static int mock_hal_init(void* config)
{
  (void)config;
  return 0;
}

static int mock_hal_rx(void* config, void* buffer, int buffer_size)
{
  (void)config;
  if (mock_rx_pos >= mock_rx_len) return 0;

  int remaining = mock_rx_len - mock_rx_pos;
  int copy_len = (remaining < buffer_size) ? remaining : buffer_size;
  memcpy(buffer, mock_rx_buffer + mock_rx_pos, copy_len);
  mock_rx_pos += copy_len;
  return copy_len;
}

static int mock_hal_tx(void* config, void* data, int data_size)
{
  (void)config;
  if (data_size > (int)sizeof(mock_tx_buffer)) return -1;

  memcpy(mock_tx_buffer, data, data_size);
  mock_last_tx_size = data_size;
  mock_tx_count++;
  return 0;
}

static int mock_hal_reopen(void* config, int reason)
{
  (void)config;
  (void)reason;
  return 0;
}

int (*HAL_init_handlers[])(void* config) = { mock_hal_init };
int (*HAL_rx_handlers[])(void* config, void* buffer, int buffer_size) = { mock_hal_rx };
int (*HAL_tx_handlers[])(void* config, void* data, int data_size) = { mock_hal_tx };
int (*HAL_reopen_handlers[])(void* config, int reason) = { mock_hal_reopen };

void mock_hal_reset(void)
{
  mock_rx_len = 0;
  mock_rx_pos = 0;
  mock_tx_count = 0;
  mock_last_tx_size = 0;
  memset(mock_rx_buffer, 0, sizeof(mock_rx_buffer));
  memset(mock_tx_buffer, 0, sizeof(mock_tx_buffer));
}

void mock_hal_set_rx_data(const uint8_t* data, int len)
{
  if (len > (int)sizeof(mock_rx_buffer)) len = sizeof(mock_rx_buffer);
  memcpy(mock_rx_buffer, data, len);
  mock_rx_len = len;
  mock_rx_pos = 0;
}

int mock_hal_get_tx_count(void) { return mock_tx_count; }
const uint8_t* mock_hal_get_last_tx_data(void) { return mock_tx_buffer; }
int mock_hal_get_last_tx_size(void) { return mock_last_tx_size; }
