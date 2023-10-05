/**
 * @file
 * @brief  Framingパラメタのオーバーライド用ヘッダー
 */
#ifndef FRAMING_PARAMS_H_
#define FRAMING_PARAMS_H_

#undef FRM_STREAM_MAX
#undef FRM_IF_RX_BUFFER_SIZE

#define FRM_STREAM_MAX           (3)
#define FRM_IF_RX_BUFFER_SIZE    (1024)

#endif
