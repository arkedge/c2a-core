/**
 * @file driver_super_mock_prelude.h
 * @brief driver_super.c / テストコード コンパイル用の前処理ヘッダ
 *
 * -include フラグで driver_super.c とテスト .cpp より先にインクルードする。
 *
 * 方針:
 *   - 軽量で自己完結した実ヘッダ (obc_time.h / endian.h / print.h) はそのまま使う。
 *     ObcTime / OBCT_* / ENDIAN_TYPE は実型・実実装 (obc_time.c をリンク) を使い、
 *     テスト独自の再実装による乖離を避ける。
 *   - common_cmd_packet.h を芋づる式に引いて重い src_user 依存を持ち込む time_manager.h /
 *     common_cmd_packet(_util).h、および user 設定である hal_handler_registry.h /
 *     driver_super_params.h は、インクルードガードを先回りで定義してスキップし、
 *     driver_super.c が必要とする最小限の型・宣言だけをここで供給する。
 */
#ifndef DRIVER_SUPER_MOCK_PRELUDE_H_
#define DRIVER_SUPER_MOCK_PRELUDE_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// 実 obc_time.h を使用 (ObcTime / cycle_t / step_t / OBCT_* は本物。実装は obc_time.c をリンク)。
// time_manager.h をスキップするため、ここで明示的に取り込んでおく。
#include <system/time_manager/obc_time.h>

// time_manager.h は common_cmd_packet.h 経由で重い src_user を引くためスキップする。
// driver_super.c が使うのは TMGR_get_master_clock のみ (実装は mock_time_manager.c)。
#define TIME_MANAGER_H_
ObcTime TMGR_get_master_clock(void);

// common_cmd_packet 系も重い src_user を引くためスキップし、必要な型のみ供給する。
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

// hal_handler_registry.h は user 設定。テスト用の HAL ID と関数ポインタ配列を供給する
// (配列の実体は mock_hal_handler_registry.c で定義)。
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

extern int (*HAL_init_handlers[])(void* config);
extern int (*HAL_rx_handlers[])(void* config, void* buffer, int buffer_size);
extern int (*HAL_tx_handlers[])(void* config, void* data, int data_size);
extern int (*HAL_reopen_handlers[])(void* config, int reason);

// driver_super_params.h は user 設定。core デフォルトを使うのでスキップのみ。
#define DRIVER_SUPER_PARAMS_H_

#ifdef __cplusplus
}
#endif

#endif
