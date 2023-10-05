/**
 * @file
 * @brief コンポ間通信などで標準的に使う ネットワーク層 の Common Tlm Cmd Packet (概ね Space Packet)
 * @note  ここでは，データリンク層は CCSDS ではなく EB90 Frame を使うことを想定 (TODO: 今後拡張予定)
 * @note  データリンク層は FRM_StreamConfig.data_link_layer_ で規定する
 * @note  packet 構造 などは tlm_cmd/common_tlm_cmd_packet.h を参照のこと
 */
#ifndef FRAMING_COMMON_TLM_CMD_PACKET_H_
#define FRAMING_COMMON_TLM_CMD_PACKET_H_

#include <stdint.h>
#include "framing.h"
#include "../../tlm_cmd/common_tlm_cmd_packet.h"

/**
 * @brief  CommonTlmCmdPacket を受信データからコピーして取得する
 * @note   フレームが確定しているときに呼び出すこと
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[out] received_packet: 抽出したパケット
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: パケット長が異常など，エラーでコピーできない
 */
FRM_ERR_CODE CTCP_get_ctcp_from_dssc(const FRM_StreamConfig* p_stream_config, CommonTlmCmdPacket* received_packet);

/**
 * @brief  CommonTlmPacket を受信データからコピーして取得する
 * @note   フレームが確定しているときに呼び出すこと
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[out] received_packet: 抽出したパケット
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: パケット長が異常など，エラーでコピーできない
 */
FRM_ERR_CODE CTP_get_ctp_from_dssc(const FRM_StreamConfig* p_stream_config, CommonTlmPacket* received_packet);

/**
 * @brief  CommonCmdPacket を受信データからコピーして取得する
 * @note   フレームが確定しているときに呼び出すこと
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[out] received_packet: 抽出したパケット
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: パケット長が異常など，エラーでコピーできない
 */
FRM_ERR_CODE CCP_get_ccp_from_dssc(const FRM_StreamConfig* p_stream_config, CommonCmdPacket* received_packet);

/**
 * @brief  C2A 間通信など， CTCP をコンポ間通信に用いるとき，FRM_init で渡す初期化関数内部用の Init Util
 *
 *         これを呼び出すと，まるっと DSSC の初期設定ができる．
 * @note   DSSC_enable は Driver 側でやること
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[in]  tx_frame_buffer: コマンドフレーム（送信フレーム）のバッファ
 * @param[in]  tx_frame_buffer_size: バッファサイズ
 * @param[in]  data_analyzer: DSSC_set_data_analyzer で渡すための data_analyzer
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: フレームバッファのサイズ不足などのエラー
 */
FRM_ERR_CODE CTCP_init_dssc(FRM_StreamConfig* p_stream_config,
                           uint8_t* tx_frame_buffer,
                           int16_t tx_frame_buffer_size,
                           FRM_ERR_CODE (*data_analyzer)(FRM_StreamConfig* p_stream_config, void* p_driver));

/**
 * @brief  C2A 間通信など， CTCP をコンポ間通信に用いるときの tx_frame のセット
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[in]  send_packet: 送信するパケット
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: DSSC 内部の設定不足などのエラー
 */
FRM_ERR_CODE CTCP_set_tx_frame_to_dssc(FRM_StreamConfig* p_stream_config,
                                      const CommonTlmCmdPacket* send_packet);

/**
 * @brief  C2A 間通信など， CTP をコンポ間通信に用いるときの tx_frame のセット
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[in]  send_packet: 送信するパケット
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: DSSC 内部の設定不足などのエラー
 */
FRM_ERR_CODE CTP_set_tx_frame_to_dssc(FRM_StreamConfig* p_stream_config,
                                     const CommonTlmPacket* send_packet);

/**
 * @brief  C2A 間通信など， CCP をコンポ間通信に用いるときの tx_frame のセット
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[in]  send_packet: 送信するパケット
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: DSSC 内部の設定不足などのエラー
 */
FRM_ERR_CODE CCP_set_tx_frame_to_dssc(FRM_StreamConfig* p_stream_config,
                                     const CommonCmdPacket* send_packet);

#endif
