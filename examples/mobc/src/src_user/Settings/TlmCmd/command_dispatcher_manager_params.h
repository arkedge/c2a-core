/**
 * @file
 * @brief Command Dispatcher Manager のパラメタのオーバーライド用ヘッダー
 */
#ifndef COMMAND_DISPATCHER_MANAGER_PARAMS_H_
#define COMMAND_DISPATCHER_MANAGER_PARAMS_H_

#undef CDIS_MGR_MAX_NUM_OF_CDIS

#define CDIS_MGR_MAX_NUM_OF_CDIS   (8)      // core (標準的には RT, TL * 4, GS, TDSP の 7 個) + AOBC

#endif
