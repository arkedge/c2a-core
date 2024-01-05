#pragma section REPRO
/**
 * @file
 * @brief エンディアンに関するものを定義する
 */
#include "endian.h"
#include <src_user/settings/build_settings.h>
#include <stdint.h>
#include <string.h>

void* ENDIAN_memcpy(void* dest, const void* src, size_t size)
{
#ifdef IS_LITTLE_ENDIAN
  // 内部でcopyと同等のことを行っている
  ENDIAN_conv(dest, src, size);
#else
  memcpy(dest, src, size);
#endif

  return dest;
}

void ENDIAN_conv(void* after, const void* before, size_t size)
{
  const uint8_t* bef = (const uint8_t*)before;
  uint8_t* aft = (uint8_t*)after;
  size_t i;

  if (size < 0) return;

  size--;
  for (i = 0; i <= size; i++)
  {
    *(aft + (size - i)) = *(bef + i);
  }

  return;
}

ENDIAN_TYPE ENDIAN_detect_system_endian(void)
{
  uint32_t test = 0x12345678;
  uint8_t* p = (uint8_t*)&test;

  if (*p == 0x12)
  {
    return ENDIAN_TYPE_BIG;
  }
  else if (*p == 0x78)
  {
    return ENDIAN_TYPE_LITTLE;
  }
  else
  {
    return ENDIAN_TYPE_UNKNOWN;
  }
}

#pragma section
