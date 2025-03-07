#pragma section REPRO
/**
 * @file
 * @brief Application の雛形や，アトミック操作のデモのための，実利用価値はないデモアプリ
 */
#include "demo_calculator.h"
#include <stddef.h>
#include <string.h>
#include <src_core/system/time_manager/time_manager.h>
#include <src_core/tlm_cmd/common_packet/common_cmd_packet_util.h>

/**
 * @brief  App 初期化関数
 * @param  void
 * @return RESULT
 */
static RESULT APP_CALC_init_(void);
static RESULT APP_CALC_exec_(void);

static DemoCalculator demo_calculator_;
const DemoCalculator* const demo_calculator = &demo_calculator_;


AppInfo APP_CALC_create_app(void)
{
  return AI_create_app_info("demo_calc", APP_CALC_init_, APP_CALC_exec_);
}


static RESULT APP_CALC_init_(void)
{
  memset(&demo_calculator_, 0x00, sizeof(DemoCalculator));
  return RESULT_OK;
}


static RESULT APP_CALC_exec_(void)
{
  demo_calculator_.info.exec_count++;
  demo_calculator_.info.last_exec_time = TMGR_get_master_clock();
  demo_calculator_.info.status = APP_CALC_STATUS_OK;

  switch (demo_calculator_.calc.operator)
  {
  case APP_CALC_OPERATOR_ADD:
    demo_calculator_.calc.result = demo_calculator_.calc.a + demo_calculator_.calc.b;
    break;
  case APP_CALC_OPERATOR_SUB:
    demo_calculator_.calc.result = demo_calculator_.calc.a - demo_calculator_.calc.b;
    break;
  case APP_CALC_OPERATOR_MUL:
    demo_calculator_.calc.result = demo_calculator_.calc.a * demo_calculator_.calc.b;
    break;
  case APP_CALC_OPERATOR_DIV:
    if (demo_calculator_.calc.b == 0)
    {
      demo_calculator_.info.status = APP_CALC_STATUS_ERR_DIV_BY_ZERO;
      demo_calculator_.info.err_count++;
      return RESULT_ERR;
    }
    demo_calculator_.calc.result = demo_calculator_.calc.a / demo_calculator_.calc.b;
    break;
  case APP_CALC_OPERATOR_MOD:
    if (demo_calculator_.calc.b == 0)
    {
      demo_calculator_.info.status = APP_CALC_STATUS_ERR_DIV_BY_ZERO;
      demo_calculator_.info.err_count++;
      return RESULT_ERR;
    }
    demo_calculator_.calc.result = demo_calculator_.calc.a % demo_calculator_.calc.b;
    break;
  default:
    demo_calculator_.info.status = APP_CALC_STATUS_ERR_UNKNOWN;
    demo_calculator_.info.err_count++;
    return RESULT_ERR;
  }

  return RESULT_OK;
}


CCP_CmdRet Cmd_APP_CALC_EXEC(const CommonCmdPacket* packet)
{
  (void)packet;

  if (APP_CALC_exec_() == RESULT_ERR)
  {
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT, demo_calculator_.info.status);
  }

  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_APP_CALC_SET_A(const CommonCmdPacket* packet)
{
  demo_calculator_.calc.a = CCP_get_param_from_packet(packet, 0, int32_t);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_APP_CALC_SET_B(const CommonCmdPacket* packet)
{
  demo_calculator_.calc.b = CCP_get_param_from_packet(packet, 0, int32_t);
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_APP_CALC_SET_OP(const CommonCmdPacket* packet){
  APP_CALC_OPERATOR op = (APP_CALC_OPERATOR)CCP_get_param_from_packet(packet, 0, uint8_t);

  if (op >= APP_CALC_OPERATOR_MAX)
  {
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, op);
  }

  demo_calculator_.calc.operator = op;
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}


CCP_CmdRet Cmd_APP_CALC_RESET(const CommonCmdPacket* packet)
{
  (void)packet;
  APP_CALC_init_();
  return CCP_make_cmd_ret_without_err_code(CCP_EXEC_SUCCESS);
}

#pragma section
