/**
 * @file  event_logger_params.h
 * @brief event_logger �̊e��ݒ�
 *        �e�p�����^�ނ̏ڍׂ� event_logger.h ���Q�Ƃ��邱��
 */
#ifndef EVENT_LOGGER_PARAMS_H_
#define EVENT_LOGGER_PARAMS_H_

#undef EL_TLOG_TLM_PAGE_SIZE
#undef EL_TLOG_TLM_PAGE_MAX_HIGH
#undef EL_TLOG_TLM_PAGE_MAX_MIDDLE
#undef EL_TLOG_TLM_PAGE_MAX_LOW
#undef EL_CLOG_TLM_PAGE_SIZE
#undef EL_CLOG_TLM_PAGE_MAX_HIGH
#undef EL_CLOG_TLM_PAGE_MAX_MIDDLE
#undef EL_CLOG_TLM_PAGE_MAX_LOW

#undef EL_TLOG_SIZE_MAX_EL
#undef EL_CLOG_SIZE_MAX_EL

#undef EL_IS_ENABLE_TLOG
#undef EL_IS_ENABLE_CLOG
#undef EL_IS_ENABLE_EVENT_NOTE
#undef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#undef EL_IS_ENABLE_EL_ERROR_LEVEL

#undef SIZE_OF_EL_CLOG_COUNTER_T


#define EL_TLOG_TLM_PAGE_SIZE         (36)
#define EL_TLOG_TLM_PAGE_MAX_HIGH     (4)
#define EL_TLOG_TLM_PAGE_MAX_MIDDLE   (4)
#define EL_TLOG_TLM_PAGE_MAX_LOW      (4)
#define EL_CLOG_TLM_PAGE_SIZE         (22)
#define EL_CLOG_TLM_PAGE_MAX_HIGH     (4)
#define EL_CLOG_TLM_PAGE_MAX_MIDDLE   (4)
#define EL_CLOG_TLM_PAGE_MAX_LOW      (4)

#define EL_TLOG_SIZE_MAX_EL           (16)
#define EL_CLOG_SIZE_MAX_EL           (8)

#define EL_IS_ENABLE_TLOG
#define EL_IS_ENABLE_CLOG
#define EL_IS_ENABLE_EVENT_NOTE
#define EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#define EL_IS_ENABLE_EL_ERROR_LEVEL

#define SIZE_OF_EL_CLOG_COUNTER_T     (2)

#endif