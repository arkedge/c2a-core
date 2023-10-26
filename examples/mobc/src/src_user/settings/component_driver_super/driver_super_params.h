/**
 * @file
 * @brief  ComponentDriverSuperパラメタのオーバーライド用ヘッダー
 */
#ifndef DRIVER_SUPER_PARAMS_H_
#define DRIVER_SUPER_PARAMS_H_

#undef CDS_STREAM_MAX
#undef CDS_HAL_RX_BUFFER_SIZE

#define CDS_STREAM_MAX           (3)
#define CDS_HAL_RX_BUFFER_SIZE   (1024)

#endif
