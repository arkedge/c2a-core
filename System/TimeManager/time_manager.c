#pragma section REPRO
/**
 * @file
 * @brief OBC時刻のカウントアップと，各種衛星時刻関連処理
 */
#include "time_manager.h"
#include <string.h>
#include "../../Library/c2a_round.h"
#include "../TaskManager/task_dispatcher.h"
#include "../../TlmCmd/common_cmd_packet_util.h"

static TimeManager time_manager_;
const TimeManager* const time_manager = &time_manager_;

/**
 * @brief TI (master_clock_.total_cycle) の setter
 * @param[in] total_cycle
 * @return TMGR_ACK
 */
static TMGR_ACK TMGR_set_master_total_cycle_(cycle_t total_cycle);

/**
 * @brief time_manager_.unixtime_info_.utl_unixtime_epoch の setter
 * @param[in] utl_unixtime_epoch
 * @return TMGR_ACK
 */
static TMGR_ACK TMGR_set_utl_unixtime_epoch_(double utl_unixtime_epoch);

/**
 * @brief time_manager_.unixtime_info_.cycle_correction の setter
 * @param[in] cycle_correction
 * @return TMGR_ACK
 */
static TMGR_ACK TMGR_set_cycle_correction_(double cycle_correction);

/**
 * @brief enum 変換用関数
 * @param[in] ack: TMGR_ACK
 * @return CCP_CmdRet
 */
static CCP_CmdRet TMGR_conv_tmgr_ack_to_ccp_cmd_ret_(TMGR_ACK ack);

void TMGR_init(void)
{
  OBCT_clear(&time_manager_.init_info_.initializing_time);
  time_manager_.init_info_.initializing_flag = 1;
  TMGR_clear();
}

void TMGR_clear(void)
{
  OBCT_clear(&time_manager_.master_clock_);
  TMGR_clear_unixtime_info();
}

void TMGR_clear_master_mode_cycle(void)
{
  time_manager_.master_clock_.mode_cycle = 0;
}

#pragma section _FIX_TMGR
// Pセクションしか割り当てていないので，
// D, Bセクションに乗るような変数定義は禁止！！！
void TMGR_count_up_master_clock(void)
{
  OBCT_count_up(&time_manager_.master_clock_);
}
#pragma section
#pragma section REPRO

void TMGR_down_initializing_flag(void)
{
  memcpy(&time_manager_.init_info_.initializing_time, &time_manager_.master_clock_, sizeof(ObcTime));
  time_manager_.init_info_.initializing_flag = 0;

  TMGR_clear();
}

ObcTime TMGR_get_master_clock(void)
{
  if (time_manager_.init_info_.initializing_flag)
  {
    return OBCT_create(0, 0, 0);
  }
  else
  {
    return time_manager_.master_clock_;
  }
}

ObcTime TMGR_get_master_clock_from_boot(void)
{
  return OBCT_add(&time_manager_.init_info_.initializing_time, &time_manager_.master_clock_);
}

cycle_t TMGR_get_master_total_cycle(void) {
  return OBCT_get_total_cycle(&time_manager_.master_clock_);
}

cycle_t TMGR_get_master_mode_cycle(void) {
  return OBCT_get_mode_cycle(&time_manager_.master_clock_);
}

step_t  TMGR_get_master_step(void) {
  return OBCT_get_step(&time_manager_.master_clock_);
}

uint32_t TMGR_get_master_total_cycle_in_msec(void)
{
  return OBCT_get_total_cycle_in_msec(&time_manager_.master_clock_);
}

uint32_t TMGR_get_master_mode_cycle_in_msec(void)
{
  return OBCT_get_mode_cycle_in_msec(&time_manager_.master_clock_);
}

static TMGR_ACK TMGR_set_master_total_cycle_(cycle_t total_cycle)
{
  if (total_cycle >= OBCT_MAX_CYCLE) return TMGR_ACK_PARAM_ERR;

  time_manager_.master_clock_.total_cycle = total_cycle;
  TDSP_resync_internal_counter();
  return TMGR_ACK_OK;
}

void TMGR_clear_unixtime_info(void)
{
  time_manager_.unixtime_info_.unixtime_at_ti0 = 0.0;
  time_manager_.unixtime_info_.ti_at_last_update = 0;
  TMGR_set_utl_unixtime_epoch_(TMGR_DEFAULT_UNIXTIME_EPOCH_FOR_UTL);
  TMGR_set_cycle_correction_(1.0);
}

TMGR_ACK TMGR_update_unixtime(const double unixtime, const ObcTime* time)
{
  double ti_sec = TMGR_get_precise_ti_in_sec(time);

  // unixtime が ti より小さいと困る
  if (unixtime < ti_sec) return TMGR_ACK_PARAM_ERR;

  time_manager_.unixtime_info_.unixtime_at_ti0 = unixtime - ti_sec;
  time_manager_.unixtime_info_.ti_at_last_update = OBCT_get_total_cycle(time);
  return TMGR_ACK_OK;
}

double TMGR_get_unixtime_at_ti0(void)
{
  return time_manager_.unixtime_info_.unixtime_at_ti0;
}

double TMGR_get_utl_unixtime_epoch(void)
{
  return time_manager_.unixtime_info_.utl_unixtime_epoch;
}

double TMGR_get_precise_cycles_per_sec(void)
{
  return OBCT_CYCLES_PER_SEC * time_manager_.unixtime_info_.cycle_correction;
}

double TMGR_get_precise_ti_in_sec(const ObcTime* time)
{
  double cycle = time->total_cycle + (double)time->step / OBCT_STEPS_PER_CYCLE;
  return cycle / TMGR_get_precise_cycles_per_sec();
}

double TMGR_get_current_unixtime(void)
{
  return TMGR_get_unixtime_at_ti0() + TMGR_get_precise_ti_in_sec(&time_manager_.master_clock_);
}

double TMGR_get_unixtime_from_obc_time(const ObcTime* time)
{
  return TMGR_get_unixtime_at_ti0() + TMGR_get_precise_ti_in_sec(time);
}

double TMGR_get_unixtime_from_utl_unixtime(const cycle_t utl_unixtime)
{
  return TMGR_get_utl_unixtime_epoch() + (double)utl_unixtime / OBCT_CYCLES_PER_SEC;
}

double TMGR_get_precise_ti_from_unixtime(const double unixtime)
{
  double ti = (unixtime - TMGR_get_unixtime_at_ti0()) * TMGR_get_precise_cycles_per_sec();

  // unixtime_at_ti0 より小さい引数は無効なのでゼロを返す
  if (ti < 0) return 0;

  return ti;
}

ObcTime TMGR_get_obc_time_from_unixtime(const double unixtime)
{
  double diff_in_cycle = TMGR_get_precise_ti_from_unixtime(unixtime);
  cycle_t cycle;
  step_t step;
  ObcTime res;

  cycle = (cycle_t)diff_in_cycle; // cycle未満は切り捨て
  step = (step_t)((diff_in_cycle - cycle) * OBCT_STEPS_PER_CYCLE); // step未満は切り捨て

  res.total_cycle = cycle;
  res.mode_cycle = 0; // 取得出来ないので0とする
  res.step = step;

  return res;
}

cycle_t TMGR_get_ti_from_utl_unixtime(const cycle_t utl_unixtime)
{
  double unixtime = TMGR_get_unixtime_from_utl_unixtime(utl_unixtime);
  double ti = TMGR_get_precise_ti_from_unixtime(unixtime);

  return (cycle_t)c2a_round(ti);
}

static TMGR_ACK TMGR_set_utl_unixtime_epoch_(double utl_unixtime_epoch)
{
  if (utl_unixtime_epoch < 0) return TMGR_ACK_PARAM_ERR;

  time_manager_.unixtime_info_.utl_unixtime_epoch = utl_unixtime_epoch;
  return TMGR_ACK_OK;
}

static TMGR_ACK TMGR_set_cycle_correction_(double cycle_correction)
{
  // 比なので、負数はおかしい
  if (cycle_correction <= 0) return TMGR_ACK_PARAM_ERR;

  time_manager_.unixtime_info_.cycle_correction = cycle_correction;
  return TMGR_ACK_OK;
}

static CCP_CmdRet TMGR_conv_tmgr_ack_to_ccp_cmd_ret_(TMGR_ACK ack)
{
  switch (ack)
  {
  case TMGR_ACK_OK:
    return CCP_make_cmd_ret(CCP_EXEC_SUCCESS, (uint32_t)ack);
  case TMGR_ACK_PARAM_ERR:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, (uint32_t)ack);
  default:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT, (uint32_t)ack);
  }
}

CCP_CmdRet Cmd_TMGR_SET_TIME(const CommonCmdPacket* packet)
{
  cycle_t set_value = CCP_get_param_from_packet(packet, 0, cycle_t);
  TMGR_ACK ack = TMGR_set_master_total_cycle_(set_value);

  return TMGR_conv_tmgr_ack_to_ccp_cmd_ret_(ack);
}

CCP_CmdRet Cmd_TMGR_UPDATE_UNIXTIME(const CommonCmdPacket* packet)
{
  ObcTime time;
  double unixtime = CCP_get_param_from_packet(packet, 0, double);
  cycle_t total_cycle = CCP_get_param_from_packet(packet, 1, cycle_t);
  step_t step = CCP_get_param_from_packet(packet, 2, cycle_t);
  TMGR_ACK ack;

  if (unixtime < 0) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  if (total_cycle >= OBCT_MAX_CYCLE) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);
  if (step >= OBCT_STEPS_PER_CYCLE) return CCP_make_cmd_ret_without_err_code(CCP_EXEC_ILLEGAL_PARAMETER);

  time.total_cycle = total_cycle;
  time.step = step;
  time.mode_cycle = 0; // 必要ないので0とする

  ack = TMGR_update_unixtime(unixtime, &time);

  return TMGR_conv_tmgr_ack_to_ccp_cmd_ret_(ack);
}

CCP_CmdRet Cmd_TMGR_SET_UTL_UNIXTIME_EPOCH(const CommonCmdPacket* packet)
{
  double utl_unixtime_epoch = CCP_get_param_from_packet(packet, 0, double);
  TMGR_ACK ack = TMGR_set_utl_unixtime_epoch_(utl_unixtime_epoch);

  return TMGR_conv_tmgr_ack_to_ccp_cmd_ret_(ack);
}

CCP_CmdRet Cmd_TMGR_SET_CYCLE_CORRECTION(const CommonCmdPacket* packet)
{
  double cycle_correction = CCP_get_param_from_packet(packet, 0, double);
  TMGR_ACK ack = TMGR_set_cycle_correction_(cycle_correction);

  return TMGR_conv_tmgr_ack_to_ccp_cmd_ret_(ack);
}

CCP_CmdRet Cmd_TMGR_RESET_CYCLE_CORRECTION(const CommonCmdPacket* packet)
{
  (void)packet;
  TMGR_set_cycle_correction_(1.0);

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

CCP_CmdRet Cmd_TMGR_CLEAR_UNIXTIME_INFO(const CommonCmdPacket* packet)
{
  (void)packet;
  TMGR_clear_unixtime_info();

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
