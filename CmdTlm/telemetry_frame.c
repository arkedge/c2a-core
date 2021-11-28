#pragma section REPRO
/**
 * @file telemetry_frame.c
 * @brief  �e�����g���̓o�^�C�e�����g���֘A����
 */
#include "telemetry_frame.h"

#include <src_user/Library/stdint.h>
#include <src_user/CmdTlm/telemetry_definitions.h>
#include "../Library/print.h"
#include "../Library/endian_memcpy.h"
#include <src_user/Settings/build_settings.h>

static TlmInfo tlm_table_[TLM_MAX_TLMS];
const TlmInfo* tlm_table;
static int TF_page_no_;
const int* TF_page_no;

static void initialize_tlm_table_(void);

int TF_generate_contents(int packet_id,
                         unsigned char* contents,
                         int max_len)
{
  int (*tlm_func)(unsigned char*, int)
    = tlm_table_[packet_id].tlm_func;

  if (tlm_func != NULL)
  {
    return tlm_func(contents, max_len);
  }
  else
  {
    return TLM_NOT_DEFINED;
  }
}

void TF_initialize(void)
{
  // tlm_table_��������
  initialize_tlm_table_();

  if (TLM_MAX_TLMS <= TLM_CODE_MAX)
  {
    Printf("TF: init error!!!\n");
    return;
  }

  TF_load_tlm_table(tlm_table_);
}

static void initialize_tlm_table_(void)
{
  int i;
  for (i = 0; i < TLM_MAX_TLMS; ++i)
  {
    tlm_table_[i].tlm_func = NULL;
  }

  tlm_table = tlm_table_;

  // �y�[�W�ԍ��̏����l��0�ɐݒ�
  TF_page_no_ = 0;
  TF_page_no = &TF_page_no_;
}

void TF_copy_u8(uint8_t* ptr,
                uint8_t data)
{
  ptr[0] = data;
}

void TF_copy_u16(uint8_t* ptr,
                 uint16_t data)
{
  ptr[0] = (uint8_t)((data >>  8) & 0xff);
  ptr[1] = (uint8_t)(        data & 0xff);
}

void TF_copy_u32(uint8_t* ptr,
                 uint32_t data)
{
  ptr[0] = (uint8_t)((data >> 24) & 0xff);
  ptr[1] = (uint8_t)((data >> 16) & 0xff);
  ptr[2] = (uint8_t)((data >>  8) & 0xff);
  ptr[3] = (uint8_t)(        data & 0xff);
}

void TF_copy_i8(uint8_t* ptr,
                int8_t data)
{
  ptr[0] = (uint8_t)data;
}

void TF_copy_i16(uint8_t* ptr,
                 int16_t data)
{
  ptr[0] = (uint8_t)((data >>  8) & 0xff);
  ptr[1] = (uint8_t)(        data & 0xff);
}

void TF_copy_i32(uint8_t* ptr,
                 int32_t data)
{
  ptr[0] = (uint8_t)((data >> 24) & 0xff);
  ptr[1] = (uint8_t)((data >> 16) & 0xff);
  ptr[2] = (uint8_t)((data >>  8) & 0xff);
  ptr[3] = (uint8_t)(        data & 0xff);
}

void TF_copy_float(uint8_t* ptr,
                   float data)
{
  uint8_t* temp = (uint8_t*)&data;
  int i;

  for (i = 0; i < sizeof(float); ++i)
  {
#ifdef IS_LITTLE_ENDIAN
    // Little Endian��z�肵���R�[�h�B
    ptr[i] = temp[sizeof(float) - i - 1];
#else
    // Big Endian��z�肵���R�[�h�B
    ptr[i] = temp[i];
#endif
  }
}

void TF_copy_double(uint8_t* ptr,
                    double data)
{
  uint8_t* temp = (uint8_t*)&data;
  int i;

  for (i = 0; i < sizeof(double); ++i)
  {
#ifdef IS_LITTLE_ENDIAN
    // Little Endian��z�肵���R�[�h�B
    ptr[i] = temp[sizeof(double) - i - 1];
#else
    // Big Endian��z�肵���R�[�h�B
    ptr[i] = temp[i];
#endif
  }
}

CCP_EXEC_STS Cmd_TF_REGISTER_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t index;
  uint32_t tlm_func;

  endian_memcpy(&index, param, 1);
  endian_memcpy(&tlm_func, param + 1, 4);

  if (index >= TLM_MAX_TLMS)
  {
    // �o�^�w��ʒu���e�����g��������𒴂��Ă���ꍇ�ُ͈픻��
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  tlm_table_[index].tlm_func
    = (int (*)(unsigned char*, int))tlm_func;

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_TF_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  uint8_t page;

  page = CCP_get_param_head(packet)[0];
  if (page >= TF_TLM_PAGE_MAX)
  {
    // �y�[�W�ԍ����R�}���h�e�[�u���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  TF_page_no_ = page;
  return CCP_EXEC_SUCCESS;
}

#pragma section