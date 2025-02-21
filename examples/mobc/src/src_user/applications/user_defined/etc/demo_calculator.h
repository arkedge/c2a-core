/**
 * @file
 * @brief Application の雛形や，アトミック操作のデモのための，実利用価値はないデモアプリ
 */
#ifndef DEMO_CALCULATOR_H_
#define DEMO_CALCULATOR_H_

#include <src_core/system/application_manager/app_info.h>
#include <src_core/tlm_cmd/common_packet/common_cmd_packet.h>
#include <src_core/system/time_manager/obc_time.h>

/**
 * @enum   APP_CALC_OPERATOR
 * @note   uint8_t を想定
 * @brief  演算子
 */
typedef enum
{
  APP_CALC_OPERATOR_ADD,    //!< addition
  APP_CALC_OPERATOR_SUB,    //!< subtraction
  APP_CALC_OPERATOR_MUL,    //!< multiply
  APP_CALC_OPERATOR_DIV,    //!< division
  APP_CALC_OPERATOR_MOD,    //!< modulo
  APP_CALC_OPERATOR_MAX
} APP_CALC_OPERATOR;

/**
 * @enum   APP_CALC_STATUS
 * @note   uint8_t を想定
 * @brief  演算結果
 */
typedef enum
{
  APP_CALC_STATUS_OK,               //!< OK
  APP_CALC_STATUS_ERR_DIV_BY_ZERO,  //!< 0 割エラー
  APP_CALC_STATUS_ERR_UNKNOWN       //!< 不明なエラー
} APP_CALC_STATUS;

/**
 * @struct DemoCalculator
 * @brief  DemoCalculator の AppInfo 構造体
 */
typedef struct
{
  struct
  {
    uint32_t          exec_count;       //!< 実行回数
    uint32_t          err_count;        //!< エラーカウント
    ObcTime           last_exec_time;   //!< 最終実行時刻
    APP_CALC_STATUS   status;           //!< 最終実行結果
  } info;                               //!< 実行状況
  struct
  {
    int32_t           a;                //!< 第 1 引数
    int32_t           b;                //!< 第 2 引数
    int32_t           result;           //!< 結果
    APP_CALC_OPERATOR operator;         //!< 演算子
  } calc;                               //!< 計算
} DemoCalculator;

extern const DemoCalculator* const demo_calculator;

AppInfo APP_CALC_create_app(void);

CCP_CmdRet Cmd_APP_CALC_EXEC(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_APP_CALC_SET_A(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_APP_CALC_SET_B(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_APP_CALC_SET_OP(const CommonCmdPacket* packet);
CCP_CmdRet Cmd_APP_CALC_RESET(const CommonCmdPacket* packet);

#endif
