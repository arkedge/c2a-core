/**
 * @file mock_print.c
 * @brief Printf のスタブ実装
 *
 * 実 print.h (宣言のみ) を使う driver_super.c / obc_time.c のリンク用。
 * テスト中はデバッグ出力を抑制する。
 */
void Printf(const char* format, ...)
{
  (void)format;
}
