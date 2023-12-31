@ECHO OFF
REM @brief sils_define.h のコメントアウトを有効化する

REM スクリプト実行時，sils_define.h から
REM ```
REM #define SILS_FW
REM ```
REM と書かれた行を検出し，コメントアウトを有効化する
REM 以下のサイトを参考にした
REM   文字列置換: https://qiita.com/wagase/items/2180d8911dcc2a748fd3
REM   空行処理  : https://blogs.yahoo.co.jp/kerupani/15344574.html

cd /d %~dp0
setlocal enabledelayedexpansion

for /f "tokens=1* delims=:" %%A in ('findstr /n "^" ..\settings\sils_define.h') do (
  set line0=%%B

  REM 空の行はechoで表示できないので分岐しておく
  if "%%B" == "" (
    echo.>>..\settings\sils_define.tmp
  ) else (
    set line0=!line0:#define SILS_FW=// #define SILS_FW!
    set line0=!line0:////=//!
    echo !line0!>>..\settings\sils_define.tmp
  )
)

REM 一時ファイルを削除し、変更を反映
move ..\settings\sils_define.tmp ..\settings\sils_define.h
