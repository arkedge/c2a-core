/**
 * @file mock_time_manager.h
 * @brief TimeManager のモック
 *
 * 時刻演算 (OBCT_*) は実 obc_time.c をリンクして使う。ここではグローバルな
 * マスタクロックを返す TMGR_get_master_clock のスタブと、その制御用関数のみを提供する。
 */
#ifndef MOCK_TIME_MANAGER_H_
#define MOCK_TIME_MANAGER_H_

#include <system/time_manager/obc_time.h>  // 実 ObcTime / cycle_t / OBCT_*

#ifdef __cplusplus
extern "C" {
#endif

ObcTime TMGR_get_master_clock(void);

// モック制御用
void mock_time_reset(void);
void mock_time_set_current(cycle_t total_cycle);
void mock_time_advance(cycle_t cycles);

#ifdef __cplusplus
}
#endif

#endif
