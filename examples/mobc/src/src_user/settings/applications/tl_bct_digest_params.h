/**
 * @file
 * @brief  TL_BCT_DIGEST の CoreApp パラメタのオーバーライド用ヘッダー
 */
#ifndef TL_BCT_DIGEST_PARAMS_H_
#define TL_BCT_DIGEST_PARAMS_H_

#include <src_core/tlm_cmd/packet_handler.h>

#undef TL_BCT_DIGSET_TL_DIGEST_PAGE_SIZE

#define TL_BCT_DIGSET_TL_DIGEST_PAGE_SIZE   (TL_TLM_PAGE_SIZE * 2)

#endif
