/**
 * @file
 * @brief  Driver の各種バッファサイズなどの定義
 */
#ifndef FRAMING_BUFFER_DEFINE_H_
#define FRAMING_BUFFER_DEFINE_H_

#include "./framing_params.h"

#define FRM_STREAM_REC_BUFFER_SIZE_DEFAULT   (FRM_IF_RX_BUFFER_SIZE * 2)    /*!< FRM_StreamRecBuffer のバッファサイズのデフォルト値
                                                                               UART などの非同期通信はメモリに余力があれば FRM_IF_RX_BUFFER_SIZE * 2 を推奨
                                                                               詳細は dirver_super.c の @note を参照 */

#endif
