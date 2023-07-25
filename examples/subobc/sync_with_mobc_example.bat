@echo off
rem FIXME:
rem 現状， subobc sample と mobc sample で全く同じファイルが大量にあり，管理上めんどくさいため作った
rem そのうちなんとかする
echo sync_with_mobc_example.bat
echo sync with mobc sample...
echo.

call :sync_dir ".\src\s2e_mockup\" "..\mobc\src\s2e_mockup\"
call :sync_dir ".\src\src_user\BootLoader\" "..\mobc\src\src_user\BootLoader\"
call :sync_dir ".\src\src_user\Library\" "..\mobc\src\src_user\Library\"
call :sync_dir ".\src\src_user\ReProgramming\" "..\mobc\src\src_user\ReProgramming\"
call :sync_dir ".\src\src_user\Script\" "..\mobc\src\src_user\Script\"
call :sync_dir ".\src\src_user\Settings\TlmCmd\Ccsds\" "..\mobc\src\src_user\Settings\TlmCmd\Ccsds\"

call :sync_file ".\src\src_user\Applications\UserDefined\debug_apps.h" "..\mobc\src\src_user\Applications\UserDefined\debug_apps.h"
call :sync_file ".\src\src_user\hal\sils\uart_sils_sci_if.hpp" "..\mobc\src\src_user\hal\sils\uart_sils_sci_if.hpp"
call :sync_file ".\src\src_user\hal\sils\wdt_sils.cpp" "..\mobc\src\src_user\hal\sils\wdt_sils.cpp"
call :sync_file ".\src\src_user\hal\sils_mockup\README.md" "..\mobc\src\src_user\hal\sils_mockup\README.md"
call :sync_file ".\src\src_user\hal\sils_mockup\uart_sils.c" "..\mobc\src\src_user\hal\sils_mockup\uart_sils.c"
call :sync_file ".\src\src_user\hal\sils_mockup\wdt_sils.c" "..\mobc\src\src_user\hal\sils_mockup\wdt_sils.c"
call :sync_file ".\src\src_user\Settings\git_revision_config.h" "..\mobc\src\src_user\Settings\git_revision_config.h"
call :sync_file ".\src\src_user\Settings\DriverSuper\driver_buffer_define.h" "..\mobc\src\src_user\Settings\DriverSuper\driver_buffer_define.h"
call :sync_file ".\src\src_user\Settings\TlmCmd\common_cmd_packet_define.c" "..\mobc\src\src_user\Settings\TlmCmd\common_cmd_packet_define.c"
call :sync_file ".\src\src_user\Settings\TlmCmd\common_tlm_cmd_packet_define.h" "..\mobc\src\src_user\Settings\TlmCmd\common_tlm_cmd_packet_define.h"
call :sync_file ".\src\src_user\TlmCmd\block_command_user_settings.c" "..\mobc\src\src_user\TlmCmd\block_command_user_settings.c"
call :sync_file ".\src\src_user\TlmCmd\block_command_user_settings.h" "..\mobc\src\src_user\TlmCmd\block_command_user_settings.h"
call :sync_file ".\src\src_user\TlmCmd\common_cmd_packet.c" "..\mobc\src\src_user\TlmCmd\common_cmd_packet.c"
call :sync_file ".\src\src_user\TlmCmd\common_tlm_cmd_packet.c" "..\mobc\src\src_user\TlmCmd\common_tlm_cmd_packet.c"

call :sync_file ".\src\src_user\Test\utils\wings_utils.py" "..\mobc\src\src_user\Test\utils\wings_utils.py"
call :sync_file ".\src\src_user\Test\pytest.ini" "..\mobc\src\src_user\Test\pytest.ini"


echo.
echo The process has been completed.
echo Press any key to exit...
pause >nul
exit


:sync_dir
if exist %1 (
  rmdir /s /q %1
)
xcopy /e %2 %1
exit /b

:sync_file
xcopy /y %2 %1
exit /b
