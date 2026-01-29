/**
 * @file mock_time_manager.h
 * @brief TimeManager のモック
 */
#ifndef MOCK_TIME_MANAGER_H_
#define MOCK_TIME_MANAGER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
ObcTime TMGR_get_master_clock(void);

void mock_time_reset(void);
void mock_time_set_current(cycle_t total_cycle);
void mock_time_advance(cycle_t cycles);

#ifdef __cplusplus
}
#endif

#endif
