/**
 * @file
 * @brief UART と ComponentDriverSuper テスト用
 */
#ifndef CSRV_UART_TEST_H_
#define CSRV_UART_TEST_H_

#include "../../component_driver/etc/uart_test.h"
#include <src_core/system/application_manager/app_info.h>
#include <src_core/tlm_cmd/common_cmd_packet.h>

extern const UART_TEST_Driver* const uart_test_driver;

// アプリケーション
AppInfo UART_TEST_update(void);

CCP_CmdRet Cmd_UART_TEST_INIT_CSRV(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_UART_TEST_UPDATE(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_UART_TEST_SEND_TEST(const CommonCmdPacket* packet);


#endif
