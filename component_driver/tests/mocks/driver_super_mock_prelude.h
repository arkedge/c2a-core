/**
 * @file driver_super_mock_prelude.h
 * @brief driver_super.c コンパイル用の前処理ヘッダ
 *
 * -include フラグで driver_super.c より先にインクルードすることで、
 * 実際のヘッダのインクルードガードを設定し、複雑な依存を回避する。
 *
 * 注意: このヘッダは driver_super.c のコンパイルにのみ使用する。
 *       テストコードは mocks/ 以下のモックファイルを直接インクルードする。
 */
#ifndef DRIVER_SUPER_MOCK_PRELUDE_H_
#define DRIVER_SUPER_MOCK_PRELUDE_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// obc_time.h のモック
// =============================================================================
#define OBC_TIME_H_

typedef uint32_t cycle_t;
typedef uint32_t step_t;

typedef struct
{
  cycle_t total_cycle;
  cycle_t mode_cycle;
  step_t  step;
} ObcTime;

ObcTime OBCT_create(cycle_t total_cycle, cycle_t mode_cycle, step_t step);
void OBCT_clear(ObcTime* time);
cycle_t OBCT_get_total_cycle(const ObcTime* time);
uint32_t OBCT_diff_in_msec(const ObcTime* before, const ObcTime* after);
uint32_t OBCT_get_total_cycle_in_msec(const ObcTime* time);

// =============================================================================
// time_manager.h のモック
// =============================================================================
#define TIME_MANAGER_H_

ObcTime TMGR_get_master_clock(void);

// =============================================================================
// endian.h のモック
// =============================================================================
#define ENDIAN_H_

typedef enum
{
  ENDIAN_TYPE_BIG,
  ENDIAN_TYPE_LITTLE,
  ENDIAN_TYPE_UNKNOWN
} ENDIAN_TYPE;

static inline void* ENDIAN_memcpy(void* dest, const void* src, size_t size)
{
  const uint8_t* s = (const uint8_t*)src;
  uint8_t* d = (uint8_t*)dest;
  for (size_t i = 0; i < size; i++)
  {
    d[i] = s[size - 1 - i];
  }
  return dest;
}

static inline void ENDIAN_conv(void* after, const void* before, size_t size)
{
  const uint8_t* b = (const uint8_t*)before;
  uint8_t* a = (uint8_t*)after;
  for (size_t i = 0; i < size; i++)
  {
    a[i] = b[size - 1 - i];
  }
}

static inline ENDIAN_TYPE ENDIAN_detect_system_endian(void)
{
  return ENDIAN_TYPE_LITTLE;
}

// =============================================================================
// print.h のモック
// =============================================================================
#define PRINT_H_

static inline void Printf(const char* format, ...)
{
  (void)format;
}

// =============================================================================
// common_cmd_packet 関連のモック
// =============================================================================
#define COMMON_CMD_PACKET_H_
#define COMMON_CMD_PACKET_UTIL_H_

typedef enum
{
  CCP_EXEC_SUCCESS = 0,
  CCP_EXEC_ILLEGAL_CONTEXT,
  CCP_EXEC_ILLEGAL_PARAMETER,
  CCP_EXEC_ILLEGAL_LENGTH
} CCP_EXEC_STS;

typedef struct
{
  CCP_EXEC_STS exec_sts;
  uint32_t err_code;
} CCP_CmdRet;

static inline CCP_CmdRet CCP_make_cmd_ret(CCP_EXEC_STS exec_sts, uint32_t err_code)
{
  CCP_CmdRet ret;
  ret.exec_sts = exec_sts;
  ret.err_code = err_code;
  return ret;
}

static inline CCP_CmdRet CCP_make_cmd_ret_without_err_code(CCP_EXEC_STS exec_sts)
{
  return CCP_make_cmd_ret(exec_sts, 0);
}

// =============================================================================
// hal_handler_registry.h のモック
// =============================================================================
#define HAL_HANDLER_REGISTRY_H_

typedef enum
{
  HAL_HANDLER_ID_UART,
  HAL_HANDLER_ID_MAX
} HAL_HANDLER_ID;

typedef enum
{
  IF_REOPEN_TLM_DISRUPTION = 100
} HAL_HANDLER_REOPEN_REASON;

// HAL 関数ポインタ配列（mock_hal_handler_registry.c で定義）
extern int (*HAL_init_handlers[])(void* config);
extern int (*HAL_rx_handlers[])(void* config, void* buffer, int buffer_size);
extern int (*HAL_tx_handlers[])(void* config, void* data, int data_size);
extern int (*HAL_reopen_handlers[])(void* config, int reason);

// =============================================================================
// driver_super_params.h のモック
// =============================================================================
#define DRIVER_SUPER_PARAMS_H_

// デフォルト設定を使用

#ifdef __cplusplus
}
#endif

#endif
