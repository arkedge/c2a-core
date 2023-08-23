/**
 * @file
 * @brief Appのヘッダをまとめたヘッダ
 */
#ifndef APP_HEADERS_H_
#define APP_HEADERS_H_

// Core
#include <c2a-core/applications/nop.h>
#include <c2a-core/applications/gs_command_dispatcher.h>
#include <c2a-core/applications/realtime_command_dispatcher.h>
#include <c2a-core/applications/timeline_command_dispatcher.h>
#include <c2a-core/applications/event_utility.h>
#include <c2a-core/applications/memory_dump.h>
#include <c2a-core/applications/telemetry_manager.h>
#include <c2a-core/applications/utility_command.h>
#include <c2a-core/applications/utility_counter.h>
#include <c2a-core/applications/divided_cmd_utility.h>

// Core TestApp
#include <c2a-core/applications/test_app/test_ccp_util.h>

// DI
#include "driver_instances/di_aobc.h"
#include "driver_instances/di_uart_test.h"
#include "driver_instances/di_gs.h"

// UserDefined
#include "user_defined/debug_apps.h"

#endif
