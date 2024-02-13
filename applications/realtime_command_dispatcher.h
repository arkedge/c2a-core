#ifndef REALTIME_COMMAND_DISPATCHER_H_
#define REALTIME_COMMAND_DISPATCHER_H_

#include "../tlm_cmd/command_dispatcher.h"
#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_packet/common_cmd_packet.h"

extern const CommandDispatcher* const realtime_command_dispatcher;

AppInfo RTCD_create_app(void);

CCP_CmdRet Cmd_RTCD_CLEAR_ALL_REALTIME(const CommonCmdPacket* packet);

CCP_CmdRet Cmd_RTCD_CLEAR_ERR_LOG(const CommonCmdPacket* packet);

#endif
