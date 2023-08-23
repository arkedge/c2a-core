/**
 * @file
 * @brief  テレメトリ定義に必要なヘッダー
 */
#ifndef TELEMETRY_SOURCE_H_
#define TELEMETRY_SOURCE_H_

#include "string.h" // for memcpy
#include <c2a-core/library/git_revision.h>
#include <c2a-core/System/ApplicationManager/app_manager.h>
#include <c2a-core/System/TaskManager/task_dispatcher.h>
#include <c2a-core/System/ModeManager/mode_manager.h>
#include <c2a-core/System/EventManager/event_logger.h>
#include <c2a-core/System/EventManager/event_handler.h>
#include <c2a-core/System/WatchdogTimer/watchdog_timer.h>
#include <c2a-core/System/TimeManager/time_manager.h>
#include <c2a-core/TlmCmd/command_analyze.h>
#include <c2a-core/TlmCmd/command_dispatcher_manager.h>
#include <c2a-core/TlmCmd/block_command_executor.h>
#include "../applications/app_headers.h"
#include "../Settings/build_settings.h"

#endif
