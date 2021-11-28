/**
 * @file   command_analyze_params.h
 * @brief  CA��CoreTlm�p�����^�̃I�[�o�[���C�h�p�w�b�_�[
 */
#ifndef COMMAND_ANALYZE_PARAMS_H_
#define COMMAND_ANALYZE_PARAMS_H_

#undef CA_TLM_PAGE_SIZE
#undef CA_TLM_PAGE_MAX
#undef CMD_MAX_CMDS

// 1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����R�}���h��
#define CA_TLM_PAGE_SIZE (64)
// �R�}���h�e�[�u���y�[�W��
#define CA_TLM_PAGE_MAX (24)
// �R�}���h�e�[�u���T�C�Y
#define CMD_MAX_CMDS (CA_TLM_PAGE_SIZE * CA_TLM_PAGE_MAX)

#endif