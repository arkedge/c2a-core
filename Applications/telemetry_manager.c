#pragma section REPRO
/**
 * @file  telemetry_manager.c
 * @brief user���e�����g���l�܂����TL����h���C�܂�CDH�Ȃǂ��e�����g�����Ǘ����₷�����邽�߂�App
 * @note  https://gitlab.com/ut_issl/c2a/c2a_core_oss/-/issues/81 �� telemetry_manager.h �̍ŉ������Q�ƁiFIXME: ���Ƃ�document�Ɉڂ��j
 */
#include "telemetry_manager.h"

#include <string.h>     // for memset�Ȃǂ�mem�n

#include "./divided_cmd_utility.h"
#include "../CmdTlm/block_command_loader.h"
#include "../CmdTlm/block_command_executor.h"
#include "../CmdTlm/command_analyze.h"
#include "../Library/print.h"
#include "../Library/endian_memcpy.h"
#include "../System/WatchdogTimer/watchdog_timer.h"
#include <src_user/CmdTlm/block_command_definitions.h>
#include <src_user/CmdTlm/command_definitions.h>

/**
 * @brief  App�������֐�
 * @param  void
 * @return void
 */
static void TLM_MGR_init_by_am_(void);
/**
 * @brief  ������
 * @note   ���s���Ԃ̖�肩�炵�������Ă���
 * @param  void
 * @return 0:OK, 1:NG
 */
static uint8_t TLM_MGR_init_1_(void);
static uint8_t TLM_MGR_init_2_(void);
static uint8_t TLM_MGR_init_3_(void);
static uint8_t TLM_MGR_init_4_(void);
/**
 * @brief  AppInfo�\���̂̃N���A
 * @param  void
 * @return void
 */
static void TLM_MGR_clear_info_(void);
/**
 * @brief  ���ׂĂ�TLM_MGR_RegisterInfo�̃N���A
 * @param  void
 * @return void
 */
static void TLM_MGR_clear_register_info_all_(void);
/**
 * @brief  TLM_MGR_RegisterInfo�̃N���A
 * @param  register_info: �N���A������ TLM_MGR_RegisterInfo
 * @return void
 */
static void TLM_MGR_clear_register_info_(TLM_MGR_RegisterInfo* register_info);
/**
 * @brief  ���ׂĂ�BC�� NOP x TLM_MGR_MAX_TLM_NUM_PER_BC �Ŗ��߂�
 * @param  void
 * @return void
 */
static void TLM_MGR_clear_bc_to_nop_all_(void);
/**
 * @brief  �w�肵��BC�� NOP x TLM_MGR_MAX_TLM_NUM_PER_BC �Ŗ��߂�
 * @param  bc_id: NOP�ł��߂�BC ID
 * @return void
 */
static void TLM_MGR_clear_bc_to_nop_(bct_id_t bc_id);
/**
 * @brief  �ݒ肳�ꂽ TLM_MGR_BcInfo ��������Ŏg����� TLM_MGR_RegisterInfo �̍\�z
 * @param  void
 * @return TLM_MGR_ERR_CODE
 */
static TLM_MGR_ERR_CODE TLM_MGR_calc_register_info_from_bc_info_(void);
/**
 * @brief  TLM_MGR_RegisterInfo �Ɏg�� BC ����o�^
 * @param  register_info: �o�^��� TLM_MGR_RegisterInfo
 * @return TLM_MGR_ERR_CODE
 */
static TLM_MGR_ERR_CODE TLM_MGR_add_bc_info_to_register_info_(TLM_MGR_RegisterInfo* register_info, uint8_t bc_info_idx);
/**
 * @brief  TLM_MGR_RegisterInfo �o�^����Ă��� BC ���N���A���� NOP �Ŗ��߂�
 * @param  register_info: ���� BC ���o�^����Ă��� TLM_MGR_RegisterInfo
 * @return void
 */
static void TLM_MGR_clear_bc_of_register_info_(TLM_MGR_RegisterInfo* register_info);
/**
 * @brief  TLM_MGR_RegisterInfo �� �e���������R�}���h��o�^����
 * @param  register_info: �R�}���h�o�^�� BC ���o�^����Ă��� TLM_MGR_RegisterInfo
 * @param  param: Cmd_GENERATE_TLM �̃R�}���h�p�����^
 * @return TLM_MGR_ERR_CODE
 */
static TLM_MGR_ERR_CODE TLM_MGR_register_generate_tlm_(TLM_MGR_RegisterInfo* register_info, const uint8_t* param);
/**
 * @brief  BC�S�̂�W�J���Ă���master BC�̍\�z
 * @param  void
 * @return void
 */
static void TLM_MGR_load_master_bc_(void);
/**
 * @brief  NOP �Ŗ��߂�ꂽ BC�̍\�z
 * @param  void
 * @return void
 */
static void TLM_MGR_load_nop_bc_(void);


static TelemetryManager telemetry_manager_;
const TelemetryManager* const telemetry_manager = &telemetry_manager_;

static CTCP TLM_MGR_packet_;

AppInfo TLM_MGR_create_app(void)
{
  return create_app_info("telemetry_manager", TLM_MGR_init_by_am_, NULL);
}


static void TLM_MGR_init_by_am_(void)
{
  telemetry_manager_.is_inited = 0;
}


// FIXME: ���s���Ԃ�΂��D Cmd_TLM_MGR_INIT �𒼂����ɒ���
// BCT�̏��������O�Ȃ̂ŁCAppInit�ɂł��Ȃ��D
static uint8_t TLM_MGR_init_1_(void)
{
  telemetry_manager_.is_inited = 0;

  TLM_MGR_clear_info_();

  // BC�̐ݒ�
  telemetry_manager_.bc_info[0].bc_id = BC_TLM_MGR0;
  telemetry_manager_.bc_info[1].bc_id = BC_TLM_MGR1;
  telemetry_manager_.bc_info[2].bc_id = BC_TLM_MGR2;
  telemetry_manager_.bc_info[3].bc_id = BC_TLM_MGR3;
  telemetry_manager_.bc_info[4].bc_id = BC_TLM_MGR4;
  telemetry_manager_.bc_info[5].bc_id = BC_TLM_MGR5;
  telemetry_manager_.bc_info[6].bc_id = BC_TLM_MGR6;
  telemetry_manager_.bc_info[7].bc_id = BC_TLM_MGR7;
  telemetry_manager_.bc_info[8].bc_id = BC_TLM_MGR8;
  telemetry_manager_.bc_info[9].bc_id = BC_TLM_MGR9;
  telemetry_manager_.bc_info[0].bc_type = TLM_MGR_BC_TYPE_MASTER;
  telemetry_manager_.bc_info[1].bc_type = TLM_MGR_BC_TYPE_HK_TLM;
  telemetry_manager_.bc_info[2].bc_type = TLM_MGR_BC_TYPE_SYSTEM_TLM;
  telemetry_manager_.bc_info[3].bc_type = TLM_MGR_BC_TYPE_SYSTEM_TLM;
  telemetry_manager_.bc_info[4].bc_type = TLM_MGR_BC_TYPE_SYSTEM_TLM;
  // telemetry_manager_.bc_info[4].bc_type = TLM_MGR_BC_TYPE_RESERVE;       // reserve �ł��߂Ă��ǂ�
  telemetry_manager_.bc_info[5].bc_type = TLM_MGR_BC_TYPE_HIGH_FREQ_TLM;
  telemetry_manager_.bc_info[6].bc_type = TLM_MGR_BC_TYPE_HIGH_FREQ_TLM;
  telemetry_manager_.bc_info[7].bc_type = TLM_MGR_BC_TYPE_HIGH_FREQ_TLM;
  telemetry_manager_.bc_info[8].bc_type = TLM_MGR_BC_TYPE_HIGH_FREQ_TLM;
  telemetry_manager_.bc_info[9].bc_type = TLM_MGR_BC_TYPE_LOW_FREQ_TLM;

  return 0;
}


static uint8_t TLM_MGR_init_2_(void)
{
  TLM_MGR_ERR_CODE ret;

  // BC�ݒ肩������Ŏg�����̍\�z
  ret = TLM_MGR_calc_register_info_from_bc_info_();
  if (ret != TLM_MGR_ERR_CODE_OK)
  {
    // ���������s
    // Printf("TLM MGR init Failed at calc_register_info !\n");
    return 1;
  }

  return 0;
}


static uint8_t TLM_MGR_init_3_(void)
{
  TLM_MGR_clear_bc_to_nop_all_();
  return 0;
}


static uint8_t TLM_MGR_init_4_(void)
{
  BCL_load_bc(telemetry_manager_.master_bc_id, TLM_MGR_load_master_bc_);

  telemetry_manager_.is_inited = 1;
  return 0;
}


static void TLM_MGR_clear_info_(void)
{
  uint8_t bc_info_idx;

  for (bc_info_idx = 0; bc_info_idx < TLM_MGR_USE_BC_NUM; ++bc_info_idx)
  {
    telemetry_manager_.bc_info[bc_info_idx].bc_id   = BCT_MAX_BLOCKS;
    telemetry_manager_.bc_info[bc_info_idx].bc_type = TLM_MGR_BC_TYPE_RESERVE;
  }
  telemetry_manager_.master_bc_id = BCT_MAX_BLOCKS;

  TLM_MGR_clear_register_info_all_();
}


static void TLM_MGR_clear_register_info_all_(void)
{
  TLM_MGR_clear_register_info_(&telemetry_manager_.register_info.master);
  TLM_MGR_clear_register_info_(&telemetry_manager_.register_info.hk_tlm);
  TLM_MGR_clear_register_info_(&telemetry_manager_.register_info.system_tlm);
  TLM_MGR_clear_register_info_(&telemetry_manager_.register_info.high_freq_tlm);
  TLM_MGR_clear_register_info_(&telemetry_manager_.register_info.low_freq_tlm);
  TLM_MGR_clear_register_info_(&telemetry_manager_.register_info.reserve);
}


static void TLM_MGR_clear_register_info_(TLM_MGR_RegisterInfo* register_info)
{
  memset(register_info, 0x00, sizeof(*register_info));
}


static void TLM_MGR_clear_bc_to_nop_all_(void)
{
  uint8_t bc_info_idx;

  for (bc_info_idx = 0; bc_info_idx < TLM_MGR_USE_BC_NUM; ++bc_info_idx)
  {
    TLM_MGR_clear_bc_to_nop_(telemetry_manager_.bc_info[bc_info_idx].bc_id);
    WDT_clear_wdt();      // TODO: ���s���Ԃ��m�F���ď���
  }
}

static void TLM_MGR_clear_bc_to_nop_(bct_id_t bc_id)
{
  BCL_load_bc(bc_id, TLM_MGR_load_nop_bc_);
  // ���̒��� BCT_activate_block() ������邱�Ƃɒ���
}


static TLM_MGR_ERR_CODE TLM_MGR_calc_register_info_from_bc_info_(void)
{
  uint8_t bc_info_idx;
  TLM_MGR_RegisterInfo* register_info_master;

  TLM_MGR_clear_register_info_all_();    // TODO: �������̂��߂ɏ����Ă����������H

  for (bc_info_idx = 0; bc_info_idx < TLM_MGR_USE_BC_NUM; ++bc_info_idx)
  {
    TLM_MGR_ERR_CODE ret = TLM_MGR_ERR_CODE_OK;
    switch (telemetry_manager_.bc_info[bc_info_idx].bc_type)
    {
    case TLM_MGR_BC_TYPE_MASTER:
      ret = TLM_MGR_add_bc_info_to_register_info_(&telemetry_manager_.register_info.master, bc_info_idx);
      break;
    case TLM_MGR_BC_TYPE_HK_TLM:
      ret = TLM_MGR_add_bc_info_to_register_info_(&telemetry_manager_.register_info.hk_tlm, bc_info_idx);
      break;
    case TLM_MGR_BC_TYPE_SYSTEM_TLM:
      ret = TLM_MGR_add_bc_info_to_register_info_(&telemetry_manager_.register_info.system_tlm, bc_info_idx);
      break;
    case TLM_MGR_BC_TYPE_HIGH_FREQ_TLM:
      ret = TLM_MGR_add_bc_info_to_register_info_(&telemetry_manager_.register_info.high_freq_tlm, bc_info_idx);
      break;
    case TLM_MGR_BC_TYPE_LOW_FREQ_TLM:
      ret = TLM_MGR_add_bc_info_to_register_info_(&telemetry_manager_.register_info.low_freq_tlm, bc_info_idx);
      break;
    case TLM_MGR_BC_TYPE_RESERVE:
      ret = TLM_MGR_add_bc_info_to_register_info_(&telemetry_manager_.register_info.reserve, bc_info_idx);
      break;
    default:
      return TLM_MGR_ERR_CODE_OTHER_ERR;
      break;
    }
    if (ret != TLM_MGR_ERR_CODE_OK) return ret;
  }

  register_info_master = &telemetry_manager_.register_info.master;

  if (register_info_master->bc_info_idx_used_num == 0) return TLM_MGR_ERR_CODE_MASTER_IS_ABSENT;
  if (register_info_master->bc_info_idx_used_num > 1)  return TLM_MGR_ERR_CODE_MASTER_DUPLICATED;

  bc_info_idx = register_info_master->bc_info_idx[0];
  telemetry_manager_.master_bc_id = telemetry_manager_.bc_info[bc_info_idx].bc_id;
  return TLM_MGR_ERR_CODE_OK;
}


static TLM_MGR_ERR_CODE TLM_MGR_add_bc_info_to_register_info_(TLM_MGR_RegisterInfo* register_info, uint8_t bc_info_idx)
{
  if (register_info->bc_info_idx_used_num >= TLM_MGR_USE_BC_NUM) return TLM_MGR_ERR_CODE_REGISTER_INFO_BC_FULL;

  register_info->bc_info_idx[register_info->bc_info_idx_used_num] = bc_info_idx;
  register_info->bc_info_idx_used_num++;

  return TLM_MGR_ERR_CODE_OK;
}


static void TLM_MGR_clear_bc_of_register_info_(TLM_MGR_RegisterInfo* register_info)
{
  uint8_t i;

  for (i = 0; i < register_info->bc_info_idx_used_num; ++i)
  {
    uint8_t bc_info_idx = register_info->bc_info_idx[i];
    TLM_MGR_clear_bc_to_nop_(telemetry_manager_.bc_info[bc_info_idx].bc_id);
  }
  register_info->tlm_register_pointer_to_idx_of_bc_info_idx = 0;
  register_info->tlm_register_pointer_to_bc_cmd_idx = 0;
}


static TLM_MGR_ERR_CODE TLM_MGR_register_generate_tlm_(TLM_MGR_RegisterInfo* register_info, const uint8_t* param)
{
  uint8_t  bc_info_idx = register_info->bc_info_idx[register_info->tlm_register_pointer_to_idx_of_bc_info_idx];
  bct_id_t bc_id = telemetry_manager_.bc_info[bc_info_idx].bc_id;
  uint8_t  bc_cmd_pos = register_info->tlm_register_pointer_to_bc_cmd_idx;
  BCT_Pos  bc_register_pos;
  CTCP_UTIL_ACK ctcp_util_ack;
  BCT_ACK  bct_ack;

  if (bc_cmd_pos >= TLM_MGR_MAX_TLM_NUM_PER_BC) return TLM_MGR_ERR_CODE_CMD_FULL;

  BCT_make_pos(&bc_register_pos, bc_id, bc_cmd_pos);

  ctcp_util_ack = CCP_form_tlc(&TLM_MGR_packet_,
                               (cycle_t)bc_cmd_pos,
                               Cmd_CODE_GENERATE_TLM,
                               param,
                               cmd_table[Cmd_CODE_GENERATE_TLM].param_len);
  if (ctcp_util_ack != CTCP_UTIL_ACK_OK) return TLM_MGR_ERR_CODE_OTHER_ERR;

  // ���ł� NOP �Ŗ��߂��Ă���C���� activate �ςȂ̂ŁC���� BCT �ɃR�}���h��}���ł���D
  bct_ack = BCT_overwrite_cmd(&bc_register_pos, &TLM_MGR_packet_);
  if (bct_ack != BCT_SUCCESS) return TLM_MGR_ERR_CODE_OTHER_ERR;

  register_info->tlm_register_pointer_to_idx_of_bc_info_idx++;
  if (register_info->tlm_register_pointer_to_idx_of_bc_info_idx >= register_info->bc_info_idx_used_num)
  {
    register_info->tlm_register_pointer_to_idx_of_bc_info_idx %= register_info->bc_info_idx_used_num;
    register_info->tlm_register_pointer_to_bc_cmd_idx++;
  }

  return TLM_MGR_ERR_CODE_OK;
}


static void TLM_MGR_load_master_bc_(void)
{
  cycle_t ti = 1;   // 1 - 9 �܂ł� 9 �o�^����D 10 ��deploy
  uint8_t bc_info_idx;

  for (bc_info_idx = 0; bc_info_idx < TLM_MGR_USE_BC_NUM; ++bc_info_idx)
  {
    switch (telemetry_manager_.bc_info[bc_info_idx].bc_type)
    {
    case TLM_MGR_BC_TYPE_HK_TLM:        // FALLTHROUGH
    case TLM_MGR_BC_TYPE_SYSTEM_TLM:    // FALLTHROUGH
    case TLM_MGR_BC_TYPE_HIGH_FREQ_TLM: // FALLTHROUGH
    case TLM_MGR_BC_TYPE_RESERVE:
      BCL_tool_register_combine(ti, telemetry_manager_.bc_info[bc_info_idx].bc_id);
      ti++;
      break;
    case TLM_MGR_BC_TYPE_LOW_FREQ_TLM:
      BCL_tool_register_rotate(ti, telemetry_manager_.bc_info[bc_info_idx].bc_id);
      ti++;
      break;
    default:
      // TLM_MGR_BC_TYPE_MASTER ��������
      break;
    }
  }

  BCL_tool_register_deploy(TLM_MGR_USE_BC_NUM, telemetry_manager_.master_bc_id, TL_ID_DEPLOY_TLM);
}


static void TLM_MGR_load_nop_bc_(void)
{
  cycle_t ti;

  for (ti = 0; ti < TLM_MGR_MAX_TLM_NUM_PER_BC; ++ti)
  {
    BCL_tool_register_cmd(ti, Cmd_CODE_NOP);
  }
}


// FIXME: ���s���Ԃ�΂��F 21ms
// �K���ɕ������Ȃ���
CCP_EXEC_STS Cmd_TLM_MGR_INIT(const CTCP* packet)
{
  uint8_t ret;
  uint16_t exec_counter;
  (void)packet;

  switch (DCU_check_in(Cmd_CODE_TLM_MGR_INIT, &exec_counter))
  {
  case DCU_STATUS_FINISHED:   // FALLTHROUGH
  case DCU_STATUS_PROGRESS:
    break;
  default:
    // DCU_STATUS_ABORTED_BY_ERR
    // DCU_STATUS_ABORTED_BY_CMD
    // ��������
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  switch (exec_counter)
  {
  case 0:
    ret = TLM_MGR_init_1_();
    break;
  case 1:
    ret = TLM_MGR_init_2_();
    break;
  case 2:
    ret = TLM_MGR_init_3_();    // ���ꂪ21ms�DNOP BC�����̂��d��
    break;
  case 3:
    ret = TLM_MGR_init_4_();
    break;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  if (ret != 0)
  {
    DCU_report_err(Cmd_CODE_TLM_MGR_INIT, CCP_EXEC_ILLEGAL_CONTEXT);
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  if (exec_counter == 3)
  {
    DCU_report_finish(Cmd_CODE_TLM_MGR_INIT, CCP_EXEC_SUCCESS);
    return CCP_EXEC_SUCCESS;
  }

  // �ċA���s
  if (DCU_register_next(Cmd_CODE_TLM_MGR_INIT, NULL, 0) != DCU_ACK_OK)
  {
    DCU_report_err(Cmd_CODE_TLM_MGR_INIT, CCP_EXEC_ILLEGAL_CONTEXT);
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_INIT_MASTER_BC(const CTCP* packet)
{
  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  // TODO: TLM_MGR_calc_register_info_from_bc_info_ �͓���Ȃ��Ă�������������
  //       �Ƃ肠�����͂Ȃ��Ă����C������

  BCL_load_bc(telemetry_manager_.master_bc_id, TLM_MGR_load_master_bc_);

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_HK_TLM(const CTCP* packet)
{
  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  TLM_MGR_clear_bc_of_register_info_(&telemetry_manager_.register_info.hk_tlm);

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_SYSTEM_TLM(const CTCP* packet)
{
  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  TLM_MGR_clear_bc_of_register_info_(&telemetry_manager_.register_info.system_tlm);

  return CCP_EXEC_SUCCESS;
}


// FIXME: ���s���ԃ`�F�b�N :9ms
// ���ǁCNOP BC���̂��d��
CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_USER_TLM(const CTCP* packet)
{
  uint16_t exec_counter;
  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  switch (DCU_check_in(Cmd_CODE_TLM_MGR_CLEAR_USER_TLM, &exec_counter))
  {
  case DCU_STATUS_FINISHED:   // FALLTHROUGH
  case DCU_STATUS_PROGRESS:
    break;
  default:
    // DCU_STATUS_ABORTED_BY_ERR
    // DCU_STATUS_ABORTED_BY_CMD
    // ��������
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  switch (exec_counter)
  {
  case 0:
    TLM_MGR_clear_bc_of_register_info_(&telemetry_manager_.register_info.high_freq_tlm);
    break;
  case 1:
    TLM_MGR_clear_bc_of_register_info_(&telemetry_manager_.register_info.low_freq_tlm);
    break;
  default:
    TLM_MGR_clear_bc_of_register_info_(&telemetry_manager_.register_info.reserve);     // �֋X�ケ����
    DCU_report_finish(Cmd_CODE_TLM_MGR_CLEAR_USER_TLM, CCP_EXEC_SUCCESS);
    return CCP_EXEC_SUCCESS;
  }

  // �ċA���s
  if (DCU_register_next(Cmd_CODE_TLM_MGR_CLEAR_USER_TLM, NULL, 0) != DCU_ACK_OK)
  {
    DCU_report_err(Cmd_CODE_TLM_MGR_CLEAR_USER_TLM, CCP_EXEC_ILLEGAL_CONTEXT);
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_START_TLM(const CTCP* packet)
{
  BCT_Pos  bc_register_pos;
  bct_id_t master_bc_id;
  uint8_t  bc_cmd_pos;
  uint8_t  param[1 + SIZE_OF_BCT_ID_T];

  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  // master BC �� 1�łȂ��͉̂�������������
  if (telemetry_manager_.register_info.master.bc_info_idx_used_num != 1)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  // master bc �̖����� nop �� deploy �ɍ����ւ���
  master_bc_id = telemetry_manager_.master_bc_id;
  bc_cmd_pos = TLM_MGR_USE_BC_NUM - 1;

  BCT_make_pos(&bc_register_pos, master_bc_id, bc_cmd_pos);
  param[0] = TL_ID_DEPLOY_TLM;
  endian_memcpy(&param[1], &master_bc_id, SIZE_OF_BCT_ID_T);

  CCP_form_tlc(&TLM_MGR_packet_,
               TLM_MGR_USE_BC_NUM,
               Cmd_CODE_TLCD_DEPLOY_BLOCK,
               param,
               1 + SIZE_OF_BCT_ID_T);
  BCT_overwrite_cmd(&bc_register_pos, &TLM_MGR_packet_);

  // master bc �W�J
  CCP_form_block_deploy_cmd(&TLM_MGR_packet_, TL_ID_DEPLOY_TLM, master_bc_id);
  PH_analyze_packet(&TLM_MGR_packet_);

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_STOP_TLM(const CTCP* packet)
{
  BCT_Pos  bc_register_pos;
  bct_id_t master_bc_id;
  uint8_t  bc_cmd_pos;

  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  // master BC �� 1�łȂ��͉̂�������������
  if (telemetry_manager_.register_info.master.bc_info_idx_used_num != 1)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  // master bc �̖����� deploy �� nop �ɍ����ւ��邱�ƂŎ~�߂�
  master_bc_id = telemetry_manager_.master_bc_id;
  bc_cmd_pos = TLM_MGR_USE_BC_NUM - 1;

  BCT_make_pos(&bc_register_pos, master_bc_id, bc_cmd_pos);
  CCP_form_tlc(&TLM_MGR_packet_,
               TLM_MGR_USE_BC_NUM,
               Cmd_CODE_NOP,
               NULL,
               0);
  BCT_overwrite_cmd(&bc_register_pos, &TLM_MGR_packet_);

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_TLM_MGR_CLEAR_TLM_TL(const CTCP* packet)
{
  uint8_t param[1];

  (void)packet;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  param[0] = TL_ID_DEPLOY_TLM;
  CCP_form_rtc(&TLM_MGR_packet_, Cmd_CODE_TLCD_CLEAR_ALL_TIMELINE, param, 1);
  PH_analyze_packet(&TLM_MGR_packet_);

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_HK_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  TLM_MGR_ERR_CODE ret;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  if (CA_ckeck_cmd_param_len(Cmd_CODE_GENERATE_TLM, CCP_get_param_len(packet)) != CA_ACK_OK)
  {
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  ret = TLM_MGR_register_generate_tlm_(&telemetry_manager_.register_info.hk_tlm, param);
  if (ret != TLM_MGR_ERR_CODE_OK) return CCP_EXEC_ILLEGAL_CONTEXT;

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_SYSTEM_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  TLM_MGR_ERR_CODE ret;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  if (CA_ckeck_cmd_param_len(Cmd_CODE_GENERATE_TLM, CCP_get_param_len(packet)) != CA_ACK_OK)
  {
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  ret = TLM_MGR_register_generate_tlm_(&telemetry_manager_.register_info.system_tlm, param);
  if (ret != TLM_MGR_ERR_CODE_OK) return CCP_EXEC_ILLEGAL_CONTEXT;

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_HIGH_FREQ_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  TLM_MGR_ERR_CODE ret;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  if (CA_ckeck_cmd_param_len(Cmd_CODE_GENERATE_TLM, CCP_get_param_len(packet)) != CA_ACK_OK)
  {
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  ret = TLM_MGR_register_generate_tlm_(&telemetry_manager_.register_info.high_freq_tlm, param);
  if (ret != TLM_MGR_ERR_CODE_OK) return CCP_EXEC_ILLEGAL_CONTEXT;

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_TLM_MGR_REGISTER_LOW_FREQ_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  TLM_MGR_ERR_CODE ret;

  if (telemetry_manager_.is_inited == 0) return CCP_EXEC_ILLEGAL_CONTEXT;

  if (CA_ckeck_cmd_param_len(Cmd_CODE_GENERATE_TLM, CCP_get_param_len(packet)) != CA_ACK_OK)
  {
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  ret = TLM_MGR_register_generate_tlm_(&telemetry_manager_.register_info.low_freq_tlm, param);
  if (ret != TLM_MGR_ERR_CODE_OK) return CCP_EXEC_ILLEGAL_CONTEXT;

  return CCP_EXEC_SUCCESS;
}

#pragma section