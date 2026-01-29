/**
 * @file mock_ccp.h
 * @brief Common Cmd Packet 関連のモック
 */
#ifndef MOCK_CCP_H_
#define MOCK_CCP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
