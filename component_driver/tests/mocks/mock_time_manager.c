/**
 * @file mock_time_manager.c
 * @brief TimeManager のモック実装
 */
#include "mock_time_manager.h"

static cycle_t mock_current_cycle = 0;
#define OBCT_CYCLES_PER_SEC (10)

ObcTime OBCT_create(cycle_t total_cycle, cycle_t mode_cycle, step_t step)
{
  ObcTime time;
  time.total_cycle = total_cycle;
  time.mode_cycle = mode_cycle;
  time.step = step;
  return time;
}

void OBCT_clear(ObcTime* time)
{
  time->total_cycle = 0;
  time->mode_cycle = 0;
  time->step = 0;
}

cycle_t OBCT_get_total_cycle(const ObcTime* time)
{
  return time->total_cycle;
}

uint32_t OBCT_diff_in_msec(const ObcTime* before, const ObcTime* after)
{
  if (after->total_cycle < before->total_cycle) return 0;
  cycle_t diff_cycle = after->total_cycle - before->total_cycle;
  return (diff_cycle * 1000) / OBCT_CYCLES_PER_SEC;
}

uint32_t OBCT_get_total_cycle_in_msec(const ObcTime* time)
{
  return (time->total_cycle * 1000) / OBCT_CYCLES_PER_SEC;
}

ObcTime TMGR_get_master_clock(void)
{
  return OBCT_create(mock_current_cycle, mock_current_cycle, 0);
}

void mock_time_reset(void) { mock_current_cycle = 0; }
void mock_time_set_current(cycle_t total_cycle) { mock_current_cycle = total_cycle; }
void mock_time_advance(cycle_t cycles) { mock_current_cycle += cycles; }
