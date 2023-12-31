/**
 * @file
 * @brief Memory Dump  の各種定義
 */
#ifndef MEMORY_DUMP_DEFINE_H_
#define MEMORY_DUMP_DEFINE_H_

#include "../tlm_cmd/ccsds/apid_define.h"

#define MEM_DUMP_TLM_APID  (APID_TLM_DUMP)   //!< MemoryDump で利用する APID
#define MEM_DUMP_TLM_ID    ((TLM_CODE)0x00)  //!< MemoryDump で利用する TLM ID

#endif
