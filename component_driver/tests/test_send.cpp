/**
 * @file test_send.cpp
 * @brief CDS_send_general_cmd / CDS_send_req_tlm_cmd（送信系）のユニットテスト
 *
 * 送信系のテスト:
 * - 正常送信（HAL に届く内容・send_status・カウンタ・送信時刻）
 * - 無効 stream / tx フレーム未設定の no-op 動作
 * - HAL tx エラーの伝播 (TX_ERR)
 * - 送信前 validation エラー (VALIDATE_ERR)
 * - req_tlm_cmd_tx_count_after_last_tx のフレーム確定によるリセット
 */
#include <gtest/gtest.h>
#include <cstring>

extern "C" {

// prelude が重い依存を遮断するため、実 driver_super.h をそのまま include する
#include "driver_super.h"

// モック制御用関数の宣言（mock_hal_handler_registry.c で定義）
void mock_hal_reset(void);
void mock_hal_set_rx_data(const uint8_t* data, int len);
void mock_hal_set_tx_result(int result);
int mock_hal_get_tx_count(void);
const uint8_t* mock_hal_get_last_tx_data(void);
int mock_hal_get_last_tx_size(void);

// モック制御用関数の宣言（mock_time_manager.c で定義）
void mock_time_reset(void);
void mock_time_set_current(cycle_t total_cycle);

}  // extern "C"

// ================================================================
// テストフィクスチャ
// ================================================================

class SendTest : public ::testing::Test {
protected:
  static constexpr uint16_t RX_BUFFER_SIZE = 256;
  uint8_t rx_buffer_mem_[RX_BUFFER_SIZE];
  CDS_StreamRecBuffer rx_buffer_;
  ComponentDriverSuper super_;
  int dummy_hal_config_;

  // 受信側の設定（validation を通すため最小構成: ヘッダ EB 90 + 固定長 6）
  static const uint8_t rx_header_[2];

  // 送信側の設定（テスト間で共有する静的データ）
  static const uint8_t* current_tx_frame_;
  static int16_t current_tx_frame_size_;
  static int16_t current_tx_frame_buffer_size_;

  void SetUp() override {
    mock_hal_reset();
    mock_time_reset();

    current_tx_frame_ = nullptr;
    current_tx_frame_size_ = 0;
    current_tx_frame_buffer_size_ = -1;

    memset(rx_buffer_mem_, 0, sizeof(rx_buffer_mem_));
    CDS_init_stream_rec_buffer(&rx_buffer_, rx_buffer_mem_, RX_BUFFER_SIZE);
    memset(&super_, 0, sizeof(super_));
  }

  void TearDown() override {
    mock_hal_reset();
  }

  static CDS_ERR_CODE LoadInitSetting(ComponentDriverSuper* p_super) {
    CDS_StreamConfig* p_stream = &p_super->stream_config[0];

    p_super->hal_handler_id = HAL_HANDLER_ID_UART;
    CDSC_set_hal_rx_buffer_size(p_super, RX_BUFFER_SIZE);

    CDSSC_enable(p_stream);
    CDSSC_set_rx_header(p_stream, rx_header_, sizeof(rx_header_));
    CDSSC_set_rx_frame_size(p_stream, 6);

    CDSSC_set_tx_frame(p_stream, const_cast<uint8_t*>(current_tx_frame_));
    CDSSC_set_tx_frame_size(p_stream, current_tx_frame_size_);
    CDSSC_set_tx_frame_buffer_size(p_stream, current_tx_frame_buffer_size_);

    return CDS_ERR_CODE_OK;
  }

  void InitSuper() {
    CDS_ERR_CODE ret = CDS_init(&super_, &dummy_hal_config_, &rx_buffer_, LoadInitSetting);
    ASSERT_EQ(CDS_ERR_CODE_OK, ret);
  }

  // ヘルパー: 送信フレーム付きで初期化
  void SetupWithTxFrame(const uint8_t* frame, int16_t size, int16_t buffer_size = -1) {
    current_tx_frame_ = frame;
    current_tx_frame_size_ = size;
    current_tx_frame_buffer_size_ = buffer_size;
    InitSuper();
  }

  CDS_StreamConfig* Stream0() { return &super_.stream_config[0]; }

  CDS_STREAM_SEND_STATUS_CODE GetSendStatusCode() {
    return CDSSC_get_send_status(Stream0())->status_code;
  }
};

const uint8_t SendTest::rx_header_[2] = {0xEB, 0x90};
const uint8_t* SendTest::current_tx_frame_ = nullptr;
int16_t SendTest::current_tx_frame_size_ = 0;
int16_t SendTest::current_tx_frame_buffer_size_ = -1;

// ================================================================
// CDS_send_general_cmd
// ================================================================

// 正常送信: HAL に内容どおりのフレームが渡り、status / カウンタが更新される
TEST_F(SendTest, SendGeneralCmdSuccess) {
  static const uint8_t tx_frame[] = {0xAA, 0xBB, 0xCC};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));

  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_send_general_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_OK, GetSendStatusCode());
  EXPECT_EQ(1, mock_hal_get_tx_count());
  EXPECT_EQ(3, mock_hal_get_last_tx_size());
  EXPECT_EQ(0, memcmp(tx_frame, mock_hal_get_last_tx_data(), sizeof(tx_frame)));
  EXPECT_EQ(1u, CDSSC_get_general_cmd_tx_count(Stream0()));
}

// 連続送信でカウンタが累積する
TEST_F(SendTest, SendGeneralCmdCountsAccumulate) {
  static const uint8_t tx_frame[] = {0x01, 0x02};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));

  CDS_send_general_cmd(&super_, 0);
  CDS_send_general_cmd(&super_, 0);

  EXPECT_EQ(2, mock_hal_get_tx_count());
  EXPECT_EQ(2u, CDSSC_get_general_cmd_tx_count(Stream0()));
}

// 送信時刻が記録される
TEST_F(SendTest, SendGeneralCmdRecordsTxTime) {
  static const uint8_t tx_frame[] = {0x01};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));

  mock_time_set_current(42);
  CDS_send_general_cmd(&super_, 0);

  EXPECT_EQ(42u, CDSSC_get_general_cmd_tx_time(Stream0())->total_cycle);
}

// 無効 stream への送信: OK を返すが DISABLE status で、HAL は呼ばれない
TEST_F(SendTest, SendGeneralCmdDisabledStream) {
  static const uint8_t tx_frame[] = {0x01, 0x02};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));
  CDSSC_disable(Stream0());

  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_send_general_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_DISABLE, GetSendStatusCode());
  EXPECT_EQ(0, mock_hal_get_tx_count());
  EXPECT_EQ(0u, CDSSC_get_general_cmd_tx_count(Stream0()));
}

// HAL tx エラー: ERR を返し、TX_ERR status と HAL の返り値が記録される
TEST_F(SendTest, SendGeneralCmdHalTxError) {
  static const uint8_t tx_frame[] = {0x01, 0x02};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));
  mock_hal_set_tx_result(-3);

  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_send_general_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_TX_ERR, GetSendStatusCode());
  EXPECT_EQ(-3, CDSSC_get_send_status(Stream0())->ret_from_hal_tx);
  EXPECT_EQ(0, mock_hal_get_tx_count());
}

// tx フレーム未設定 (size 0): 何も送らずに正常終了する
TEST_F(SendTest, SendGeneralCmdWithoutTxFrameIsNoop) {
  InitSuper();  // tx_frame = NULL, size = 0

  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_send_general_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_OK, GetSendStatusCode());
  EXPECT_EQ(0, mock_hal_get_tx_count());
}

// 送信前 validation: 初期化後に不正な設定 (frame_size > buffer_size) へ変更すると
// 送信時の validation で弾かれ、HAL は呼ばれない
TEST_F(SendTest, SendGeneralCmdValidateErrAfterBadReconfig) {
  static const uint8_t tx_frame[] = {0x01, 0x02, 0x03};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame), 8);

  // setter が is_validation_needed_for_send_ を立てる
  CDSSC_set_tx_frame_size(Stream0(), 9);  // buffer_size (8) を超過

  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_send_general_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_VALIDATE_ERR, GetSendStatusCode());
  EXPECT_EQ(0, mock_hal_get_tx_count());
}

// ================================================================
// CDS_send_req_tlm_cmd
// ================================================================

// 正常送信: req tlm 用カウンタ 2 種が更新される
TEST_F(SendTest, SendReqTlmCmdCountsAndStatus) {
  static const uint8_t tx_frame[] = {0x10, 0x20};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));

  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_send_req_tlm_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_OK, GetSendStatusCode());
  EXPECT_EQ(1u, CDSSC_get_req_tlm_cmd_tx_count(Stream0()));
  EXPECT_EQ(1u, CDSSC_get_req_tlm_cmd_tx_count_after_last_tx(Stream0()));
  EXPECT_EQ(0u, CDSSC_get_general_cmd_tx_count(Stream0()));  // general 側は不変
}

// 無効 stream への送信: DISABLE status、カウンタ不変
TEST_F(SendTest, SendReqTlmCmdDisabledStream) {
  static const uint8_t tx_frame[] = {0x10};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));
  CDSSC_disable(Stream0());

  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_send_req_tlm_cmd(&super_, 0));

  EXPECT_EQ(CDS_STREAM_SEND_STATUS_DISABLE, GetSendStatusCode());
  EXPECT_EQ(0u, CDSSC_get_req_tlm_cmd_tx_count(Stream0()));
}

// req_tlm_cmd_tx_count_after_last_tx はテレメフレーム確定でリセットされる
// (総数 req_tlm_cmd_tx_count は維持される)
TEST_F(SendTest, ReqTlmCmdCountAfterLastTxResetsOnFrameFix) {
  static const uint8_t tx_frame[] = {0x10};
  SetupWithTxFrame(tx_frame, sizeof(tx_frame));

  CDS_send_req_tlm_cmd(&super_, 0);
  CDS_send_req_tlm_cmd(&super_, 0);
  EXPECT_EQ(2u, CDSSC_get_req_tlm_cmd_tx_count_after_last_tx(Stream0()));

  // 完全なテレメフレームを受信してフレーム確定させる
  const uint8_t rx_frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  mock_hal_set_rx_data(rx_frame, sizeof(rx_frame));
  CDS_receive(&super_);
  ASSERT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME,
            CDSSC_get_rec_status(Stream0())->status_code);

  EXPECT_EQ(0u, CDSSC_get_req_tlm_cmd_tx_count_after_last_tx(Stream0()));
  EXPECT_EQ(2u, CDSSC_get_req_tlm_cmd_tx_count(Stream0()));
}
