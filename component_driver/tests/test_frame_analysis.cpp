/**
 * @file test_frame_analysis.cpp
 * @brief CDS_receive() を通したフレーム解析のユニットテスト
 *
 * フレーム解析機能のテスト:
 * - 固定長フレーム（ヘッダ有/無、フッタ有/無）
 * - 可変長フレーム（フレーム長データ付き）
 * - 可変長フレーム（フッタ終端）
 * - 複数フレーム連続受信
 * - 厳格なフレーム探索モード
 * - ノイズ混入時のリカバリー
 *
 * このテストは公開 API (CDS_receive) を経由してフレーム解析をテストする。
 * HAL モックを使用してデータを注入し、実際の driver_super.c の実装を検証する。
 */
#include <gtest/gtest.h>
#include <cstring>

extern "C" {

// prelude が型を提供するため、driver_super.h のみインクルード
// mock 関数の宣言のみ必要
#include "driver_super.h"

// モック制御用関数の宣言（mock_hal_handler_registry.c で定義）
void mock_hal_reset(void);
void mock_hal_set_rx_data(const uint8_t* data, int len);
void mock_hal_append_rx_data(const uint8_t* data, int len);
void mock_hal_set_rx_chunk_size(int chunk_size);

// モック制御用関数の宣言（mock_time_manager.c で定義）
void mock_time_reset(void);

}  // extern "C"

// ================================================================
// テストフィクスチャ
// ================================================================

class FrameAnalysisTest : public ::testing::Test {
protected:
  static constexpr uint16_t RX_BUFFER_SIZE = 512;
  uint8_t rx_buffer_mem_[RX_BUFFER_SIZE];
  CDS_StreamRecBuffer rx_buffer_;
  ComponentDriverSuper super_;
  int dummy_hal_config_;  // CDS_validate_config では NULL でないことが必要

  // stream 設定用の静的データ（テスト間で共有）
  static const uint8_t* current_header_;
  static uint16_t current_header_size_;
  static const uint8_t* current_footer_;
  static uint16_t current_footer_size_;
  static int16_t current_rx_frame_size_;
  static uint16_t current_max_rx_frame_size_;
  static int16_t current_framelength_pos_;
  static uint16_t current_framelength_type_size_;
  static uint16_t current_framelength_offset_;
  static ENDIAN_TYPE current_framelength_endian_;

  void SetUp() override {
    // モックをリセット
    mock_hal_reset();
    mock_time_reset();

    // 静的変数をリセット
    current_header_ = nullptr;
    current_header_size_ = 0;
    current_footer_ = nullptr;
    current_footer_size_ = 0;
    current_rx_frame_size_ = 0;
    current_max_rx_frame_size_ = 0xffff;
    current_framelength_pos_ = -1;
    current_framelength_type_size_ = 0;
    current_framelength_offset_ = 0;
    current_framelength_endian_ = ENDIAN_TYPE_BIG;

    // 受信バッファ初期化
    memset(rx_buffer_mem_, 0, sizeof(rx_buffer_mem_));
    CDS_init_stream_rec_buffer(&rx_buffer_, rx_buffer_mem_, RX_BUFFER_SIZE);

    // ComponentDriverSuper 初期化
    memset(&super_, 0, sizeof(super_));
  }

  void TearDown() override {
    mock_hal_reset();
  }

  // load_init_setting コールバック
  static CDS_ERR_CODE LoadInitSetting(ComponentDriverSuper* p_super) {
    // HAL handler ID を設定（validation: 0 <= hal_handler_id < HAL_HANDLER_ID_MAX）
    p_super->hal_handler_id = HAL_HANDLER_ID_UART;

    // hal_rx_buffer_size_ を受信バッファサイズに合わせる
    // (validation: rx_buffer_->capacity >= hal_rx_buffer_size_)
    CDSC_set_hal_rx_buffer_size(p_super, RX_BUFFER_SIZE);

    CDS_StreamConfig* p_stream = &p_super->stream_config[0];

    CDSSC_enable(p_stream);
    CDSSC_set_rx_header(p_stream, current_header_, current_header_size_);
    CDSSC_set_rx_footer(p_stream, current_footer_, current_footer_size_);
    CDSSC_set_rx_frame_size(p_stream, current_rx_frame_size_);
    CDSSC_set_max_rx_frame_size(p_stream, current_max_rx_frame_size_);

    if (current_framelength_pos_ >= 0) {
      CDSSC_set_rx_framelength_pos(p_stream, current_framelength_pos_);
      CDSSC_set_rx_framelength_type_size(p_stream, current_framelength_type_size_);
      CDSSC_set_rx_framelength_offset(p_stream, current_framelength_offset_);
      CDSSC_set_rx_framelength_endian(p_stream, current_framelength_endian_);
    }

    return CDS_ERR_CODE_OK;
  }

  // ヘルパー: ComponentDriverSuper を初期化
  void InitSuper() {
    CDS_ERR_CODE ret = CDS_init(&super_, &dummy_hal_config_, &rx_buffer_, LoadInitSetting);
    ASSERT_EQ(CDS_ERR_CODE_OK, ret);
  }

  // ヘルパー: 固定長フレーム設定
  void SetupFixedFrame(const uint8_t* header, uint16_t header_size,
                       const uint8_t* footer, uint16_t footer_size,
                       int16_t frame_size) {
    current_header_ = header;
    current_header_size_ = header_size;
    current_footer_ = footer;
    current_footer_size_ = footer_size;
    current_rx_frame_size_ = frame_size;
    InitSuper();
  }

  // ヘルパー: 可変長フレーム設定（フレーム長データ付き）
  void SetupVariableFrameWithLength(const uint8_t* header, uint16_t header_size,
                                    const uint8_t* footer, uint16_t footer_size,
                                    int16_t framelength_pos, uint16_t framelength_type_size,
                                    uint16_t framelength_offset, ENDIAN_TYPE endian) {
    current_header_ = header;
    current_header_size_ = header_size;
    current_footer_ = footer;
    current_footer_size_ = footer_size;
    current_rx_frame_size_ = -1;  // 可変長
    current_framelength_pos_ = framelength_pos;
    current_framelength_type_size_ = framelength_type_size;
    current_framelength_offset_ = framelength_offset;
    current_framelength_endian_ = endian;
    InitSuper();
  }

  // ヘルパー: 可変長フレーム設定（フッタ終端）
  void SetupVariableFrameWithFooter(const uint8_t* header, uint16_t header_size,
                                    const uint8_t* footer, uint16_t footer_size) {
    current_header_ = header;
    current_header_size_ = header_size;
    current_footer_ = footer;
    current_footer_size_ = footer_size;
    current_rx_frame_size_ = -1;  // 可変長
    current_framelength_pos_ = -1;  // フレーム長なし
    InitSuper();
  }

  // ヘルパー: max_rx_frame_size 設定
  void SetMaxRxFrameSize(uint16_t max_size) {
    current_max_rx_frame_size_ = max_size;
  }

  // HAL にデータをセットして受信処理
  void ReceiveData(const uint8_t* data, uint16_t size) {
    mock_hal_set_rx_data(data, size);
    CDS_receive(&super_);
  }

  // HAL にデータを追加して受信処理
  void AppendAndReceive(const uint8_t* data, uint16_t size) {
    mock_hal_append_rx_data(data, size);
    CDS_receive(&super_);
  }

  // 受信結果を取得
  CDS_STREAM_REC_STATUS_CODE GetRecStatusCode() {
    return CDSSC_get_rec_status(&super_.stream_config[0])->status_code;
  }

  uint16_t GetFixedFrameLen() {
    return CDSSC_get_rec_status(&super_.stream_config[0])->fixed_frame_len;
  }

  const uint8_t* GetRxFrame() {
    return CDSSC_get_rx_frame(&super_.stream_config[0]);
  }

  uint16_t GetFixedRxFrameSize() {
    return CDSSC_get_fixed_rx_frame_size(&super_.stream_config[0]);
  }
};

// 静的メンバ変数の定義
const uint8_t* FrameAnalysisTest::current_header_ = nullptr;
uint16_t FrameAnalysisTest::current_header_size_ = 0;
const uint8_t* FrameAnalysisTest::current_footer_ = nullptr;
uint16_t FrameAnalysisTest::current_footer_size_ = 0;
int16_t FrameAnalysisTest::current_rx_frame_size_ = 0;
uint16_t FrameAnalysisTest::current_max_rx_frame_size_ = 0xffff;
int16_t FrameAnalysisTest::current_framelength_pos_ = -1;
uint16_t FrameAnalysisTest::current_framelength_type_size_ = 0;
uint16_t FrameAnalysisTest::current_framelength_offset_ = 0;
ENDIAN_TYPE FrameAnalysisTest::current_framelength_endian_ = ENDIAN_TYPE_BIG;

// ================================================================
// 固定長フレームテスト
// ================================================================

// 固定長フレーム: ヘッダ有り、フッタ無し、一括受信でフレーム確定
TEST_F(FrameAnalysisTest, FixedFrameHeaderOnlyComplete) {
  const uint8_t header[] = {0xEB, 0x90};
  // フレーム: [EB 90] [DATA x 4] = 6 bytes
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());
}

// 固定長フレーム: ヘッダ有り、フッタ有り、一括受信でフレーム確定
TEST_F(FrameAnalysisTest, FixedFrameHeaderFooterComplete) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  // フレーム: [EB 90] [DATA x 4] [C5 79] = 8 bytes
  SetupFixedFrame(header, 2, footer, 2, 8);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0xC5, 0x79};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(8, GetFixedFrameLen());
}

// 固定長フレーム: ヘッダ無し、フッタ無し（データ先頭からフレーム開始）
TEST_F(FrameAnalysisTest, FixedFrameNoHeaderNoFooter) {
  // ヘッダ・フッタなしの 4 バイトフレーム
  SetupFixedFrame(nullptr, 0, nullptr, 0, 4);

  uint8_t frame[] = {0x01, 0x02, 0x03, 0x04};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(4, GetFixedFrameLen());
}

// 固定長フレーム: ヘッダ無し、フッタ有り
TEST_F(FrameAnalysisTest, FixedFrameNoHeaderWithFooter) {
  const uint8_t footer[] = {0xFF};
  // フレーム: [DATA x 3] [FF] = 4 bytes
  SetupFixedFrame(nullptr, 0, footer, 1, 4);

  uint8_t frame[] = {0x01, 0x02, 0x03, 0xFF};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(4, GetFixedFrameLen());
}

// 固定長フレーム: 分割受信（ヘッダ → データ → フッタ）
TEST_F(FrameAnalysisTest, FixedFrameFragmentedReception) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  SetupFixedFrame(header, 2, footer, 2, 8);

  // ヘッダ部分のみ受信
  uint8_t part1[] = {0xEB, 0x90};
  ReceiveData(part1, sizeof(part1));
  EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());

  // データ部分受信
  uint8_t part2[] = {0x01, 0x02, 0x03, 0x04};
  AppendAndReceive(part2, sizeof(part2));
  EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());

  // フッタ部分受信
  uint8_t part3[] = {0xC5, 0x79};
  AppendAndReceive(part3, sizeof(part3));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(8, GetFixedFrameLen());
}

// 固定長フレーム: ノイズ後にフレーム受信（ヘッダ探索テスト）
TEST_F(FrameAnalysisTest, FixedFrameWithNoisePrefix) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // ノイズ + 有効フレーム
  uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveData(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());
}

// 固定長フレーム: ヘッダ不一致からのリカバリー
TEST_F(FrameAnalysisTest, FixedFrameHeaderMismatchRecovery) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // 偽ヘッダ（0xEB だけ一致）+ 本物のフレーム
  uint8_t data[] = {0xEB, 0x00, 0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveData(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());
}

// 固定長フレーム: フッタ不一致からのリカバリー
TEST_F(FrameAnalysisTest, FixedFrameFooterMismatchRecovery) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  SetupFixedFrame(header, 2, footer, 2, 8);

  // 偽フレーム（フッタ不一致）+ 本物のフレーム
  uint8_t data[] = {
    0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00,  // 偽（フッタ不一致）
    0xEB, 0x90, 0xAA, 0xBB, 0xCC, 0xDD, 0xC5, 0x79   // 本物
  };
  ReceiveData(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(8, GetFixedFrameLen());
}

// ================================================================
// 可変長フレームテスト（フレーム長データ付き）
// ================================================================

// 可変長フレーム: ビッグエンディアン、フレーム長 1 バイト
TEST_F(FrameAnalysisTest, VariableFrameBigEndian1Byte) {
  const uint8_t header[] = {0xEB, 0x90};
  // フォーマット: [EB 90] [LEN:1byte] [DATA...] = LEN bytes total
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2,  // framelength_pos
                               1,  // framelength_type_size (1 byte)
                               0,  // offset
                               ENDIAN_TYPE_BIG);

  // フレーム: EB 90 08 01 02 03 04 05 = 8 bytes (LEN=8)
  uint8_t frame[] = {0xEB, 0x90, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(8, GetFixedFrameLen());
}

// 可変長フレーム: ビッグエンディアン、フレーム長 2 バイト
TEST_F(FrameAnalysisTest, VariableFrameBigEndian2Byte) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2,  // framelength_pos
                               2,  // framelength_type_size (2 bytes)
                               0,  // offset
                               ENDIAN_TYPE_BIG);

  // フレーム: EB 90 00 0A 01 02 03 04 05 06 = 10 bytes (LEN=0x000A=10)
  uint8_t frame[] = {0xEB, 0x90, 0x00, 0x0A, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(10, GetFixedFrameLen());
}

// 可変長フレーム: リトルエンディアン、フレーム長 2 バイト
TEST_F(FrameAnalysisTest, VariableFrameLittleEndian2Byte) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2,  // framelength_pos
                               2,  // framelength_type_size (2 bytes)
                               0,  // offset
                               ENDIAN_TYPE_LITTLE);

  // フレーム: EB 90 0A 00 01 02 03 04 05 06 = 10 bytes (LEN=0x000A=10, little endian)
  uint8_t frame[] = {0xEB, 0x90, 0x0A, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(10, GetFixedFrameLen());
}

// 可変長フレーム: オフセット付き（ヘッダ・フッタ除くボディ長）
TEST_F(FrameAnalysisTest, VariableFrameWithOffset) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  // LEN フィールドがボディ長（ヘッダ・フッタ・LEN自体を含まない）を示す場合
  // offset = header(2) + len_field(1) + footer(2) = 5
  SetMaxRxFrameSize(0xffff);
  SetupVariableFrameWithLength(header, 2, footer, 2,
                               2,  // framelength_pos
                               1,  // framelength_type_size
                               5,  // offset (header + len + footer size)
                               ENDIAN_TYPE_BIG);

  // フレーム: EB 90 04 01 02 03 04 C5 79 = 9 bytes
  // LEN=4 (body length), total = 4 + 5 = 9
  uint8_t frame[] = {0xEB, 0x90, 0x04, 0x01, 0x02, 0x03, 0x04, 0xC5, 0x79};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(9, GetFixedFrameLen());
}

// 可変長フレーム: フレーム長が大きすぎる場合のエラー
TEST_F(FrameAnalysisTest, VariableFrameTooLong) {
  const uint8_t header[] = {0xEB, 0x90};
  SetMaxRxFrameSize(100);  // 最大 100 バイト
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2,  // framelength_pos
                               2,  // framelength_type_size
                               0,  // offset
                               ENDIAN_TYPE_BIG);

  // LEN = 0x1000 = 4096 (max_rx_frame_size_ を超える)
  uint8_t frame[] = {0xEB, 0x90, 0x10, 0x00, 0x01, 0x02};
  ReceiveData(frame, sizeof(frame));

  // エラー後、ヘッダ探索に戻る
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, GetRecStatusCode());
}

// 可変長フレーム: フレーム長が小さすぎる場合のエラー
TEST_F(FrameAnalysisTest, VariableFrameTooShort) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  SetupVariableFrameWithLength(header, 2, footer, 2,
                               2,  // framelength_pos
                               1,  // framelength_type_size
                               0,  // offset
                               ENDIAN_TYPE_BIG);

  // LEN = 2 だがヘッダ(2)+フッタ(2)=4 より小さい → エラー
  uint8_t frame[] = {0xEB, 0x90, 0x02, 0x01, 0x02};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, GetRecStatusCode());
}

// ================================================================
// 可変長フレームテスト（フッタ終端）
// ================================================================

// 可変長フレーム（フッタ終端）: 基本ケース
TEST_F(FrameAnalysisTest, VariableFrameFooterTerminated) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0D, 0x0A};  // CRLF
  SetupVariableFrameWithFooter(header, 2, footer, 2);

  // フレーム: EB 90 01 02 03 04 05 0D 0A
  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0x05, 0x0D, 0x0A};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(9, GetFixedFrameLen());
}

// 可変長フレーム（フッタ終端）: 単一バイトフッタ
TEST_F(FrameAnalysisTest, VariableFrameSingleByteFooter) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0A};  // LF only
  SetupVariableFrameWithFooter(header, 2, footer, 1);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x0A};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());
}

// 可変長フレーム（フッタ終端）: 偽フッタのスキップ
TEST_F(FrameAnalysisTest, VariableFrameFalseFooterSkip) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0D, 0x0A};
  SetupVariableFrameWithFooter(header, 2, footer, 2);

  // データ中に 0x0A（フッタ末尾）があるが、0x0D が先行しないケース
  uint8_t frame[] = {0xEB, 0x90, 0x0A, 0x0A, 0x0A, 0x0D, 0x0A};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(7, GetFixedFrameLen());
}

// 可変長フレーム（フッタ終端）: 最大長超過エラー
TEST_F(FrameAnalysisTest, VariableFrameFooterTerminatedTooLong) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0A};
  SetMaxRxFrameSize(6);
  SetupVariableFrameWithFooter(header, 2, footer, 1);

  // 7 バイト（最大 6 を超過）
  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0x0A};
  ReceiveData(frame, sizeof(frame));

  // max_rx_frame_size_ 超過でエラー → ヘッダ探索に戻る
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, GetRecStatusCode());
}

// ================================================================
// 複数フレーム連続受信テスト
// ================================================================

// 複数フレームが一度に到着: 最初のフレームのみ確定
TEST_F(FrameAnalysisTest, MultipleFramesFirstOnly) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // 2 つのフレームが連続
  uint8_t data[] = {
    0xEB, 0x90, 0x01, 0x02, 0x03, 0x04,  // frame 1
    0xEB, 0x90, 0x11, 0x12, 0x13, 0x14   // frame 2
  };
  ReceiveData(data, sizeof(data));

  // 最初のフレームのみ確定
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());
}

// 次のフレーム処理のシミュレーション
TEST_F(FrameAnalysisTest, ProcessSecondFrame) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  uint8_t data[] = {
    0xEB, 0x90, 0x01, 0x02, 0x03, 0x04,
    0xEB, 0x90, 0x11, 0x12, 0x13, 0x14
  };
  ReceiveData(data, sizeof(data));

  // 最初のフレーム確定
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());

  // 再度 CDS_receive を呼び出すと次のフレームが処理される
  // （HAL からの新規データなし、バッファ内の残りを処理）
  mock_hal_set_rx_data(nullptr, 0);  // 新規データなし
  CDS_receive(&super_);

  // 2 つ目のフレーム確定
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(6, GetFixedFrameLen());
}

// ================================================================
// 1 バイトずつ受信テスト（細切れ受信）
// ================================================================

// 1 バイトずつ受信して固定長フレーム確定
TEST_F(FrameAnalysisTest, ByteByByteReception) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  SetupFixedFrame(header, 2, footer, 2, 8);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0xC5, 0x79};

  // HAL を 1 バイトずつ返すように設定
  mock_hal_set_rx_chunk_size(1);
  mock_hal_set_rx_data(frame, sizeof(frame));

  // 7 回の receive ではフレーム未確定
  for (int i = 0; i < 7; i++) {
    CDS_receive(&super_);
    EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  }

  // 最後の 1 バイトでフレーム確定
  CDS_receive(&super_);
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(8, GetFixedFrameLen());
}

// ================================================================
// エッジケーステスト
// ================================================================

// ヘッダがバッファの最後にある場合
TEST_F(FrameAnalysisTest, HeaderAtBufferEnd) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // ノイズ + ヘッダの先頭 1 バイトのみ
  uint8_t part1[] = {0xFF, 0xFF, 0xEB};
  ReceiveData(part1, sizeof(part1));
  EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());

  // 残りを受信
  uint8_t part2[] = {0x90, 0x01, 0x02, 0x03, 0x04};
  AppendAndReceive(part2, sizeof(part2));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
}

// 空データ受信: 何も起きない
TEST_F(FrameAnalysisTest, EmptyDataReception) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // 空データ
  mock_hal_set_rx_data(nullptr, 0);
  CDS_receive(&super_);

  // 何も受信していないのでステータスは変わらない
  // 初期状態は FINDING_HEADER（または DISABLE でなければ OK）
}

// rx_frame_size_ = 0 の場合: stream は無効
TEST_F(FrameAnalysisTest, FrameSizeZeroNoProcessing) {
  // rx_frame_size = 0 で初期化すると stream は無効になる
  current_rx_frame_size_ = 0;
  InitSuper();

  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  ReceiveData(data, sizeof(data));

  // stream が無効なので FIXED_FRAME にはならない
  EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
}

// ヘッダ全体がデータ内に複数回出現
TEST_F(FrameAnalysisTest, MultipleHeaderOccurrences) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  SetupFixedFrame(header, 2, footer, 2, 8);

  // 最初の "EB 90" 後のフッタが不一致、2 つ目の "EB 90" が本物
  uint8_t data[] = {
    0xEB, 0x90, 0xEB, 0x90, 0x01, 0x02, 0x00, 0x00,  // 偽（EB 90 が 2 回、フッタ不一致）
    0xEB, 0x90, 0xAA, 0xBB, 0xCC, 0xDD, 0xC5, 0x79   // 本物
  };
  ReceiveData(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
}

// ================================================================
// フレーム長フィールドの境界テスト
// ================================================================

// フレーム長フィールドが分割受信される場合
TEST_F(FrameAnalysisTest, FramelengthFieldFragmented) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2, 2, 0, ENDIAN_TYPE_BIG);

  // ヘッダ + フレーム長の上位バイトのみ
  uint8_t part1[] = {0xEB, 0x90, 0x00};
  ReceiveData(part1, sizeof(part1));
  EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());

  // フレーム長の下位バイト + データ
  uint8_t part2[] = {0x08, 0x01, 0x02, 0x03, 0x04};
  AppendAndReceive(part2, sizeof(part2));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(8, GetFixedFrameLen());
}

// 4 バイトフレーム長（uint32_t）
TEST_F(FrameAnalysisTest, FramelengthField4Bytes) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2, 4, 0, ENDIAN_TYPE_BIG);

  // フレーム: EB 90 00 00 00 0C ... = 12 bytes (LEN=0x0000000C)
  uint8_t frame[] = {0xEB, 0x90, 0x00, 0x00, 0x00, 0x0C,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(12, GetFixedFrameLen());
}

// ================================================================
// 実際の通信プロトコル形式テスト
// ================================================================

// EB90 フレーム形式（C2A 標準）
TEST_F(FrameAnalysisTest, EB90FrameFormat) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  // EB90 format: [EB 90] [LEN:2bytes] [DATA...] [C5 79]
  // LEN = total frame size
  SetupVariableFrameWithLength(header, 2, footer, 2,
                               2, 2, 0, ENDIAN_TYPE_BIG);

  // フレーム: EB 90 00 0C 01 02 03 04 05 06 C5 79 = 12 bytes
  uint8_t frame[] = {0xEB, 0x90, 0x00, 0x0C, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xC5, 0x79};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(12, GetFixedFrameLen());
}

// NMEA 形式（$ ... *XX\r\n）のシミュレーション
TEST_F(FrameAnalysisTest, NMEALikeFormat) {
  const uint8_t header[] = {'$'};
  const uint8_t footer[] = {'\r', '\n'};
  SetupVariableFrameWithFooter(header, 1, footer, 2);

  // $GPGGA,123456,*XX\r\n
  uint8_t frame[] = {'$', 'G', 'P', 'G', 'G', 'A', ',', '1', '2', '3', '*', 'X', 'X', '\r', '\n'};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());
  EXPECT_EQ(15, GetFixedFrameLen());
}

// ================================================================
// フレームデータ検証テスト
// ================================================================

// 受信したフレームの内容を検証
TEST_F(FrameAnalysisTest, VerifyReceivedFrameContent) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  uint8_t frame[] = {0xEB, 0x90, 0x11, 0x22, 0x33, 0x44};
  ReceiveData(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, GetRecStatusCode());

  // 受信フレームの内容を検証
  const uint8_t* rx_frame = GetRxFrame();
  ASSERT_NE(nullptr, rx_frame);
  EXPECT_EQ(0xEB, rx_frame[0]);
  EXPECT_EQ(0x90, rx_frame[1]);
  EXPECT_EQ(0x11, rx_frame[2]);
  EXPECT_EQ(0x22, rx_frame[3]);
  EXPECT_EQ(0x33, rx_frame[4]);
  EXPECT_EQ(0x44, rx_frame[5]);
}
