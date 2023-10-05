#pragma section REPRO
/**
 * @file
 * @brief I2C 通信関連の一般 IO 以外の公開関数
 * @note  レジスタの読み書き等，ハードウェアに依存しない関数群を定義する
 */

#include "i2c_common.h"
#include "../library/endian.h"
#include <string.h>

/**
 * @brief I2C_Config 構造体にて指定されたデバイスのレジスタへ書き込む
 * @param[in] p_framing  : Framing 構造体へのポインタ
 * @param[in] stream   : 使用する stream_config の番号
 * @param[in] p_i2c_config : 対象とする I2C_Config 構造体へのポインタ
 * @param[in] register_address : 書き込むレジスタのアドレス
 * @param[in] data_v   : 書き込むデータを格納している場所のポインタ
 * @param[in] data_len : 書き込むデータの長さ．1 or 2 [byte] のみ指定可
 * @retval FRM_ERR_CODE (FRM_send_general_cmd の返り値)
 */
static FRM_ERR_CODE I2C_write_bytes_(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config,
                                    uint8_t register_address, void* data_v, uint8_t data_len);

/**
 * @brief I2C_Config 構造体にて指定されたデバイスのレジスタへ読み込む
 * @param[in] p_framing  : Framing 構造体へのポインタ
 * @param[in] stream   : 使用する stream_config の番号
 * @param[in] p_i2c_config : 対象とする I2C_Config 構造体へのポインタ
 * @param[in] register_address : 読み込むレジスタのアドレス
 * @param[out] data_v  : データ格納先へのポインタ
 * @param[in] buffer_size : 読み込むデータの長さ．
 * @retval FRM_ERR_CODE
 */
static FRM_ERR_CODE I2C_read_bytes_(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config,
                                   uint8_t register_address, void* data_v, uint8_t buffer_size);

FRM_ERR_CODE I2C_write_byte(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config, uint8_t register_address, uint8_t data)
{
  return I2C_write_bytes_(p_framing, stream, p_i2c_config, register_address, &data, sizeof(data));
}

FRM_ERR_CODE I2C_write_2bytes(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config, uint8_t register_address, uint16_t data)
{
  return I2C_write_bytes_(p_framing, stream, p_i2c_config, register_address, &data, sizeof(data));
}

static FRM_ERR_CODE I2C_write_bytes_(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config,
                                    uint8_t register_address, void* data_v, uint8_t data_len)
{
  uint8_t tx_data[sizeof(register_address) + sizeof(uint16_t)];
  FRM_StreamConfig* stream_config = &(p_framing->stream_config[stream]);

  tx_data[0] = register_address;
  ENDIAN_memcpy(tx_data + sizeof(register_address), data_v, data_len);
  I2C_set_stop_flag(p_i2c_config, 1);
  FRMSC_set_tx_frame(stream_config, tx_data);
  FRMSC_set_tx_frame_size(stream_config, sizeof(register_address) + data_len);

  return FRM_send_general_cmd(p_framing, stream);
}

FRM_ERR_CODE I2C_read_byte(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config,
                          uint8_t register_address, uint8_t* data)
{
  return I2C_read_bytes_(p_framing, stream, p_i2c_config, register_address, data, sizeof(*data));
}

FRM_ERR_CODE I2C_read_2bytes(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config,
                            uint8_t register_address, uint16_t* data)
{
  return I2C_read_bytes_(p_framing, stream, p_i2c_config, register_address, data, sizeof(*data));
}

static FRM_ERR_CODE I2C_read_bytes_(Framing* p_framing, uint8_t stream, I2C_Config* p_i2c_config,
                                   uint8_t register_address, void* data_v, uint8_t buffer_size)
{
  FRM_ERR_CODE ret = FRM_ERR_CODE_OK;
  const uint8_t* rx_data;
  FRM_StreamConfig* stream_config = &(p_framing->stream_config[stream]);

  // read setting
  FRMSC_set_rx_frame_size(stream_config, buffer_size);
  I2C_set_rx_length(p_i2c_config, buffer_size);
  FRM_clear_rx_buffer(p_framing);
  // send
  I2C_set_stop_flag(p_i2c_config, 0);
  FRMSC_set_tx_frame(stream_config, &register_address);
  FRMSC_set_tx_frame_size(stream_config, sizeof(register_address));
  ret = FRM_send_req_tlm_cmd(p_framing, stream);
  if (ret != FRM_ERR_CODE_OK) return ret;
  // read
  I2C_set_stop_flag(p_i2c_config, 1);
  ret = FRM_receive(p_framing);
  if (ret != FRM_ERR_CODE_OK) return ret;
  if (FRMSC_get_rec_status(stream_config)->status_code != FRM_STREAM_REC_STATUS_FIXED_FRAME)
  {
    return FRM_ERR_CODE_ERR;
  }

  rx_data = FRMSC_get_rx_frame(stream_config);
  ENDIAN_memcpy(data_v, rx_data, buffer_size);

  return ret;
}

#pragma section
