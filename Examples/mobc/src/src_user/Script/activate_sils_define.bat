@ECHO OFF
REM @brief sils_define.h �̃R�����g�A�E�g����������

REM �X�N���v�g���s���Csils_define.h ����
REM ```
REM // #define SILS_FW
REM ```
REM �Ə����ꂽ�s�����o���C�R�����g�A�E�g����������
REM �ȉ��̃T�C�g���Q�l�ɂ���
REM   ������u��: https://qiita.com/wagase/items/2180d8911dcc2a748fd3
REM   ��s����  : https://blogs.yahoo.co.jp/kerupani/15344574.html

cd /d %~dp0
setlocal enabledelayedexpansion

for /f "tokens=1* delims=:" %%A in ('findstr /n "^" ..\Settings\sils_define.h') do (
  set line0=%%B

  REM ��̍s��echo�ŕ\���ł��Ȃ��̂ŕ��򂵂Ă���
  if "%%B" == "" (
    echo.>>..\Settings\sils_define.tmp
  ) else (
    set line1=!line0:// #define SILS_FW=#define SILS_FW!
    echo !line1!>>..\Settings\sils_define.tmp
  )
)

REM �ꎞ�t�@�C�����폜���C�ύX�𔽉f
move ..\Settings\sils_define.tmp ..\Settings\sils_define.h
