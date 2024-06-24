/**
 * @file
 * @brief  HW 依存 Print, つまり ログ情報のフォーマット出力（デバッグ出力）
 */
// こいつもBootLoaderに配置するので，リプロ可能領域のものをincludeしないこと！！

// printはHW依存性が強いため，ヘッダーはcoreにおき，オーバーライドする
// ビルド対象にて選択する
#include <src_core/library/print.h>

#include <stdio.h>
#include <stdarg.h>

#include "../settings/sils_define.h"

// バッファサイズよりでかい文字列が来ると死ぬ
static char PRINT_buffer_[512];

#ifdef SILS_FW

// SILS 用 Printf() 実装

void Printf(const char* format, ...)
{
#ifdef SHOW_DEBUG_PRINT_ON_SILS
  va_list args;
  va_start(args, format);
  vsprintf(PRINT_buffer_, format, args);

  printf("%s", PRINT_buffer_);
  fflush(stdout);

  va_end(args);
#else
  // なにも表示しない
#endif
}

#else

#include <src_core/system/watchdog_timer/watchdog_timer.h>

// 実機用 Printf() 実装のテンプレート
void Printf(const char* format, ...)
{
  va_list argptr;

  WDT_clear_wdt();

  va_start(argptr, format);

  // ここに出力の本体を書く

  va_end(argptr);

  WDT_clear_wdt();
}

#endif
