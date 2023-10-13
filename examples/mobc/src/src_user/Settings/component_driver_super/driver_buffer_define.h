/**
 * @file
 * @brief  Driver の各種バッファサイズなどの定義
 */
#ifndef DRIVER_BUFFER_DEFINE_H_
#define DRIVER_BUFFER_DEFINE_H_

#include "./driver_super_params.h"

#define CDS_STREAM_REC_BUFFER_SIZE_DEFAULT   (CDS_IF_RX_BUFFER_SIZE * 2)    /*!< CDS_StreamRecBuffer のバッファサイズのデフォルト値
                                                                               UART などの非同期通信はメモリに余力があれば CDS_IF_RX_BUFFER_SIZE * 2 を推奨
                                                                               詳細は dirver_super.c の @note を参照 */

#endif
