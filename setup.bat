@echo off
echo setup.bat
echo setting for local environment...
echo.

echo [SET USER HOOKS]
git config --local core.hooksPath script/git_hooks
echo done.
echo.

echo [SET UP DEVELOP ENVIRONMENT FOR C2A MOBC USER SAMPLE]
call :make_link ".\examples\mobc\src\src_core" ".\"
echo done.
echo.

echo [SET UP DEVELOP ENVIRONMENT FOR SUBOBC USER SAMPLE]
call :make_link ".\examples\subobc\src\src_core" ".\"
echo done.
echo.

echo The process has been completed.
echo Press any key to exit...
pause >nul
exit


:make_link
if exist %1 (
  rmdir /s /q %1
)
mklink /j /d %1 %2

exit /b
