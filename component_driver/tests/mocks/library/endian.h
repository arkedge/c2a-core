/**
 * @file endian.h
 * @brief エンディアン関連のモック（テスト用）
 */
#ifndef MOCK_ENDIAN_H_
#define MOCK_ENDIAN_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  ENDIAN_TYPE_BIG,
  ENDIAN_TYPE_LITTLE,
  ENDIAN_TYPE_UNKNOWN
} ENDIAN_TYPE;

// テスト環境向けのシンプルな実装
static inline void* ENDIAN_memcpy(void* dest, const void* src, size_t size)
{
  // リトルエンディアン環境を想定し、バイト順を反転
  const uint8_t* s = (const uint8_t*)src;
  uint8_t* d = (uint8_t*)dest;
  for (size_t i = 0; i < size; i++)
  {
    d[i] = s[size - 1 - i];
  }
  return dest;
}

static inline void ENDIAN_conv(void* after, const void* before, size_t size)
{
  const uint8_t* b = (const uint8_t*)before;
  uint8_t* a = (uint8_t*)after;
  for (size_t i = 0; i < size; i++)
  {
    a[i] = b[size - 1 - i];
  }
}

static inline ENDIAN_TYPE ENDIAN_detect_system_endian(void)
{
  // テスト環境（x86/x64）はリトルエンディアン
  return ENDIAN_TYPE_LITTLE;
}

#ifdef __cplusplus
}
#endif

#endif
