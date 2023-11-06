#!/bin/bash
echo "System -> system rename migration"

function find_all() {
  find . -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

function find_settings() {
  find src/src_user/Settings -name "*" -not \( -path "*/.git/*" -o -path "*/src_core/*" -o -path "*.xlsm" \) -type f -print0
}

## c2a-core ref
echo "rename c2a-core ref: src_core/System -> src_core/system"
find_all | xargs -0 sed -i -e "s#src_core/System/ApplicationManager#src_core/system/application_manager#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System\\\ApplicationManager#src_core\\\system\\\application_manager#g"

find_all | xargs -0 sed -i -e "s#src_core/System/EventManager#src_core/system/event_manager#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System\\\EventManager#src_core\\\system\\\event_manager#g"

find_all | xargs -0 sed -i -e "s#src_core/System/ModeManager#src_core/system/mode_manager#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System\\\ModeManager#src_core\\\system\\\mode_manager#g"

find_all | xargs -0 sed -i -e "s#src_core/System/TaskManager#src_core/system/task_manager#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System\\\TaskManager#src_core\\\system\\\task_manager#g"

find_all | xargs -0 sed -i -e "s#src_core/System/TimeManager#src_core/system/time_manager#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System\\\TimeManager#src_core\\\system\\\time_manager#g"

find_all | xargs -0 sed -i -e "s#src_core/System/WatchdogTimer#src_core/system/watchdog_timer#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System\\\WatchdogTimer#src_core\\\system\\\watchdog_timer#g"

find_all | xargs -0 sed -i -e "s#src_core/System#src_core/system#g"
find_all | xargs -0 sed -i -e "s#src_core\\\System#src_core\\\system#g"

find_all | xargs -0 sed -i -e "s#\${C2A_CORE_DIR}/System#\${C2A_CORE_DIR}/system#g"

# Settings
echo "rename settings dir: src_user/Settings/System -> src_user/Settings/system"
mv src/src_user/Settings/System src/src_user/Settings/system
mv src/src_user/Settings/system/EventHandlerRules src/src_user/Settings/system/event_handler_rules
find_settings | xargs -0 sed -i -e "s#System/EventHandlerRules#system/event_handler_rules#g"
find_settings | xargs -0 sed -i -e "s#System#system#g"

find_all | xargs -0 sed -i -e "s#src_user/Settings/System/EventHandlerRules#src_user/Setting/system/event_handler_rules#g"
find_all | xargs -0 sed -i -e "s#src_user\\\Settings\\\System\\\EventHandlerRules#src_user\\\Settings\\\system\\\event_handler_rules#g"

find_all | xargs -0 sed -i -e "s#src_user/Settings/System#src_user/Settings/system#g"
find_all | xargs -0 sed -i -e "s#src_user\\\Settings\\\System#src_user\\\Settings\\\system#g"

# Test
echo "rename test dir: test/src_core/System -> test/src_core/system"
mkdir -p src/src_user/Test/test/src_core/system
mv src/src_user/Test/test/src_core/System/EventManager src/src_user/Test/test/src_core/system/event_manager
mv src/src_user/Test/test/src_core/System/ModeManager src/src_user/Test/test/src_core/system/mode_manager
mv src/src_user/Test/test/src_core/System/TaskManager src/src_user/Test/test/src_core/system/task_manager
mv src/src_user/Test/test/src_core/System/TimeManager src/src_user/Test/test/src_core/system/time_manager
mv src/src_user/Test/test/src_core/System/WatchdogTimer src/src_user/Test/test/src_core/system/watchdog_timer

# Script
echo "rename ref in script: System -> system"
find src/src_user/script -name "*" -not -path "*/.git/*" -type f -print0 | xargs -0 sed -i -e "s#System#system#g"
