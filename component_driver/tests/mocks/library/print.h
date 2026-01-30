/**
 * @file print.h
 * @brief Printf のモック（テスト用）
 */
#ifndef MOCK_PRINT_H_
#define MOCK_PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

// テスト環境では Printf を空実装にする
static inline void Printf(const char* format, ...)
{
  (void)format;
  // Do nothing - suppress debug output during tests
}

#ifdef __cplusplus
}
#endif

#endif
