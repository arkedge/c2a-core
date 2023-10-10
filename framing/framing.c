#pragma section REPRO
/**
 * @file
 * @brief  各制御センサ・アクチュエータ等とのインターフェースドライバ群のスーパークラス
 *
 *         Framing Class は
 *         各制御センサ・アクチュエータ等とのインターフェースを実現し，
 *         初期化，コマンド発行，テレメトリリクエスト，テレメトリ受信，テレメトリ解析などを行う，ドライバ群のスーパークラスです．
 *         個々の機器のインターフェースドライバに継承させて使用します．
 */

#include "framing.h"
#include "../library/print.h"
#include <string.h>     // for memset などの mem 系
#include <stddef.h>     // for NULL
#include "../tlm_cmd/common_cmd_packet_util.h"

// #define FRM_DEBUG                       // 適切なときにコメントアウトする
// #define FRM_DEBUG_SHOW_REC_DATA         // 適切なときにコメントアウトする

static uint8_t FRM_if_rx_buffer_[FRM_IF_RX_BUFFER_SIZE];    //!< IF_RX で受信するときの一次バッファ

/**
 * @brief  コマンド送信処理
 *
 *         FRM_send_general_cmd と FRM_send_req_tlm_cmdの共通部分
 * @param  p_framing: Framing 構造体へのポインタ
 * @param  stream:  どの config を使用するか．stream は 0-MAX なので，継承先で ENUM など宣言して使いやすくすればいいと思う．
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: IF_TX でのエラーあり
 * @note   受信状況やエラー情報は send_status_ に格納されている
 */
static FRM_ERR_CODE FRM_send_cmd_(Framing* p_framing, uint8_t stream);

/**
 * @brief  継承先の機器にコマンドを発行する
 * @note   この関数の実行前に，tx_frame_, tx_frame_size_の設定が必要である
 * @param  p_framing: Framing 構造体へのポインタ
 * @param  stream:  どの config を使用するか．stream は 0-MAX なので，継承先で ENUM など宣言して使いやすくすればいいと思う．
 * @retval FRM_ERR_CODE_OK (0): 正常終了
 * @retval 0 以外: IF_TX の戻り値
 */
static int FRM_tx_(Framing* p_framing, uint8_t stream);

/**
 * @brief  継承先の機器からの受信データがあるか確認し，受信する
 * @param  p_framing: Framing 構造体へのポインタ
 * @retval 0:    受信データなし
 * @retval 正数: 受信データ長 [Byte]
 * @retval 負数: IF_RXのエラー
 */
static int FRM_rx_(Framing* p_framing);

/**
 * @brief  受信フレーム解析関数
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[in]  rec_data_len:    今回新規に受信したデータ長
 * @return void 詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_(FRM_StreamConfig* p_stream_config,
                                   uint16_t rec_data_len);

/**
 * @brief  解析用受信バッファの準備
 *
 *         繰り越されたデータと今回受信したデータの結合を行い，受信データ解析の準備をする
 * @param[in]  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param[in]  rec_data_len:    今回新規に受信したデータ長
 * @return void
 * @note   厳格なフレーム探索が有効かどうかで処理が変わることに注意
 */
static void FRM_analyze_rx_buffer_prepare_buffer_(FRM_StreamConfig* p_stream_config,
                                                  uint16_t rec_data_len);

/**
 * @brief  フレーム解析関数
 *
 *         受信バッファを走査し，バイト単位でフレーム内データを確認していく
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_pickup_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  フレーム解析関数後の繰越データの頭出し
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void
 */
static void FRM_analyze_rx_buffer_cueing_buffer_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  固定長フレーム解析関数（バイト列処理）
 *
 *         受信バッファのデータを走査し，必要なデータをフレームとして pickup する
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_fixed_pickup_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  可変フレーム解析関数（バイト列処理）
 *
 *         受信バッファのデータを走査し，必要なデータをフレームとして pickup する
 * @note   受信フレームにフレーム長データが存在していることを前提とする
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_variable_pickup_with_rx_frame_size_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  可変フレーム解析関数（バイト列処理）
 *
 *         受信バッファのデータを走査し，必要なデータをフレームとして pickup する
 * @note   FRM_analyze_rx_buffer_variable_pickup_with_rx_frame_size_ との違いは，テレメ長データがフレームに含まれるか否か
 * @note   フッタが存在していることを前提とする
 * @note   ヘッダなしは認める．ただし，受信データ先頭からフレームとみなすので，ヘッダありを強く推奨する
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_variable_pickup_with_footer_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  フレーム解析関数（ヘッダ探索）
 * @note   ヘッダが見つかった場合，最初の 1 byte のみ処理する
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_finding_header_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  フレーム解析関数（ヘッダ受信中）
 * @note   1 byte のみ処理する
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_receiving_header_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  フレーム解析関数（フッタ受信中）
 * @note   1 byte のみ処理する
 * @note   現在，フレーム長が uint16_t を超えることは想定していない！
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @param  rx_frame_size:   フレームサイズ（可変長フレームの場合もあるので，引数に取る）
 * @return void その他の詳細は FRM_StreamRecStatus
 */
static void FRM_analyze_rx_buffer_receiving_footer_(FRM_StreamConfig* p_stream_config,
                                                    uint16_t rx_frame_size);

/**
 * @brief  フレーム解析中に受信したフレームからフレーム長を取得する関数
 * @note   FRM_analyze_rx_buffer_variable_pickup_with_rx_frame_size_ から呼ばれることを想定
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return フレーム長
 */
static uint32_t FRM_analyze_rx_buffer_get_framelength_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  FRM_StreamConfig 構造体の初期化
 *
 *         FRM_StreamConfig 構造体を初期化し，デフォルト値で埋める．
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return FRM_ERR_CODE
 */
static FRM_ERR_CODE FRM_reset_stream_config_(FRM_StreamConfig* p_stream_config);

/**
 * @brief  FRM_StreamConfig 構造体のバリデーション
 * @param  p_framing:         Framing 構造体へのポインタ
 * @param  p_stream_config: Framing 構造体の FRM_StreamConfig
 * @return FRM_ERR_CODE
 */
static FRM_ERR_CODE FRM_validate_stream_config_(const Framing* p_framing, FRM_StreamConfig* p_stream_config);

// ダミー関数
// EQU だと関数ポインタの初期値を NULL にしていたためにぬるぽで事故ったので
static FRM_ERR_CODE FRM_load_init_setting_dummy_(Framing* p_framing);
static FRM_ERR_CODE FRM_data_analyzer_dummy_(FRM_StreamConfig* p_stream_config, void* p_driver);

// ###### FRM_StreamRecBuffer 関連関数 ######

/**
 * @brief FRM_StreamRecBuffer のクリア
 * @param[in,out] stream_rec_buffer: クリアする FRM_StreamRecBuffer
 * @return void
 */
void FRM_clear_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer);

/**
 * @brief FRM_StreamRecBuffer の前方要素の削除
 * @note stream_rec_buffer は NULL でないことを仮定する
 * @param[in,out] stream_rec_buffer: 削除する FRM_StreamRecBuffer
 * @param[in]     size:              削除するバッファサイズ
 * @return void
 */
void FRM_drop_from_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                      uint16_t size);

/**
 * @brief FRM_StreamRecBuffer の追記（後ろへの）
 * @note stream_rec_buffer は NULL でないことを仮定する
 * @param[in,out] stream_rec_buffer: 追記する FRM_StreamRecBuffer
 * @param[in]     buffer:            追記するバッファ
 * @param[in]     size:              追記するバッファサイズ
 * @retval FRM_ERR_CODE_OK:  正常終了
 * @retval FRM_ERR_CODE_ERR: サイズ不足でコピーできず
 */
FRM_ERR_CODE FRM_push_to_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                            const uint8_t* buffer,
                                            uint16_t size);

/**
 * @brief FRM_StreamRecBuffer の未処理バッファサイズの取得
 * @note stream_rec_buffer は NULL でないことを仮定する
 * @param[in] stream_rec_buffer: FRM_StreamRecBuffer
 * @return 未処理バッファサイズ
 */
uint16_t FRM_get_unprocessed_size_from_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer);

/**
 * @brief FRM_StreamRecBuffer で指定サイズだけバッファを確定させる
 * @note stream_rec_buffer は NULL でないことを仮定する
 * @param[in,out] stream_rec_buffer: 確定させる FRM_StreamRecBuffer
 * @param[in]     size:              確定させるバッファサイズ
 * @return void
 */
void FRM_confirm_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                    uint16_t size);

/**
 * @brief FRM_StreamRecBuffer で指定サイズだけフレーム開始点を前進させる
 * @note stream_rec_buffer は NULL でないことを仮定する
 * @param[in,out] stream_rec_buffer: 前進させる FRM_StreamRecBuffer
 * @param[in]     size:              前進させるバッファサイズ
 * @return void
 */
void FRM_move_forward_frame_head_candidate_of_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                                                 uint16_t size);


// ###### Framing基本関数 ######

FRM_ERR_CODE FRM_init(Framing* p_framing,
                      void* if_config,
                      FRM_StreamRecBuffer* rx_buffer,
                      FRM_ERR_CODE (*load_init_setting)(Framing* p_framing))
{
  FRM_StreamRecBuffer* rx_buffers[FRM_STREAM_MAX];
  FRM_nullify_stream_rec_buffers(rx_buffers);
  rx_buffers[0] = rx_buffer;
  return FRM_init_streams(p_framing, if_config, rx_buffers, load_init_setting);
}


FRM_ERR_CODE FRM_init_streams(Framing* p_framing,
                              void* if_config,
                              FRM_StreamRecBuffer* rx_buffers[FRM_STREAM_MAX],
                              FRM_ERR_CODE (*load_init_setting)(Framing* p_framing))
{
  uint8_t stream;

  if (FRM_reset(p_framing) != FRM_ERR_CODE_OK) return FRM_ERR_CODE_ERR;

  p_framing->if_config = if_config;

  // load_init_setting で上書きできるようにここで設定
  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    if (rx_buffers[stream] != NULL)
    {
      FRMSC_set_rx_buffer(&(p_framing->stream_config[stream]), rx_buffers[stream]);
    }
  }

  p_framing->config.internal.load_init_setting = load_init_setting;
  if (p_framing->config.internal.load_init_setting(p_framing) != FRM_ERR_CODE_OK) return FRM_ERR_CODE_ERR;

  if (FRM_validate_config(p_framing) != FRM_ERR_CODE_OK) return FRM_ERR_CODE_ERR;

  // IF の初期化
  // 一旦シンプルに IF_init のエラーコードは無視する（実機でここでエラー出る場合はコードがおかしいので．必要があれば将来実装．）
  if ( (*IF_init[p_framing->interface])(p_framing->if_config) != 0 ) return FRM_ERR_CODE_ERR;

  return FRM_ERR_CODE_OK;
}


FRM_ERR_CODE FRM_reset(Framing* p_framing)
{
  uint8_t stream;

  p_framing->interface = IF_LIST_MAX; // FIXME: (*IF_init[p_framing->interface])(p_framing->if_config) の様な使い方をするのでセグフォが起こる可能性があり
  p_framing->if_config = NULL;        // FIXME: NULL ポインタはこの関数が Reset 単体で使われるとマズい

  p_framing->config.settings.rx_buffer_size_in_if_rx_          = FRM_IF_RX_BUFFER_SIZE;
  p_framing->config.settings.should_monitor_for_rx_disruption_ = 0;
  p_framing->config.settings.time_threshold_for_rx_disruption_ = 60 * 1000;      // この値はよく考えること

  p_framing->config.info.rec_status_.ret_from_if_rx       = 0;
  p_framing->config.info.rec_status_.rx_disruption_status = FRM_RX_DISRUPTION_STATUS_OK;

  p_framing->config.info.rx_count_      = 0;
  p_framing->config.info.rx_call_count_ = 0;
  p_framing->config.info.rx_time_       = TMGR_get_master_clock();

  p_framing->config.internal.load_init_setting = FRM_load_init_setting_dummy_;

  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    FRM_ERR_CODE ret = FRM_reset_stream_config_(&p_framing->stream_config[stream]);
    if (ret != FRM_ERR_CODE_OK) return ret;
  }

  FRM_clear_rx_buffer(p_framing);

  return FRM_ERR_CODE_OK;
}


FRM_ERR_CODE FRM_validate_config(Framing* p_framing)
{
  uint8_t stream;

  if (p_framing->interface < 0 || p_framing->interface >= IF_LIST_MAX) return FRM_ERR_CODE_ERR;
  if (p_framing->if_config == NULL) return FRM_ERR_CODE_ERR;

  if (p_framing->config.settings.rx_buffer_size_in_if_rx_ > FRM_IF_RX_BUFFER_SIZE) return FRM_ERR_CODE_ERR;

  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    FRM_ERR_CODE ret = FRM_validate_stream_config_(p_framing, &p_framing->stream_config[stream]);
    if (ret != FRM_ERR_CODE_OK) return ret;
  }

  return FRM_ERR_CODE_OK;
}

FRM_ERR_CODE FRM_clear_rx_buffer(Framing* p_framing)
{
  uint8_t stream;

  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    FRM_clear_stream_rec_buffer_(p_framing->stream_config[stream].settings.rx_buffer_);
  }

  // FIXME: すべての Driver の初期化で呼ばれ，無駄
  memset(FRM_if_rx_buffer_,
         0x00,
         sizeof(FRM_if_rx_buffer_));

  return FRM_ERR_CODE_OK;
}


FRM_ERR_CODE FRM_receive(Framing* p_framing)
{
  uint8_t  stream;
  uint16_t rec_data_len;
  int      ret_rx;

  p_framing->config.info.rx_call_count_++;

  // 各 Driver で物理的に接続されている wire は１本なので，それをここで受信する．
  // 後段の stream では，その受信したビット列に対して，複数のフレーム種類に対して，フレーム探索，確定処理を走らす．
  ret_rx = FRM_rx_(p_framing);
  p_framing->config.info.rec_status_.ret_from_if_rx = ret_rx;

  if (ret_rx > 0)
  {
    // なにかしらの受信データあり
    p_framing->config.info.rx_count_++;
    p_framing->config.info.rx_time_ = TMGR_get_master_clock();
  }

  // 受信途絶判定
  // テレメなどで見るときにノイズになるので，判定しないときは OK にしておく
  p_framing->config.info.rec_status_.rx_disruption_status = FRM_RX_DISRUPTION_STATUS_OK;
  if (p_framing->config.settings.should_monitor_for_rx_disruption_)
  {
    ObcTime now = TMGR_get_master_clock();
    uint32_t last_rx_ago = OBCT_diff_in_msec(&p_framing->config.info.rx_time_, &now);

    if (last_rx_ago > p_framing->config.settings.time_threshold_for_rx_disruption_)
    {
      p_framing->config.info.rec_status_.rx_disruption_status = FRM_RX_DISRUPTION_STATUS_LOST;
    }
  }

  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);
    if (!p_stream_config->settings.is_enabled_)
    {
      p_stream_config->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_DISABLE;
      continue;
    }

    // setter で validation かけると，初期化などで何度もかかることや，
    // そもそもこの validation は打ち上げ時というよりむしろ地上試験時に有用なので，ここに置く
    if (p_stream_config->internal.is_validation_needed_for_rec_)
    {
      FRM_ERR_CODE ret = FRM_validate_stream_config_(p_framing, p_stream_config);
      if (ret != FRM_ERR_CODE_OK)
      {
        p_stream_config->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_VALIDATE_ERR;
        continue;
      }
    }

    if (ret_rx < 0)     // この共通処理が for の中にあるの，若干キモい．．．
    {
      // RX失敗
      p_stream_config->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RX_ERR;
      // [TODO] ここに Framing としての共通のアノマリ発行を入れるかは要議論
      //        個別対応なので， Driver に実装する，ないしは， IF 側に実装するのが自然か？
      continue;
    }
    else if (ret_rx == 0)
    {
      // 受信データなし

      // 今回の受信はなくとも，バッファ内部に未処理データがあれば処理する
      if (FRM_get_unprocessed_size_from_stream_rec_buffer_(p_stream_config->settings.rx_buffer_) > 0)
      {
        // 繰越があるので，ここで continue せずへ次へ
      }
      else
      {
        // rec_status_.status_code 基本は更新せず
        // FIXED の場合はリセット
        if (p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_FIXED_FRAME)
        {
          p_stream_config->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FINDING_HEADER;
        }
        continue;
      }
    }
    else
    {
      // 何もしない
    }

    // ここまできたら受信データあり

    rec_data_len = (uint16_t)ret_rx;      // ここまでくれば非負数
    // 受信データ処理
    FRM_analyze_rx_buffer_(p_stream_config, rec_data_len);

    // フレーム確定処理
    if (p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_FIXED_FRAME)
    {
      p_stream_config->info.rx_frame_fix_count_++;
      p_stream_config->info.rx_frame_fix_time_ = TMGR_get_master_clock();
      p_stream_config->info.req_tlm_cmd_tx_count_after_last_tx_ = 0;
    }
  }

  // streamのテレメ途絶判定（テレメフレーム確定途絶判定）
  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);
    ObcTime now;
    uint32_t last_tlm_fix_ago;

    // テレメなどで見るときにノイズになるので，判定しないときは OK にしておく
    p_stream_config->info.rec_status_.tlm_disruption_status = FRM_STREAM_TLM_DISRUPTION_STATUS_OK;

    if (!p_stream_config->settings.is_enabled_ || !p_stream_config->settings.should_monitor_for_tlm_disruption_)
    {
      continue;
    }

    now = TMGR_get_master_clock();
    last_tlm_fix_ago = OBCT_diff_in_msec(&p_stream_config->info.rx_frame_fix_time_, &now);

    if (last_tlm_fix_ago > p_stream_config->settings.time_threshold_for_tlm_disruption_)
    {
      p_stream_config->info.rec_status_.tlm_disruption_status = FRM_STREAM_TLM_DISRUPTION_STATUS_LOST;
    }
  }

  return (ret_rx < 0) ? FRM_ERR_CODE_ERR : FRM_ERR_CODE_OK;
}


FRM_ERR_CODE FRM_analyze_rec_data(Framing* p_framing, uint8_t stream, void* p_driver)
{
  FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);

  p_stream_config->info.ret_from_data_analyzer_ = p_stream_config->settings.data_analyzer_(p_stream_config, p_driver);

  return p_stream_config->info.ret_from_data_analyzer_;
}


FRM_ERR_CODE FRM_send_general_cmd(Framing* p_framing, uint8_t stream)
{
  FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);

  if (!p_stream_config->settings.is_enabled_)
  {
    p_stream_config->info.send_status_.status_code = FRM_STREAM_SEND_STATUS_DISABLE;
    return FRM_ERR_CODE_OK;
  }

  p_stream_config->info.general_cmd_tx_count_++;
  p_stream_config->info.general_cmd_tx_time_ = TMGR_get_master_clock();

#ifdef FRM_DEBUG
  Printf("DS: send_general_cmd\n");
#endif

  return FRM_send_cmd_(p_framing, stream);
}


FRM_ERR_CODE FRM_send_req_tlm_cmd(Framing* p_framing, uint8_t stream)
{
  FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);

  if (!p_stream_config->settings.is_enabled_)
  {
    p_stream_config->info.send_status_.status_code = FRM_STREAM_SEND_STATUS_DISABLE;
    return FRM_ERR_CODE_OK;
  }

  p_stream_config->info.req_tlm_cmd_tx_count_++;
  p_stream_config->info.req_tlm_cmd_tx_count_after_last_tx_++;
  p_stream_config->info.req_tlm_cmd_tx_time_ = TMGR_get_master_clock();

#ifdef FRM_DEBUG
  Printf("DS: send_req_tlm_cmd\n");
#endif

  return FRM_send_cmd_(p_framing, stream);
}


static FRM_ERR_CODE FRM_send_cmd_(Framing* p_framing, uint8_t stream)
{
  FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);

  // setter で validation かけると，初期化などで何度もかかることや，
  // そもそもこの validation は打ち上げじというよりむしろ地上試験時に有用なので，ここに置く
  if (p_stream_config->internal.is_validation_needed_for_send_)
  {
    FRM_ERR_CODE ret = FRM_validate_stream_config_(p_framing, p_stream_config);
    if (ret != FRM_ERR_CODE_OK)
    {
      p_stream_config->info.send_status_.status_code = FRM_STREAM_SEND_STATUS_VALIDATE_ERR;
      return FRM_ERR_CODE_ERR;
    }
  }

  p_stream_config->info.send_status_.ret_from_if_tx = FRM_tx_(p_framing, stream);

  if (p_stream_config->info.send_status_.ret_from_if_tx != 0)
  {
    p_stream_config->info.send_status_.status_code = FRM_STREAM_SEND_STATUS_TX_ERR;
    return FRM_ERR_CODE_ERR;
  }

  p_stream_config->info.send_status_.status_code = FRM_STREAM_SEND_STATUS_OK;
  return FRM_ERR_CODE_OK;
}


static int FRM_tx_(Framing* p_framing, uint8_t stream)
{
  int ret;
  FRM_StreamConfig* p_stream_config = &(p_framing->stream_config[stream]);

  if (p_stream_config->settings.tx_frame_size_ == 0) return FRM_ERR_CODE_OK;
  if (p_stream_config->settings.tx_frame_ == NULL) return FRM_ERR_CODE_OK;

#ifdef FRM_DEBUG
  Printf("DS: tx_\n");
#endif

  ret = (*IF_TX[p_framing->interface])(p_framing->if_config,
                                       p_stream_config->settings.tx_frame_,
                                       (int)p_stream_config->settings.tx_frame_size_);

  if (ret != 0) return ret;
  return FRM_ERR_CODE_OK;
}


static int FRM_rx_(Framing* p_framing)
{
  int flag;
  int rec_data_len;
  uint8_t stream;
#ifdef FRM_DEBUG_SHOW_REC_DATA
  int16_t i;
#endif

  // 少なくとも 1 つの stream が有効でかつ，rx_frame_size_ が 0 以外でないと受信処理はしない
  flag = 0;
  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    if (! (p_framing->stream_config[stream].settings.is_enabled_) ) continue;
    if (p_framing->stream_config[stream].settings.rx_frame_size_ != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0) return 0;

  rec_data_len = (*IF_RX[p_framing->interface])(p_framing->if_config,
                                                FRM_if_rx_buffer_,
                                                p_framing->config.settings.rx_buffer_size_in_if_rx_);

#ifdef FRM_DEBUG
  Printf("DS: rx_\n");
#endif

  if (rec_data_len <= 0) return rec_data_len;     // 負数はエラーコード

#ifdef FRM_DEBUG_SHOW_REC_DATA
  Printf("DS: Receive data size is %d bytes, as follow:\n", rec_data_len);
  for (i = 0; i < rec_data_len; i++)
  {
    Printf("%02x ", FRM_if_rx_buffer_[i]);
    if (i % 4 == 3) Printf("   ");
  }
  Printf("\n");
#endif

  return rec_data_len;
}


static void FRM_analyze_rx_buffer_(FRM_StreamConfig* p_stream_config,
                                   uint16_t rec_data_len)
{
  FRM_analyze_rx_buffer_prepare_buffer_(p_stream_config, rec_data_len);

  FRM_analyze_rx_buffer_pickup_(p_stream_config);

  // 次回のデータ受信に備えて，バッファの頭出し
  FRM_analyze_rx_buffer_cueing_buffer_(p_stream_config);
}


static void FRM_analyze_rx_buffer_prepare_buffer_(FRM_StreamConfig* p_stream_config,
                                                  uint16_t rec_data_len)
{
  // rx_buffer_ には，前回確定したフレームも残っているので，それは除く
  // したがって， DS の FRMSC_get_rx_frame した frame へのポインタは，次回受信時までしか有効ではない
  FRM_StreamRecBuffer* buffer = p_stream_config->settings.rx_buffer_;
  FRM_ERR_CODE ret;

  if (buffer->is_frame_fixed)
  {
    // ユーザー側ですでにデータを取得したと判断して，バッファから下ろす
    FRM_drop_from_stream_rec_buffer_(buffer, buffer->pos_of_frame_head_candidate);   // 念のため

    if (p_stream_config->settings.is_strict_frame_search_)
    {
      // 厳格なフレーム探索なので，1 つの可能性も受信漏らさないように
      FRM_drop_from_stream_rec_buffer_(buffer, 1);
    }
    else
    {
      FRM_drop_from_stream_rec_buffer_(buffer, buffer->confirmed_frame_len);
    }

    buffer->is_frame_fixed = 0;
  }

  // 新規受信分をバッファへ
  buffer->pos_of_last_rec = buffer->size;       // 最終更新地点を更新
  ret = FRM_push_to_stream_rec_buffer_(buffer, FRM_if_rx_buffer_, rec_data_len);
  if (ret != FRM_ERR_CODE_OK)
  {
    // バッファがあふれるので，一旦古いバッファのクリア
    // TODO: この仕様は検討してもよいかも？（ユーザー選択にするとか）
    FRM_clear_stream_rec_buffer_(buffer);
    FRM_push_to_stream_rec_buffer_(buffer, FRM_if_rx_buffer_, rec_data_len);
    p_stream_config->info.rec_status_.count_of_carry_over_failures++;
  }
}


static void FRM_analyze_rx_buffer_pickup_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamRecBuffer* buffer = p_stream_config->settings.rx_buffer_;
  // 受信バッファのデータを走査し，必要なデータをフレームとして pickup する関数
  void (*pickup_func)(FRM_StreamConfig* p_stream_config);

  if (p_stream_config->settings.rx_frame_size_ == 0) return;

  if (p_stream_config->settings.rx_frame_size_ > 0)
  {
    pickup_func = FRM_analyze_rx_buffer_fixed_pickup_;
  }
  else if (p_stream_config->settings.rx_frame_size_ < 0)
  {
    // フレームにフレーム長データが含まれているか？
    if (p_stream_config->settings.rx_framelength_pos_ >= 0)
    {
      pickup_func = FRM_analyze_rx_buffer_variable_pickup_with_rx_frame_size_;
    }
    else
    {
      pickup_func = FRM_analyze_rx_buffer_variable_pickup_with_footer_;
    }
  }
  else
  {
    // TODO: ビッグデータ対応
    pickup_func = NULL;
    return;
  }

  // 受信バッファからデータをピックアップしていく
  while (FRM_get_unprocessed_size_from_stream_rec_buffer_(buffer) > 0)
  {
    pickup_func(p_stream_config);

    if (p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_FIXED_FRAME)
    {
      break;
    }

    // 不整合が起きたら，現在の frame 候補の先頭 + 1 バイト目に走査場所を戻す
    if (p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_HEADER_MISMATCH ||
        p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_FOOTER_MISMATCH ||
        p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_RX_FRAME_TOO_LONG ||
        p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT)
    {
      FRM_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, 1);

      // 他の部分での条件分岐のために，詳細エラー情報を現在のステータスに上書きする
      p_stream_config->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FINDING_HEADER;
    }
  }

  if (p_stream_config->info.rec_status_.status_code == FRM_STREAM_REC_STATUS_FIXED_FRAME)
  {
    buffer->is_frame_fixed = 1;
    p_stream_config->info.rec_status_.fixed_frame_len = buffer->confirmed_frame_len;
    // FIXME: is_frame_fixed と FRM_STREAM_REC_STATUS_FIXED_FRAME あたいの混在がだるいので直す？
    // FIXME: confirmed_frame_len と fixed_frame_len も？（これはまあ最後のを残すという意味ではよさそう）
  }

  return;
}


static void FRM_analyze_rx_buffer_cueing_buffer_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;

  // pos_of_frame_head_candidate の頭出しをしておけば OK
  FRM_drop_from_stream_rec_buffer_(buffer, buffer->pos_of_frame_head_candidate);
}


static void FRM_analyze_rx_buffer_fixed_pickup_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;

  if (buffer->confirmed_frame_len == 0 && p->settings.rx_header_size_ != 0)
  {
    // まだヘッダの先頭すら未発見の場合（ヘッダなし時はここはスキップ）
    FRM_analyze_rx_buffer_finding_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_header_size_)
  {
    // ヘッダ受信中
    FRM_analyze_rx_buffer_receiving_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_frame_size_ - p->settings.rx_footer_size_)
  {
    // データ受信中
    // ここは高速化のために一括処理
    // ヘッダなしの場合は，ここがフレーム先頭であることに注意
    const uint16_t unprocessed_data_len = FRM_get_unprocessed_size_from_stream_rec_buffer_(buffer);
    uint16_t pickup_data_len = (uint16_t)(p->settings.rx_frame_size_ - p->settings.rx_footer_size_ - buffer->confirmed_frame_len);

    // 今回で全部受信しきらない場合
    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    FRM_confirm_stream_rec_buffer_(buffer, pickup_data_len);     // pickup_data_len byte 確定
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_DATA;

    // フッタがなく，data 受信仕切った場合はフレーム確定
    // これがないと，FRM_analyze_rx_buffer_fixed_ で
    // 今まさに受信したデータ長がぴったりフレーム末だった場合に，フレーム確定が１周期遅れることになるので
    if (p->settings.rx_footer_size_ == 0 && buffer->confirmed_frame_len == p->settings.rx_frame_size_)
    {
      p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FIXED_FRAME;
    }

    return;
  }
  else
  {
    // フッタ受信中 or フッタなしの場合はフレーム確定
    FRM_analyze_rx_buffer_receiving_footer_(p_stream_config,
                                            (uint16_t)(p->settings.rx_frame_size_));
    return;
  }
}


static void FRM_analyze_rx_buffer_variable_pickup_with_rx_frame_size_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t unprocessed_data_len = FRM_get_unprocessed_size_from_stream_rec_buffer_(buffer);
  uint32_t rx_frame_size = FRM_analyze_rx_buffer_get_framelength_(p_stream_config);      // まだ受信していない場合は不定値が入ることに留意すること！！

  if (buffer->confirmed_frame_len == 0 && p->settings.rx_header_size_ != 0)
  {
    // まだヘッダの先頭すら未発見の場合（ヘッダなし時はここはスキップ）
    FRM_analyze_rx_buffer_finding_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_header_size_)
  {
    // ヘッダ受信中
    FRM_analyze_rx_buffer_receiving_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_framelength_pos_ + p->settings.rx_framelength_type_size_)
  {
    // フレームサイズ探索中
    // ここは高速化のために一括処理
    // ヘッダなしの場合は，ここがフレーム先頭であることに注意
    uint16_t pickup_data_len = (uint16_t)(p->settings.rx_framelength_pos_ + p->settings.rx_framelength_type_size_ - buffer->confirmed_frame_len);

    // 今回で全部受信しきらない場合
    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    FRM_confirm_stream_rec_buffer_(buffer, pickup_data_len);     // pickup_data_len byte 確定
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_FRAMELENGTH;

    // フレーム長を受信し終わった場合，チェックする
    if (buffer->confirmed_frame_len >= p->settings.rx_framelength_pos_ + p->settings.rx_framelength_type_size_)
    {
      rx_frame_size = FRM_analyze_rx_buffer_get_framelength_(p_stream_config);

      // バッファー超え or 上限値超えはエラーを出す！
      if (rx_frame_size > buffer->capacity || rx_frame_size > p->settings.max_rx_frame_size_)
      {
        p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RX_FRAME_TOO_LONG;
#ifdef FRM_DEBUG
        Printf("DS: RX frame size is too long\n");
#endif
        return;
      }

      // body サイズがは 0 以上を要請
      if (rx_frame_size < p->settings.rx_header_size_ + p->settings.rx_footer_size_)
      {
        p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT;
#ifdef FRM_DEBUG
        Printf("DS: RX frame size is too short\n");
#endif
        return;
      }
    }

    return;
  }
  else if (buffer->confirmed_frame_len < rx_frame_size - p->settings.rx_footer_size_)
  {
    // データ受信中
    // ここは高速化のために一括処理
    // TODO: 現在，フレーム長が uint16_t を超えることは想定していない！
    uint16_t pickup_data_len = (uint16_t)(rx_frame_size - p->settings.rx_footer_size_ - buffer->confirmed_frame_len);

    // 今回で全部受信しきらない場合
    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    FRM_confirm_stream_rec_buffer_(buffer, pickup_data_len);     // pickup_data_len byte 確定
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_DATA;

    // フッタがなく，data 受信仕切った場合はフレーム確定
    // これがないと，FRM_analyze_rx_buffer_fixed_ で
    // 今まさに受信したデータ長がぴったりフレーム末だった場合に，フレーム確定が 1 周期遅れることになるので
    if (p->settings.rx_footer_size_ == 0 && buffer->confirmed_frame_len == rx_frame_size)
    {
      p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FIXED_FRAME;
    }

    return;
  }
  else
  {
    // フッタ受信中 or フッタなしの場合はフレーム確定
    FRM_analyze_rx_buffer_receiving_footer_(p_stream_config,
                                            (uint16_t)rx_frame_size);
    return;
  }
}


static void FRM_analyze_rx_buffer_variable_pickup_with_footer_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;

  if (buffer->confirmed_frame_len == 0 && p->settings.rx_header_size_ != 0)
  {
    // まだヘッダの先頭すら未発見の場合（ヘッダなし時はここはスキップ）
    FRM_analyze_rx_buffer_finding_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_header_size_)
  {
    // ヘッダ受信中
    FRM_analyze_rx_buffer_receiving_header_(p_stream_config);
    return;
  }
  else
  {
    // 最後まで受信し，フッタの最終文字を探す．フッタなしはありえない．
    // ヘッダなしの場合は，ここがフレーム先頭
    const uint16_t unprocessed_data_len = FRM_get_unprocessed_size_from_stream_rec_buffer_(buffer);
    uint8_t* p_footer_last;     // inclusive
    int32_t  body_data_len;     // サイズ的には u16 でよいが，負数もとりたいので i32 としている
    uint16_t processed_data_len;
    uint16_t i;
    const uint16_t memchr_offset = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;
    uint16_t estimated_rx_frame_end_pos;

    // 届いているデータを受信フレームバッファに格納する
    // ここは高速化のために一括処理
    // フッタ最終文字を探す
    p_footer_last = (uint8_t*)memchr(&(buffer->buffer[memchr_offset]),
                                     (int)(p->settings.rx_footer_[p->settings.rx_footer_size_ - 1]),
                                     (size_t)unprocessed_data_len);

    if (p_footer_last == NULL)
    {
      // まだフッタ候補まで受信していない → 受信データはすべて今回のフレームとして確定
      processed_data_len = unprocessed_data_len;
    }
    else
    {
      // フッタ候補発見 → フッタ候補までフレーム確定
      processed_data_len = (uint16_t)(p_footer_last - &(buffer->buffer[memchr_offset]) + 1);
    }

    // ↓ buffer->confirmed_frame_len が更新されることに注意！
    FRM_confirm_stream_rec_buffer_(buffer, processed_data_len);     // processed_data_len byte 確定
    if (buffer->confirmed_frame_len > p->settings.max_rx_frame_size_)
    {
      p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RX_FRAME_TOO_LONG;
#ifdef FRM_DEBUG
      Printf("DS: RX frame size is too long\n");
#endif
      return;
    }

    // フッタ候補未発見の場合はここで処理を返す
    if (p_footer_last == NULL)
    {
      p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_DATA;
      return;
    }

    body_data_len = buffer->confirmed_frame_len - p->settings.rx_header_size_ - p->settings.rx_footer_size_;
    if (body_data_len < 0)
    {
      // これはフッタではないので受信続行
      // まだまだ受信する
      p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_DATA;
      return;
    }

    // フッタ候補発見
    // フッタチェックする
    estimated_rx_frame_end_pos = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;
    for (i = 0; i < p->settings.rx_footer_size_; i++)
    {
      if (buffer->buffer[estimated_rx_frame_end_pos - i - 1] != p->settings.rx_footer_[p->settings.rx_footer_size_ - i - 1])
      {
        // これはフッタではないので受信続行
        // まだまだ受信する
        p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_DATA;
        return;
      }
    }

    // フッタ確定 → フレーム確定
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FIXED_FRAME;
    return;
  }
}


static void FRM_analyze_rx_buffer_finding_header_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t unprocessed_data_len = FRM_get_unprocessed_size_from_stream_rec_buffer_(buffer);
  uint8_t* p_header;
  uint16_t found_header_offset;

  // コードが正しければ，ヘッダがないケースはここに到達し得ないが，ヌルポ回避をいれておく
  if (p_stream_config->settings.rx_header_ == NULL)
  {
#ifdef FRM_DEBUG
    Printf("DS: RX header is invalid\n");
#endif
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_OTHER_ERR;
    FRM_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, unprocessed_data_len);
    return;
  }

  // まだヘッダの先頭すら未発見の場合
  // ここは高速化のために一括処理
  p_header = (uint8_t*)memchr(&buffer->buffer[buffer->pos_of_frame_head_candidate],
                              (int)(p->settings.rx_header_[0]),
                              (size_t)unprocessed_data_len);

  if (p_header == NULL)
  {
#ifdef FRM_DEBUG
    Printf("DS: RX header not found\n");
#endif
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FINDING_HEADER;
    FRM_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, unprocessed_data_len);
    return;
  }

  found_header_offset = (uint16_t)(p_header - &buffer->buffer[buffer->pos_of_frame_head_candidate]);
  FRM_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, found_header_offset);
  FRM_confirm_stream_rec_buffer_(buffer, 1);     // ヘッダ 1 byte 目が見つかった
  p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_HEADER;
  return;
}


static void FRM_analyze_rx_buffer_receiving_header_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t buffer_offset = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;

  // ヘッダ受信中
  // 受信が細切れのときなどの処理分岐がめんどくさいので， 1 byte ずつ処理させる

  // ヘッダが正しいか？
  if (buffer->buffer[buffer_offset] == p->settings.rx_header_[buffer->confirmed_frame_len])
  {
    FRM_confirm_stream_rec_buffer_(buffer, 1);     // ヘッダの次の 1 byte がみつかった
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_HEADER;
    return;
  }
  else
  {
    // ヘッダが正しくなかった
    // FRM_STREAM_REC_STATUS_HEADER_MISMATCH になり，再びバッファを巻き戻してヘッダ探索を始める
    // その後 FRM_STREAM_REC_STATUS_FINDING_HEADER に戻る
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_HEADER_MISMATCH;
#ifdef FRM_DEBUG
    Printf("DS: RX header is mismatch\n");
#endif
    return;
  }
}


static void FRM_analyze_rx_buffer_receiving_footer_(FRM_StreamConfig* p_stream_config,
                                                    uint16_t rx_frame_size)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  FRM_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t buffer_offset = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;
  uint16_t rec_footer_pos;

  // フッタなしの場合はフレーム確定
  if (p->settings.rx_footer_size_ == 0)
  {
    // フッタなし
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FIXED_FRAME;
    return;
  }

  // フッタ受信
  // ここも条件分岐がめんどくさいので，1 byte ずつ処理する
  rec_footer_pos = (uint16_t)(buffer->confirmed_frame_len - (rx_frame_size - p->settings.rx_footer_size_));

  // 期待されているフッタが受信できたか？
  // 受信できなかった場合， FRM_STREAM_REC_STATUS_FOOTER_MISMATCH になり，再びバッファを巻き戻してヘッダ探索を始める
  // その後 FRM_STREAM_REC_STATUS_FINDING_HEADER に戻る
  if (buffer->buffer[buffer_offset] != p->settings.rx_footer_[rec_footer_pos])
  {
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FOOTER_MISMATCH;
#ifdef FRM_DEBUG
    Printf("DS: RX footer is mismatch\n");
#endif
    return;
  }

  // ここまできたら正しいフッタが受信されている
  FRM_confirm_stream_rec_buffer_(buffer, 1);     // ヘッダの次の 1 byte がみつかった

  if (buffer->confirmed_frame_len == rx_frame_size)
  {
    // フレーム確定
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_FIXED_FRAME;
  }
  else
  {
    // フレーム確定せず
    p->info.rec_status_.status_code = FRM_STREAM_REC_STATUS_RECEIVING_FOOTER;
  }

  return;
}


static uint32_t FRM_analyze_rx_buffer_get_framelength_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;  // ちょっと変数名が長すぎて配列 index などがみずらいので...
  uint32_t len = 0;
  uint8_t  i;
  const uint16_t pos = p->settings.rx_framelength_pos_ + p->settings.rx_buffer_->pos_of_frame_head_candidate;
  const uint16_t size = p->settings.rx_framelength_type_size_;

  if (p->settings.rx_framelength_endian_ == ENDIAN_TYPE_BIG)
  {
    for (i = 0; i < size; ++i)
    {
      if (i == 0)
      {
        len = p->settings.rx_buffer_->buffer[pos];
      }
      else
      {
        len <<= 8;
        len |= p->settings.rx_buffer_->buffer[pos + i];
      }
    }
  }
  else
  {
    for (i = 0; i < size; ++i)
    {
      if (i == 0)
      {
        len = p->settings.rx_buffer_->buffer[pos + size - 1];
      }
      else
      {
        len <<= 8;
        len |= p->settings.rx_buffer_->buffer[pos + size - 1 - i];
      }
    }
  }

  len += p->settings.rx_framelength_offset_;
  return len;
}


static FRM_ERR_CODE FRM_reset_stream_config_(FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;

  p->settings.is_enabled_ = 0;
  p->settings.is_strict_frame_search_ = 0;

  p->settings.tx_frame_             = NULL;
  p->settings.tx_frame_size_        = 0;
  p->settings.tx_frame_buffer_size_ = -1;

  p->settings.rx_buffer_            = NULL;
  p->settings.rx_header_            = NULL;
  p->settings.rx_header_size_       = 0;
  p->settings.rx_footer_            = NULL;
  p->settings.rx_footer_size_       = 0;
  p->settings.rx_frame_size_        = 0;
  p->settings.max_rx_frame_size_    = 0xffff;

  p->settings.rx_framelength_pos_       = -1;
  p->settings.rx_framelength_type_size_ = 0;
  p->settings.rx_framelength_offset_    = 0;
  p->settings.rx_framelength_endian_    = ENDIAN_TYPE_BIG;

  p->settings.should_monitor_for_tlm_disruption_ = 0;
  p->settings.time_threshold_for_tlm_disruption_ = 60 * 1000;      // この値はよく考えること

  p->settings.data_analyzer_ = FRM_data_analyzer_dummy_;

  // FRM_StreamSendStatus の初期化
  p->info.send_status_.status_code    = FRM_STREAM_SEND_STATUS_DISABLE;
  p->info.send_status_.ret_from_if_tx = 0;

  // FRM_StreamRecStatus の初期化
  p->info.rec_status_.status_code                  = FRM_STREAM_REC_STATUS_DISABLE;
  p->info.rec_status_.fixed_frame_len              = 0;
  p->info.rec_status_.tlm_disruption_status        = FRM_STREAM_TLM_DISRUPTION_STATUS_OK;
  p->info.rec_status_.count_of_carry_over_failures = 0;

  p->info.general_cmd_tx_count_               = 0;
  p->info.req_tlm_cmd_tx_count_               = 0;
  p->info.req_tlm_cmd_tx_count_after_last_tx_ = 0;
  p->info.rx_frame_fix_count_                 = 0;

  p->info.general_cmd_tx_time_ = TMGR_get_master_clock();
  p->info.req_tlm_cmd_tx_time_ = TMGR_get_master_clock();
  p->info.rx_frame_fix_time_   = TMGR_get_master_clock();

  p->info.ret_from_data_analyzer_ = FRM_ERR_CODE_OK;

  p->internal.is_validation_needed_for_send_ = 0;
  p->internal.is_validation_needed_for_rec_  = 0;

  return FRM_ERR_CODE_OK;
}


static FRM_ERR_CODE FRM_validate_stream_config_(const Framing* p_framing, FRM_StreamConfig* p_stream_config)
{
  FRM_StreamConfig* p = p_stream_config;

  if (!p->settings.is_enabled_) return FRM_ERR_CODE_OK;

  if (p->settings.tx_frame_size_  != 0 && p->settings.tx_frame_  == NULL) return FRM_ERR_CODE_ERR;
  if (p->settings.rx_header_size_ != 0 && p->settings.rx_header_ == NULL) return FRM_ERR_CODE_ERR;
  if (p->settings.rx_footer_size_ != 0 && p->settings.rx_footer_ == NULL) return FRM_ERR_CODE_ERR;

  if (p->settings.tx_frame_buffer_size_ >= 0)
  {
    if (p->settings.tx_frame_size_ > p->settings.tx_frame_buffer_size_) return FRM_ERR_CODE_ERR;
  }

  if (p->settings.rx_frame_size_ < 0)
  {
    // テレメトリ可変長
    if (p->settings.rx_framelength_pos_ < 0)
    {
      // フレームサイズデータがない場合
      // フッタの存在が必須
      if (p->settings.rx_footer_size_ == 0) return FRM_ERR_CODE_ERR;
    }
    else
    {
      if (p->settings.rx_header_size_ == 0) return FRM_ERR_CODE_ERR;       // 可変長かつヘッダなしは対応しない（固定長のようにして回避する．詳細はヘッダファイル参照）
      if (p->settings.rx_framelength_pos_ < p->settings.rx_header_size_) return FRM_ERR_CODE_ERR;    // フレームサイズがヘッダ（つまり固定値）に含まれることはありえないので
      if (!(p->settings.rx_framelength_type_size_ == 1 ||
            p->settings.rx_framelength_type_size_ == 2 ||
            p->settings.rx_framelength_type_size_ == 3 ||
            p->settings.rx_framelength_type_size_ == 4 )) return FRM_ERR_CODE_ERR;    // 現在は uint8 to uint32 のみ対応
      if (!(p->settings.rx_framelength_endian_ == ENDIAN_TYPE_BIG ||
            p->settings.rx_framelength_endian_ == ENDIAN_TYPE_LITTLE )) return FRM_ERR_CODE_ERR;
    }
  }
  else if (p->settings.rx_frame_size_ == 0)
  {
    // テレメなし
  }
  else
  {
    // テレメトリ固定長
    if ( p->settings.rx_frame_size_ <
        (p->settings.rx_header_size_ + p->settings.rx_footer_size_) )
    {
      return FRM_ERR_CODE_ERR;
    }
  }

  if (p->settings.is_strict_frame_search_)
  {
    // ヘッダがあることが前提
    if (p->settings.rx_header_size_ == 0) return FRM_ERR_CODE_ERR;
  }

  if (p->settings.rx_buffer_ == NULL) return FRM_ERR_CODE_ERR;
  if (p->settings.rx_buffer_->buffer == NULL) return FRM_ERR_CODE_ERR;
  if (p->settings.rx_buffer_->capacity < p_framing->config.settings.rx_buffer_size_in_if_rx_) return FRM_ERR_CODE_ERR;
  if (p->settings.rx_buffer_->capacity < p->settings.rx_frame_size_) return FRM_ERR_CODE_ERR;
  if (p->settings.rx_buffer_->capacity < p->settings.rx_header_size_ + p->settings.rx_footer_size_) return FRM_ERR_CODE_ERR;

  p->internal.is_validation_needed_for_send_ = 0;
  p->internal.is_validation_needed_for_rec_ = 0;

  return FRM_ERR_CODE_OK;
}


static FRM_ERR_CODE FRM_load_init_setting_dummy_(Framing* p_framing)
{
  (void)p_framing;
  return FRM_ERR_CODE_OK;
}

static FRM_ERR_CODE FRM_data_analyzer_dummy_(FRM_StreamConfig* p_stream_config, void* p_driver)
{
  (void)p_stream_config;
  (void)p_driver;
  return FRM_ERR_CODE_OK;
}


// ###### FRM_Config Getter/Setter of Settings ######
uint16_t DSC_get_rx_buffer_size_in_if_rx(const Framing* p_framing)
{
  return (uint16_t)p_framing->config.settings.rx_buffer_size_in_if_rx_;
}

FRM_ERR_CODE DSC_set_rx_buffer_size_in_if_rx(Framing* p_framing,
                                             const uint16_t rx_buffer_size_in_if_rx)
{
  if (rx_buffer_size_in_if_rx > FRM_IF_RX_BUFFER_SIZE) return FRM_ERR_CODE_ERR;
  p_framing->config.settings.rx_buffer_size_in_if_rx_ = rx_buffer_size_in_if_rx;
  return FRM_ERR_CODE_OK;
}

uint8_t DSC_get_should_monitor_for_rx_disruption(const Framing* p_framing)
{
  return (uint8_t)p_framing->config.settings.should_monitor_for_rx_disruption_;
}

void DSC_enable_monitor_for_rx_disruption(Framing* p_framing)
{
  p_framing->config.settings.should_monitor_for_rx_disruption_ = 1;
}

void DSC_disable_monitor_for_rx_disruption(Framing* p_framing)
{
  p_framing->config.settings.should_monitor_for_rx_disruption_ = 0;
}

uint32_t DSC_get_time_threshold_for_rx_disruption(const Framing* p_framing)
{
  return (uint32_t)p_framing->config.settings.time_threshold_for_rx_disruption_;
}

void DSC_set_time_threshold_for_rx_disruption(Framing* p_framing,
                                              const uint32_t time_threshold_for_rx_disruption)
{
  p_framing->config.settings.time_threshold_for_rx_disruption_ = time_threshold_for_rx_disruption;
}


// ###### FRM_Config Getter/Setter of Info ######
// FIXME: HEW で Warning が出てしまう（gcc ではでない）ので，キャストしている関数がいくつかある
const FRM_RecStatus* DSC_get_rec_status(const Framing* p_framing)
{
  return &p_framing->config.info.rec_status_;
}

uint32_t DSC_get_rx_count(const Framing* p_framing)
{
  return (uint32_t)p_framing->config.info.rx_count_;
}

uint32_t DSC_get_rx_call_count(const Framing* p_framing)
{
  return (uint32_t)p_framing->config.info.rx_call_count_;
}

const ObcTime* DSC_get_rx_time(const Framing* p_framing)
{
  return &p_framing->config.info.rx_time_;
}

FRM_RX_DISRUPTION_STATUS_CODE DSC_get_rx_disruption_status(const Framing* p_framing)
{
  return (FRM_RX_DISRUPTION_STATUS_CODE)p_framing->config.info.rec_status_.rx_disruption_status;
}


// ###### FRM_StreamConfig Getter/Setter of Settings ######
// FIXME: HEW で Warning が出てしまう（gcc ではでない）ので，キャストしている関数がいくつかある
uint8_t FRMSC_get_is_enabled(const FRM_StreamConfig* p_stream_config)
{
  return (uint8_t)p_stream_config->settings.is_enabled_;
}

void FRMSC_enable(FRM_StreamConfig* p_stream_config)
{
  p_stream_config->settings.is_enabled_ = 1;
  p_stream_config->internal.is_validation_needed_for_send_ = 1;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_disable(FRM_StreamConfig* p_stream_config)
{
  p_stream_config->settings.is_enabled_ = 0;
}

uint8_t FRMSC_get_is_strict_frame_search(const FRM_StreamConfig* p_stream_config)
{
  return (uint8_t)p_stream_config->settings.is_strict_frame_search_;
}

void FRMSC_enable_strict_frame_search(FRM_StreamConfig* p_stream_config)
{
  p_stream_config->settings.is_strict_frame_search_ = 1;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_disable_strict_frame_search(FRM_StreamConfig* p_stream_config)
{
  p_stream_config->settings.is_strict_frame_search_ = 0;
}

const uint8_t* FRMSC_get_tx_frame(FRM_StreamConfig* p_stream_config)
{
  return p_stream_config->settings.tx_frame_;
}

uint8_t* FRMSC_get_tx_frame_as_non_const_pointer(FRM_StreamConfig* p_stream_config)
{
  return p_stream_config->settings.tx_frame_;
}

void FRMSC_set_tx_frame(FRM_StreamConfig* p_stream_config,
                        uint8_t* tx_frame)
{
  p_stream_config->settings.tx_frame_ = tx_frame;
  p_stream_config->internal.is_validation_needed_for_send_ = 1;
}

uint16_t FRMSC_get_tx_frame_size(const FRM_StreamConfig* p_stream_config)
{
  return (uint16_t)p_stream_config->settings.tx_frame_size_;
}

void FRMSC_set_tx_frame_size(FRM_StreamConfig* p_stream_config,
                             const uint16_t tx_frame_size)
{
  p_stream_config->settings.tx_frame_size_ = tx_frame_size;
  p_stream_config->internal.is_validation_needed_for_send_ = 1;
}

int16_t FRMSC_get_tx_frame_buffer_size(FRM_StreamConfig* p_stream_config)
{
  return (int16_t)p_stream_config->settings.tx_frame_buffer_size_;
}

void FRMSC_set_tx_frame_buffer_size(FRM_StreamConfig* p_stream_config,
                                    const int16_t tx_frame_buffer_size)
{
  p_stream_config->settings.tx_frame_buffer_size_ = tx_frame_buffer_size;
  p_stream_config->internal.is_validation_needed_for_send_ = 1;
}

void FRMSC_set_rx_buffer(FRM_StreamConfig* p_stream_config,
                         FRM_StreamRecBuffer* rx_buffer)
{
  p_stream_config->settings.rx_buffer_ = rx_buffer;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_set_rx_header(FRM_StreamConfig* p_stream_config,
                         const uint8_t* rx_header,
                         const uint16_t rx_header_size)
{
  p_stream_config->settings.rx_header_ = rx_header;
  p_stream_config->settings.rx_header_size_ = rx_header_size;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

uint16_t FRMSC_get_rx_header_size(const FRM_StreamConfig* p_stream_config)
{
  return (uint16_t)p_stream_config->settings.rx_header_size_;
}

void FRMSC_set_rx_footer(FRM_StreamConfig* p_stream_config,
                         const uint8_t* rx_footer,
                         const uint16_t rx_footer_size)
{
  p_stream_config->settings.rx_footer_ = rx_footer;
  p_stream_config->settings.rx_footer_size_ = rx_footer_size;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

uint16_t FRMSC_get_rx_footer_size(const FRM_StreamConfig* p_stream_config)
{
  return (uint16_t)p_stream_config->settings.rx_footer_size_;
}

int16_t FRMSC_get_rx_frame_size(const FRM_StreamConfig* p_stream_config)
{
  return (int16_t)p_stream_config->settings.rx_frame_size_;
}

void FRMSC_set_rx_frame_size(FRM_StreamConfig* p_stream_config,
                             const int16_t rx_frame_size)
{
  p_stream_config->settings.rx_frame_size_ = rx_frame_size;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

uint16_t FRMSC_get_max_rx_frame_size(const FRM_StreamConfig* p_stream_config)
{
  return (uint16_t)p_stream_config->settings.max_rx_frame_size_;
}

void FRMSC_set_max_rx_frame_size(FRM_StreamConfig* p_stream_config,
                                 const uint16_t max_rx_frame_size)
{
  p_stream_config->settings.max_rx_frame_size_ = max_rx_frame_size;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_set_rx_framelength_pos(FRM_StreamConfig* p_stream_config,
                                  const int16_t rx_framelength_pos)
{
  p_stream_config->settings.rx_framelength_pos_ = rx_framelength_pos;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_set_rx_framelength_type_size(FRM_StreamConfig* p_stream_config,
                                        const uint16_t rx_framelength_type_size)
{
  p_stream_config->settings.rx_framelength_type_size_ = rx_framelength_type_size;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_set_rx_framelength_offset(FRM_StreamConfig* p_stream_config,
                                     const uint16_t rx_framelength_offset)
{
  p_stream_config->settings.rx_framelength_offset_ = rx_framelength_offset;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_set_rx_framelength_endian(FRM_StreamConfig* p_stream_config,
                                     const ENDIAN_TYPE rx_framelength_endian)
{
  p_stream_config->settings.rx_framelength_endian_ = rx_framelength_endian;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

uint8_t FRMSC_get_should_monitor_for_tlm_disruption(const FRM_StreamConfig* p_stream_config)
{
  return (uint8_t)p_stream_config->settings.should_monitor_for_tlm_disruption_;
}

void FRMSC_enable_monitor_for_tlm_disruption(FRM_StreamConfig* p_stream_config)
{
  p_stream_config->settings.should_monitor_for_tlm_disruption_ = 1;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_disable_monitor_for_tlm_disruption(FRM_StreamConfig* p_stream_config)
{
  p_stream_config->settings.should_monitor_for_tlm_disruption_ = 0;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

uint32_t FRMSC_get_time_threshold_for_tlm_disruption(const FRM_StreamConfig* p_stream_config)
{
  return (uint32_t)p_stream_config->settings.time_threshold_for_tlm_disruption_;
}

void FRMSC_set_time_threshold_for_tlm_disruption(FRM_StreamConfig* p_stream_config,
                                                 const uint32_t time_threshold_for_tlm_disruption)
{
  p_stream_config->settings.time_threshold_for_tlm_disruption_ = time_threshold_for_tlm_disruption;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}

void FRMSC_set_data_analyzer(FRM_StreamConfig* p_stream_config,
                             FRM_ERR_CODE (*data_analyzer)(FRM_StreamConfig* p_stream_config, void* p_driver))
{
  p_stream_config->settings.data_analyzer_ = data_analyzer;
  p_stream_config->internal.is_validation_needed_for_rec_ = 1;
}


// ###### FRM_StreamConfig Getter/Setter of Info ######
const FRM_StreamSendStatus* FRMSC_get_send_status(const FRM_StreamConfig* p_stream_config)
{
  return &p_stream_config->info.send_status_;
}

const FRM_StreamRecStatus* FRMSC_get_rec_status(const FRM_StreamConfig* p_stream_config)
{
  return &p_stream_config->info.rec_status_;
}

uint32_t FRMSC_get_general_cmd_tx_count(const FRM_StreamConfig* p_stream_config)
{
  return (uint32_t)p_stream_config->info.general_cmd_tx_count_;
}

uint32_t FRMSC_get_req_tlm_cmd_tx_count(const FRM_StreamConfig* p_stream_config)
{
  return (uint32_t)p_stream_config->info.req_tlm_cmd_tx_count_;
}

uint32_t FRMSC_get_req_tlm_cmd_tx_count_after_last_tx(const FRM_StreamConfig* p_stream_config)
{
  return (uint32_t)p_stream_config->info.req_tlm_cmd_tx_count_after_last_tx_;
}

uint32_t FRMSC_get_rx_frame_fix_count(const FRM_StreamConfig* p_stream_config)
{
  return (uint32_t)p_stream_config->info.rx_frame_fix_count_;
}

const ObcTime* FRMSC_get_general_cmd_tx_time(const FRM_StreamConfig* p_stream_config)
{
  return &p_stream_config->info.general_cmd_tx_time_;
}

const ObcTime* FRMSC_get_req_tlm_cmd_tx_time(const FRM_StreamConfig* p_stream_config)
{
  return &p_stream_config->info.req_tlm_cmd_tx_time_;
}

const ObcTime* FRMSC_get_rx_frame_fix_time(const FRM_StreamConfig* p_stream_config)
{
  return &p_stream_config->info.rx_frame_fix_time_;
}

FRM_STREAM_TLM_DISRUPTION_STATUS_CODE FRMSC_get_tlm_disruption_status(const FRM_StreamConfig* p_stream_config)
{
  return (FRM_STREAM_TLM_DISRUPTION_STATUS_CODE)p_stream_config->info.rec_status_.tlm_disruption_status;
}

FRM_ERR_CODE FRMSC_get_ret_from_data_analyzer(const FRM_StreamConfig* p_stream_config)
{
  return (FRM_ERR_CODE)p_stream_config->info.ret_from_data_analyzer_;
}


// ###### Driver 汎用 Util 関数 ######

FRM_ERR_CODE FRM_init_stream_rec_buffer(FRM_StreamRecBuffer* stream_rec_buffer,
                                        uint8_t* buffer,
                                        const uint16_t buffer_capacity)
{
  if (stream_rec_buffer == NULL) return FRM_ERR_CODE_ERR;
  if (buffer == NULL) return FRM_ERR_CODE_ERR;
  stream_rec_buffer->buffer = buffer;
  stream_rec_buffer->capacity = buffer_capacity;
  FRM_clear_stream_rec_buffer_(stream_rec_buffer);
  return FRM_ERR_CODE_OK;
}


void FRM_nullify_stream_rec_buffers(FRM_StreamRecBuffer* rx_buffers[FRM_STREAM_MAX])
{
  uint8_t stream;
  if (rx_buffers == NULL) return;

  for (stream = 0; stream < FRM_STREAM_MAX; ++stream)
  {
    rx_buffers[stream] = NULL;
  }
}


CCP_CmdRet FRM_conv_driver_err_to_ccp_cmd_ret(FRM_DRIVER_ERR_CODE code)
{
  switch (code)
  {
  case FRM_DRIVER_ERR_CODE_ILLEGAL_CONTEXT:
  case FRM_DRIVER_ERR_CODE_UNKNOWN_ERR:
    // 全てこれでいいのかは，要検討
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT, (uint32_t)code);
  case FRM_DRIVER_ERR_CODE_ILLEGAL_PARAMETER:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, (uint32_t)code);
  case FRM_DRIVER_ERR_CODE_ILLEGAL_LENGTH:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_LENGTH, (uint32_t)code);
  default:
    // ここに来るのは以下
    // FRM_DRIVER_ERR_CODE_OK
    return CCP_make_cmd_ret(CCP_EXEC_SUCCESS, (uint32_t)code);
  }
}


CCP_CmdRet FRM_conv_cmd_err_to_ccp_cmd_ret(FRM_CMD_ERR_CODE code)
{
  switch (code)
  {
  case FRM_CMD_ILLEGAL_CONTEXT:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_CONTEXT, (uint32_t)code);
  case FRM_CMD_ILLEGAL_PARAMETER:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_PARAMETER, (uint32_t)code);
  case FRM_CMD_ILLEGAL_LENGTH:
    return CCP_make_cmd_ret(CCP_EXEC_ILLEGAL_LENGTH, (uint32_t)code);
  default:
    // ここに来るのは以下の 3 つ
    // FRM_CMD_OK
    // FRM_CMD_DRIVER_SUPER_ERR
    // FRM_CMD_UNKNOWN_ERR
    // 下 2 つのエラーは Driver 側の問題で，そちらでエラー情報を持つべき
    // ここでは SUCCESS を返す
    return CCP_make_cmd_ret(CCP_EXEC_SUCCESS, (uint32_t)code);
  }
}


const uint8_t* FRMSC_get_rx_frame(const FRM_StreamConfig* p_stream_config)
{
  FRM_StreamRecBuffer* buffer = p_stream_config->settings.rx_buffer_;
  return &(buffer->buffer[buffer->pos_of_frame_head_candidate]);
}


uint16_t FRMSC_get_fixed_rx_frame_size(const FRM_StreamConfig* p_stream_config)
{
  FRM_StreamRecBuffer* buffer = p_stream_config->settings.rx_buffer_;
  if (buffer->is_frame_fixed)
  {
    return buffer->confirmed_frame_len;
  }
  else
  {
    return 0;
  }
}

void FRM_clear_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer)
{
  if (stream_rec_buffer == NULL) return;
  if (stream_rec_buffer->buffer == NULL) return;

  memset(stream_rec_buffer->buffer,
         0x00,
         stream_rec_buffer->capacity);
  stream_rec_buffer->size = 0;
  stream_rec_buffer->pos_of_frame_head_candidate = 0;
  stream_rec_buffer->confirmed_frame_len = 0;
  stream_rec_buffer->is_frame_fixed = 0;
  stream_rec_buffer->pos_of_last_rec = 0;
}


void FRM_drop_from_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                      uint16_t size)
{
  int32_t move_size = stream_rec_buffer->size - size;

  if (size == 0) return;

  if (move_size > 0)
  {
    memmove(&stream_rec_buffer->buffer[0],
            &stream_rec_buffer->buffer[size],
            move_size);
  }

  // データ整合
  if (stream_rec_buffer->pos_of_frame_head_candidate < size)
  {
    stream_rec_buffer->confirmed_frame_len = 0;
  }

  if (stream_rec_buffer->size > size)
  {
    stream_rec_buffer->size -= size;
  }
  else
  {
    stream_rec_buffer->size = 0;
  }

  if (stream_rec_buffer->pos_of_frame_head_candidate >= size)
  {
    stream_rec_buffer->pos_of_frame_head_candidate -= size;
  }
  else
  {
    stream_rec_buffer->pos_of_frame_head_candidate = 0;
    stream_rec_buffer->is_frame_fixed = 0;
  }

  if (stream_rec_buffer->pos_of_last_rec > size)
  {
    stream_rec_buffer->pos_of_last_rec -= size;
  }
  else
  {
    stream_rec_buffer->pos_of_last_rec = 0;
  }
}


FRM_ERR_CODE FRM_push_to_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                            const uint8_t* buffer,
                                            uint16_t size)
{
  uint16_t rest_size = stream_rec_buffer->capacity - stream_rec_buffer->size;
  if (rest_size < size) return FRM_ERR_CODE_ERR;

  memcpy(&stream_rec_buffer->buffer[stream_rec_buffer->size],
         buffer,
         size);
  stream_rec_buffer->size += size;

  return FRM_ERR_CODE_OK;
}


uint16_t FRM_get_unprocessed_size_from_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer)
{
  int32_t len = stream_rec_buffer->size - stream_rec_buffer->pos_of_frame_head_candidate - stream_rec_buffer->confirmed_frame_len;
  if (len < 0) return 0;      // まああり得ないが
  return (uint16_t)len;
}


void FRM_confirm_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                    uint16_t size)
{
  stream_rec_buffer->confirmed_frame_len += size;
  if (stream_rec_buffer->confirmed_frame_len > stream_rec_buffer->size)
  {
    stream_rec_buffer->confirmed_frame_len = stream_rec_buffer->size;
  }
}


void FRM_move_forward_frame_head_candidate_of_stream_rec_buffer_(FRM_StreamRecBuffer* stream_rec_buffer,
                                                                 uint16_t size)
{
  stream_rec_buffer->pos_of_frame_head_candidate += size;
  if (stream_rec_buffer->pos_of_frame_head_candidate > stream_rec_buffer->size)
  {
    stream_rec_buffer->pos_of_frame_head_candidate = stream_rec_buffer->size;
  }
  // head が動くということは，フレーム探索中ということ
  stream_rec_buffer->confirmed_frame_len = 0;
}

#pragma section