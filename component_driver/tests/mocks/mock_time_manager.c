/**
 * @file mock_time_manager.c
 * @brief TimeManager のモック実装
 *
 * 時刻演算 (OBCT_*) は実 obc_time.c をリンクするため、ここでは再実装しない。
 * グローバルなマスタクロックだけをテストから制御できるようにする。
 */
#include "mock_time_manager.h"

static cycle_t mock_current_cycle = 0;

ObcTime TMGR_get_master_clock(void)
{
  return OBCT_create(mock_current_cycle, mock_current_cycle, 0);
}

void mock_time_reset(void) { mock_current_cycle = 0; }
void mock_time_set_current(cycle_t total_cycle) { mock_current_cycle = total_cycle; }
void mock_time_advance(cycle_t cycles) { mock_current_cycle += cycles; }
