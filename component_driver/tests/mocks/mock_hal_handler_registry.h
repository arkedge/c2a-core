/**
 * @file mock_hal_handler_registry.h
 * @brief HAL ハンドラのモック
 */
#ifndef MOCK_HAL_HANDLER_REGISTRY_H_
#define MOCK_HAL_HANDLER_REGISTRY_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  HAL_HANDLER_ID_UART,
  HAL_HANDLER_ID_MAX
} HAL_HANDLER_ID;

typedef enum
{
  IF_REOPEN_TLM_DISRUPTION = 100
} HAL_HANDLER_REOPEN_REASON;

extern int (*HAL_init_handlers[])(void* config);
extern int (*HAL_rx_handlers[])(void* config, void* buffer, int buffer_size);
extern int (*HAL_tx_handlers[])(void* config, void* data, int data_size);
extern int (*HAL_reopen_handlers[])(void* config, int reason);

// モック制御用関数
void mock_hal_reset(void);
void mock_hal_set_rx_data(const uint8_t* data, int len);
void mock_hal_append_rx_data(const uint8_t* data, int len);
void mock_hal_set_rx_chunk_size(int chunk_size);  // 1回の rx で返す最大バイト数（0=無制限）
int mock_hal_get_tx_count(void);
const uint8_t* mock_hal_get_last_tx_data(void);
int mock_hal_get_last_tx_size(void);

#ifdef __cplusplus
}
#endif

#endif
