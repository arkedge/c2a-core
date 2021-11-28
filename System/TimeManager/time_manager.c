#pragma section REPRO
#include "time_manager.h"

#include <src_user/CmdTlm/Ccsds/TCPacket.h>
#include "../TaskManager/task_dispatcher.h"
#include "../../Library/endian_memcpy.h"

static ObcTime master_clock_;
const ObcTime* master_clock;

static OBCT_UnixTimeInfo OBCT_unix_time_info_;
const OBCT_UnixTimeInfo* const OBCT_unix_time_info = &OBCT_unix_time_info_;

static void TMGR_set_master_total_cycle_(cycle_t total_cycle);

void TMGR_init(void)
{
  OBCT_clear(&master_clock_);
  master_clock = &master_clock_;

  OBCT_clear_unix_time_info(&OBCT_unix_time_info_);
}

void TMGR_clear_master_mode_cycle(void)
{
  master_clock_.mode_cycle = 0;
}

#pragma section _FIX_TMGR
// P�Z�N�V�����������蓖�ĂĂ��Ȃ��̂ŁC
// D, B�Z�N�V�����ɏ��悤�ȕϐ���`�͋֎~�I�I�I
void TMGR_count_up_master_clock(void)
{
  OBCT_count_up(&master_clock_);
}
#pragma section
#pragma section REPRO

uint32_t TMGR_get_master_total_cycle_in_msec(void)
{
  return OBCT_get_total_cycle_in_msec(&master_clock_);
}

uint32_t TMGR_get_master_mode_cycle_in_msec(void)
{
  return OBCT_get_mode_cycle_in_msec(&master_clock_);
}

ObcTime TMGR_get_master_clock(void) {
  return *master_clock;     // �킴�� const ��Ԃ��Ă���D���ǈӖ��Ȃ����H �I�u�W�F�N�g�̓R�s�[�����킯�����D
}

cycle_t TMGR_get_master_total_cycle(void) {
  return OBCT_get_total_cycle(&master_clock_);
}

cycle_t TMGR_get_master_mode_cycle(void) {
  return OBCT_get_mode_cycle(&master_clock_);
}

step_t  TMGR_get_master_step(void) {
  return OBCT_get_step(&master_clock_);
}


CCP_EXEC_STS Cmd_TMGR_SET_TIME(const CTCP* packet)
{
  cycle_t set_value = 0;

  endian_memcpy(&set_value, CCP_get_param_head(packet), 4);

  if (set_value < OBCT_MAX_CYCLE)
  {
    TMGR_set_master_total_cycle_(set_value);
    TDSP_resync_internal_counter();
    return CCP_EXEC_SUCCESS;
  }
  else
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }
}

static void TMGR_set_master_total_cycle_(cycle_t total_cycle)
{
  master_clock_.total_cycle = total_cycle;
}

double TMGR_get_unix_time_from_ObcTime(const ObcTime* time)
{
  ObcTime ti0 = OBCT_create(0, 0, 0);
  return OBCT_unix_time_info->unix_time_on_ti0 + OBCT_diff_in_sec(&ti0, time);
}

ObcTime TMGR_get_ObcTime_from_unix_time(const double unix_time)
{
  double diff_double = unix_time - OBCT_unix_time_info->unix_time_on_ti0;
  ObcTime res;
  uint32_t diff;
  cycle_t cycle_diff;
  step_t step_diff;

  if (diff_double < 0)  // ���蓾�Ȃ�, ��������
  {
    return res = OBCT_create(0, 0, 0);
  }

  diff = (uint32_t)(diff_double * 1000.0 + 1e-4); // ms�I�[�_�[��������Ȃɑ傫���Ȃ����Ƃ�z��, 1e-4�͐��l�덷�΍�i.999��j�؂�̂Ă���̂�h���j
  cycle_diff = diff / (OBCT_STEP_IN_MSEC * OBCT_STEPS_PER_CYCLE);
  step_diff = (diff - cycle_diff * (OBCT_STEP_IN_MSEC * OBCT_STEPS_PER_CYCLE)) / OBCT_STEP_IN_MSEC;

  res.total_cycle = cycle_diff;
  res.mode_cycle = 0; // �擾�o���Ȃ��̂�0�Ƃ���
  res.step = step_diff;

  return res;
}

void TMGR_modify_unix_time_criteria(const double unix_time, const ObcTime time)
{
  OBCT_modify_unix_time_info(&OBCT_unix_time_info_, unix_time, time);
}

OBCT_UnixTimeInfo TMGR_get_obct_unix_time_info(void)
{
  return *OBCT_unix_time_info;
}

CCP_EXEC_STS Cmd_TMGR_SET_UNIXTIME(const CTCP* packet)
{
  const unsigned char* param = CCP_get_param_head(packet);
  double unix_time;
  ObcTime time;

  endian_memcpy(&unix_time, param, 8);
  endian_memcpy(&time.total_cycle, param + 8, 4);
  endian_memcpy(&time.mode_cycle, param + 12, 4);
  endian_memcpy(&time.step, param + 16, 4);

  TMGR_modify_unix_time_criteria(unix_time, time);

  return CCP_EXEC_SUCCESS;
}

#pragma section