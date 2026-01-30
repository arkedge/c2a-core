/**
 * @file test_stream_rec_buffer.cpp
 * @brief CDS_StreamRecBuffer 関連関数のユニットテスト
 */
#include <gtest/gtest.h>
#include <cstring>

extern "C" {

#include "mocks/mock_hal_handler_registry.h"
#include "mocks/mock_time_manager.h"
#include "mocks/mock_ccp.h"

typedef enum {
  ENDIAN_TYPE_BIG,
  ENDIAN_TYPE_LITTLE,
  ENDIAN_TYPE_UNKNOWN
} ENDIAN_TYPE;

#define CDS_STREAM_MAX         (3)
#define CDS_HAL_RX_BUFFER_SIZE (256)

typedef struct
{
  uint8_t* buffer;
  uint16_t capacity;
  uint16_t size;
  uint16_t pos_of_frame_head_candidate;
  uint16_t confirmed_frame_len;
  uint8_t  is_frame_fixed;
  uint16_t pos_of_last_rec;
} CDS_StreamRecBuffer;

typedef enum
{
  CDS_ERR_CODE_OK   = 0,
  CDS_ERR_CODE_ERR  = 1
} CDS_ERR_CODE;

// 関数プロトタイプ
CDS_ERR_CODE CDS_init_stream_rec_buffer(CDS_StreamRecBuffer* stream_rec_buffer,
                                        uint8_t* buffer,
                                        const uint16_t buffer_capacity);
void CDS_clear_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer);
void CDS_drop_from_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer, uint16_t size);
CDS_ERR_CODE CDS_push_to_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer,
                                            const uint8_t* buffer, uint16_t size);
uint16_t CDS_get_unprocessed_size_from_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer);
void CDS_confirm_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer, uint16_t size);
void CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer,
                                                                 uint16_t size);

// ================================================================
// 実装（driver_super.c からの抽出）
// ================================================================

CDS_ERR_CODE CDS_init_stream_rec_buffer(CDS_StreamRecBuffer* stream_rec_buffer,
                                        uint8_t* buffer,
                                        const uint16_t buffer_capacity)
{
  if (stream_rec_buffer == NULL) return CDS_ERR_CODE_ERR;
  if (buffer == NULL) return CDS_ERR_CODE_ERR;
  if (buffer_capacity == 0) return CDS_ERR_CODE_ERR;

  stream_rec_buffer->buffer = buffer;
  stream_rec_buffer->capacity = buffer_capacity;
  CDS_clear_stream_rec_buffer_(stream_rec_buffer);
  return CDS_ERR_CODE_OK;
}

void CDS_clear_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer)
{
  if (stream_rec_buffer == NULL) return;

  stream_rec_buffer->size = 0;
  stream_rec_buffer->pos_of_frame_head_candidate = 0;
  stream_rec_buffer->confirmed_frame_len = 0;
  stream_rec_buffer->is_frame_fixed = 0;
  stream_rec_buffer->pos_of_last_rec = 0;

  if (stream_rec_buffer->buffer != NULL)
  {
    memset(stream_rec_buffer->buffer, 0x00, stream_rec_buffer->capacity);
  }
}

void CDS_drop_from_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer, uint16_t size)
{
  if (stream_rec_buffer == NULL) return;
  if (size == 0) return;

  if (size >= stream_rec_buffer->size)
  {
    CDS_clear_stream_rec_buffer_(stream_rec_buffer);
    return;
  }

  uint16_t remaining = stream_rec_buffer->size - size;
  memmove(stream_rec_buffer->buffer, stream_rec_buffer->buffer + size, remaining);
  stream_rec_buffer->size = remaining;

  if (stream_rec_buffer->pos_of_frame_head_candidate >= size)
    stream_rec_buffer->pos_of_frame_head_candidate -= size;
  else
    stream_rec_buffer->pos_of_frame_head_candidate = 0;

  if (stream_rec_buffer->pos_of_last_rec >= size)
    stream_rec_buffer->pos_of_last_rec -= size;
  else
    stream_rec_buffer->pos_of_last_rec = 0;
}

CDS_ERR_CODE CDS_push_to_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer,
                                            const uint8_t* buffer, uint16_t size)
{
  if (stream_rec_buffer == NULL) return CDS_ERR_CODE_ERR;
  if (buffer == NULL) return CDS_ERR_CODE_ERR;
  if (size == 0) return CDS_ERR_CODE_OK;

  if (stream_rec_buffer->size + size > stream_rec_buffer->capacity)
    return CDS_ERR_CODE_ERR;

  memcpy(stream_rec_buffer->buffer + stream_rec_buffer->size, buffer, size);
  stream_rec_buffer->size += size;
  return CDS_ERR_CODE_OK;
}

uint16_t CDS_get_unprocessed_size_from_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer)
{
  if (stream_rec_buffer == NULL) return 0;

  uint16_t processed = stream_rec_buffer->pos_of_frame_head_candidate +
                       stream_rec_buffer->confirmed_frame_len;
  if (processed >= stream_rec_buffer->size) return 0;
  return stream_rec_buffer->size - processed;
}

void CDS_confirm_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer, uint16_t size)
{
  if (stream_rec_buffer == NULL) return;
  stream_rec_buffer->confirmed_frame_len = size;
}

void CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(CDS_StreamRecBuffer* stream_rec_buffer,
                                                                 uint16_t size)
{
  if (stream_rec_buffer == NULL) return;
  stream_rec_buffer->pos_of_frame_head_candidate += size;
  stream_rec_buffer->confirmed_frame_len = 0;

  if (stream_rec_buffer->pos_of_frame_head_candidate > stream_rec_buffer->size)
    stream_rec_buffer->pos_of_frame_head_candidate = stream_rec_buffer->size;
}

}  // extern "C"

// ================================================================
// テスト
// ================================================================

class StreamRecBufferTest : public ::testing::Test {
protected:
  static constexpr uint16_t BUFFER_SIZE = 256;
  uint8_t buffer_[BUFFER_SIZE];
  CDS_StreamRecBuffer rec_buffer_;

  void SetUp() override {
    memset(buffer_, 0, sizeof(buffer_));
    memset(&rec_buffer_, 0, sizeof(rec_buffer_));
  }
};

// 正常な初期化: バッファとキャパシティが正しく設定されることを確認
TEST_F(StreamRecBufferTest, InitSuccess) {
  CDS_ERR_CODE ret = CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  EXPECT_EQ(CDS_ERR_CODE_OK, ret);
  EXPECT_EQ(buffer_, rec_buffer_.buffer);
  EXPECT_EQ(BUFFER_SIZE, rec_buffer_.capacity);
  EXPECT_EQ(0, rec_buffer_.size);
}

// 初期化エラー: buffer が NULL の場合はエラーを返す
TEST_F(StreamRecBufferTest, InitWithNullBuffer) {
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_init_stream_rec_buffer(&rec_buffer_, nullptr, BUFFER_SIZE));
}

// 初期化エラー: stream_rec_buffer が NULL の場合はエラーを返す
TEST_F(StreamRecBufferTest, InitWithNullRecBuffer) {
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_init_stream_rec_buffer(nullptr, buffer_, BUFFER_SIZE));
}

// 初期化エラー: キャパシティが 0 の場合はエラーを返す
TEST_F(StreamRecBufferTest, InitWithZeroCapacity) {
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, 0));
}

// データ追加: バッファにデータを追加し、サイズと内容が正しいことを確認
TEST_F(StreamRecBufferTest, PushDataSuccess) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data)));
  EXPECT_EQ(5, rec_buffer_.size);
  EXPECT_EQ(0, memcmp(buffer_, data, sizeof(data)));
}

// 複数回のデータ追加: 連続してデータを追加した場合に累積されることを確認
TEST_F(StreamRecBufferTest, PushMultipleData) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data1[] = {0x01, 0x02, 0x03};
  uint8_t data2[] = {0x04, 0x05, 0x06, 0x07};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1));
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data2, sizeof(data2));
  EXPECT_EQ(7, rec_buffer_.size);
}

// オーバーフロー防止: キャパシティを超えるデータ追加はエラーを返し、バッファは変更されない
TEST_F(StreamRecBufferTest, PushOverflow) {
  uint8_t small_buffer[8];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));
  uint8_t data[10] = {0};
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data)));
  EXPECT_EQ(0, rec_buffer_.size);
}

// 部分的なデータ削除: 先頭から指定バイト数を削除し、残りのデータが前に詰められることを確認
TEST_F(StreamRecBufferTest, DropPartialData) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 2);
  EXPECT_EQ(3, rec_buffer_.size);
  EXPECT_EQ(0x03, buffer_[0]);
}

// 全データ削除: 全データを削除するとサイズが 0 になることを確認
TEST_F(StreamRecBufferTest, DropAllData) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, sizeof(data));
  EXPECT_EQ(0, rec_buffer_.size);
}

// 未処理サイズ取得（初期状態）: 追加直後は全データが未処理
TEST_F(StreamRecBufferTest, GetUnprocessedSizeInitial) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  EXPECT_EQ(5, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// 未処理サイズ取得（確定後）: フレーム長を確定すると未処理サイズが減少
TEST_F(StreamRecBufferTest, GetUnprocessedSizeAfterConfirm) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(2, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// フレーム確定フロー: EB90 フレーム形式のデータを受信し、フレーム全体を確定
TEST_F(StreamRecBufferTest, FrameConfirmationFlow) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0xEB, 0x90, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04, 0xAA, 0xBB, 0xC5, 0x79};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  EXPECT_EQ(12, rec_buffer_.size);

  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 12);
  rec_buffer_.is_frame_fixed = 1;
  EXPECT_EQ(12, rec_buffer_.confirmed_frame_len);
  EXPECT_EQ(0, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// 複数フレーム処理: 1つ目のフレーム処理後、フレームヘッド候補を進めて次のフレームを処理
TEST_F(StreamRecBufferTest, MultipleFrameProcessing) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // 最初の 4 バイトをフレームとして確定
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(4, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));

  // フレームヘッド候補を次のフレーム位置に移動
  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(4, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);  // 確定長はリセットされる
  EXPECT_EQ(4, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// ================================================================
// エッジケーステスト
// ================================================================

// バッファ容量ぴったりまで push: 容量いっぱいまでデータを追加しても正常動作
TEST_F(StreamRecBufferTest, PushExactCapacity) {
  uint8_t small_buffer[8];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));
  uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data)));
  EXPECT_EQ(8, rec_buffer_.size);
  EXPECT_EQ(0, memcmp(small_buffer, data, sizeof(data)));
}

// 容量いっぱいの状態で追加 push はエラー
TEST_F(StreamRecBufferTest, PushWhenFull) {
  uint8_t small_buffer[4];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));
  uint8_t data1[4] = {0x01, 0x02, 0x03, 0x04};
  uint8_t data2[1] = {0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1));
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data2, sizeof(data2)));
  EXPECT_EQ(4, rec_buffer_.size);  // 元のサイズを維持
}

// drop 後の pos_of_frame_head_candidate 調整: drop サイズより大きい場合は差分を保持
TEST_F(StreamRecBufferTest, DropAdjustsFrameHeadCandidate) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // フレームヘッド候補を位置 4 に設定
  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(4, rec_buffer_.pos_of_frame_head_candidate);

  // 2 バイト drop すると、フレームヘッド候補は 4 - 2 = 2 に調整される
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 2);
  EXPECT_EQ(2, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(6, rec_buffer_.size);
}

// drop サイズがフレームヘッド候補以上の場合は 0 にリセット
TEST_F(StreamRecBufferTest, DropResetsFrameHeadCandidateToZero) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // フレームヘッド候補を位置 2 に設定
  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 2);

  // 4 バイト drop（フレームヘッド候補 2 より大きい）
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(0, rec_buffer_.pos_of_frame_head_candidate);  // 0 にリセット
  EXPECT_EQ(2, rec_buffer_.size);
}

// drop 後の pos_of_last_rec 調整
TEST_F(StreamRecBufferTest, DropAdjustsPosOfLastRec) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data1[] = {0x01, 0x02, 0x03, 0x04};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1));
  rec_buffer_.pos_of_last_rec = 4;  // 最終受信位置を設定

  // 2 バイト drop
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 2);
  EXPECT_EQ(2, rec_buffer_.pos_of_last_rec);  // 4 - 2 = 2
}

// drop サイズが pos_of_last_rec 以上の場合は 0 にリセット
TEST_F(StreamRecBufferTest, DropResetsPosOfLastRecToZero) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  rec_buffer_.pos_of_last_rec = 2;

  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 3);  // pos_of_last_rec(2) より大きい
  EXPECT_EQ(0, rec_buffer_.pos_of_last_rec);
}

// 空バッファへの drop は何も起きない
TEST_F(StreamRecBufferTest, DropFromEmptyBuffer) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 10);
  EXPECT_EQ(0, rec_buffer_.size);
  EXPECT_EQ(0, rec_buffer_.pos_of_frame_head_candidate);
}

// サイズ 0 の drop は何も起きない
TEST_F(StreamRecBufferTest, DropZeroSize) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 0);
  EXPECT_EQ(3, rec_buffer_.size);
}

// サイズを超える drop は clear と同等
TEST_F(StreamRecBufferTest, DropMoreThanSize) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  rec_buffer_.pos_of_frame_head_candidate = 1;
  rec_buffer_.confirmed_frame_len = 2;

  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 100);  // サイズより大きい
  EXPECT_EQ(0, rec_buffer_.size);
  EXPECT_EQ(0, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);
}

// move_forward がバッファサイズを超えた場合はサイズでクランプ
TEST_F(StreamRecBufferTest, MoveForwardBeyondSize) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 100);  // サイズ 4 を超える
  EXPECT_EQ(4, rec_buffer_.pos_of_frame_head_candidate);  // サイズでクランプ
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);  // リセット
}

// move_forward で confirmed_frame_len がリセットされることを確認
TEST_F(StreamRecBufferTest, MoveForwardResetsConfirmedFrameLen) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(3, rec_buffer_.confirmed_frame_len);

  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);  // リセットされる
}

// 連続した push と drop の組み合わせ: バッファが正しく管理される
TEST_F(StreamRecBufferTest, SequentialPushAndDrop) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // 最初の push
  uint8_t data1[] = {0x01, 0x02, 0x03, 0x04};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1));
  EXPECT_EQ(4, rec_buffer_.size);

  // 部分 drop
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 2);
  EXPECT_EQ(2, rec_buffer_.size);
  EXPECT_EQ(0x03, buffer_[0]);  // データが前詰めされている

  // 追加 push
  uint8_t data2[] = {0x05, 0x06};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data2, sizeof(data2));
  EXPECT_EQ(4, rec_buffer_.size);
  EXPECT_EQ(0x03, buffer_[0]);
  EXPECT_EQ(0x04, buffer_[1]);
  EXPECT_EQ(0x05, buffer_[2]);
  EXPECT_EQ(0x06, buffer_[3]);
}

// 未処理サイズが 0 になるケース: 全データが確定済み
TEST_F(StreamRecBufferTest, UnprocessedSizeZeroWhenFullyProcessed) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // pos_of_frame_head_candidate + confirmed_frame_len >= size の場合
  rec_buffer_.pos_of_frame_head_candidate = 2;
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 3);
  // 2 + 3 = 5 >= 5 なので未処理は 0
  EXPECT_EQ(0, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// NULL ポインタへの操作は安全にスキップ
TEST_F(StreamRecBufferTest, NullPointerSafety) {
  // これらの関数は NULL チェックで早期リターンする
  CDS_clear_stream_rec_buffer_(nullptr);
  CDS_drop_from_stream_rec_buffer_(nullptr, 10);
  CDS_confirm_stream_rec_buffer_(nullptr, 10);
  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(nullptr, 10);
  EXPECT_EQ(0, CDS_get_unprocessed_size_from_stream_rec_buffer_(nullptr));
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(nullptr, buffer_, 10));
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, nullptr, 10));
}

// サイズ 0 の push は OK を返すが何も追加しない
TEST_F(StreamRecBufferTest, PushZeroSize) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, 0));
  EXPECT_EQ(0, rec_buffer_.size);
}

// ================================================================
// パフォーマンス・メモリ効率テスト
// ================================================================

// memmove の正確性: drop 後のデータ内容が正しく前詰めされていること
TEST_F(StreamRecBufferTest, MemmoveDataIntegrity) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // 先頭 3 バイトを削除
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 3);

  // 残りのデータが正しく前詰めされている
  EXPECT_EQ(5, rec_buffer_.size);
  EXPECT_EQ(0xDD, buffer_[0]);
  EXPECT_EQ(0xEE, buffer_[1]);
  EXPECT_EQ(0xFF, buffer_[2]);
  EXPECT_EQ(0x11, buffer_[3]);
  EXPECT_EQ(0x22, buffer_[4]);
}

// 繰り返し drop と push: バッファ再利用パターンの検証
TEST_F(StreamRecBufferTest, RepeatedDropAndPush) {
  uint8_t small_buffer[16];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));

  // 複数回のフレーム処理をシミュレート
  for (int i = 0; i < 10; i++) {
    uint8_t frame[] = {(uint8_t)(0x10 + i), (uint8_t)(0x20 + i), (uint8_t)(0x30 + i), (uint8_t)(0x40 + i)};
    EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, frame, sizeof(frame)));
    EXPECT_EQ(4, rec_buffer_.size);

    // フレーム処理完了後に削除
    CDS_drop_from_stream_rec_buffer_(&rec_buffer_, sizeof(frame));
    EXPECT_EQ(0, rec_buffer_.size);
  }
}

// 複数フレーム一括受信のシミュレーション
TEST_F(StreamRecBufferTest, MultipleFramesInBuffer) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // 3 つのフレームを一度に受信（各フレーム 4 バイト）
  uint8_t frames[] = {
    0x01, 0x02, 0x03, 0x04,  // frame 1
    0x11, 0x12, 0x13, 0x14,  // frame 2
    0x21, 0x22, 0x23, 0x24   // frame 3
  };
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, frames, sizeof(frames));
  EXPECT_EQ(12, rec_buffer_.size);

  // フレーム 1 を処理
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(8, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));

  // フレーム 1 を削除して次へ
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(8, rec_buffer_.size);
  EXPECT_EQ(0x11, buffer_[0]);  // frame 2 の先頭

  // フレーム 2 を処理
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 4);
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(4, rec_buffer_.size);
  EXPECT_EQ(0x21, buffer_[0]);  // frame 3 の先頭

  // フレーム 3 を処理
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 4);
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(0, rec_buffer_.size);
}

// フレーム分割受信: 1 フレームが複数回の push で完成
TEST_F(StreamRecBufferTest, FragmentedFrameReception) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // フレームの前半を受信
  uint8_t part1[] = {0xEB, 0x90, 0x00};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, part1, sizeof(part1));
  EXPECT_EQ(3, rec_buffer_.size);

  // フレームの後半を受信
  uint8_t part2[] = {0x04, 0x01, 0x02, 0x03, 0x04};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, part2, sizeof(part2));
  EXPECT_EQ(8, rec_buffer_.size);

  // フレーム全体が正しく連結されている
  EXPECT_EQ(0xEB, buffer_[0]);
  EXPECT_EQ(0x90, buffer_[1]);
  EXPECT_EQ(0x00, buffer_[2]);
  EXPECT_EQ(0x04, buffer_[3]);
}

// バッファの境界条件: 容量-1 まで使用後、1 バイト追加
TEST_F(StreamRecBufferTest, BoundaryConditionCapacityMinusOne) {
  uint8_t small_buffer[8];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));

  // 7 バイトを追加
  uint8_t data7[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data7, sizeof(data7)));
  EXPECT_EQ(7, rec_buffer_.size);

  // 1 バイト追加（容量ぴったり）
  uint8_t data1[1] = {0x08};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1)));
  EXPECT_EQ(8, rec_buffer_.size);

  // さらに 1 バイト追加（オーバーフロー）
  uint8_t data_over[1] = {0x09};
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data_over, sizeof(data_over)));
  EXPECT_EQ(8, rec_buffer_.size);  // サイズ変更なし
}

// フレームヘッド候補と確定長の連携: 複数フレーム処理時の状態管理
TEST_F(StreamRecBufferTest, FrameHeadCandidateWithConfirmedLen) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // 最初のフレーム（3 バイト）を部分的に確定
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 2);
  EXPECT_EQ(8, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));  // 10 - 0 - 2 = 8

  // さらに確定を進める
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(7, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));  // 10 - 0 - 3 = 7

  // フレームヘッド候補を移動
  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(3, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);
  EXPECT_EQ(7, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));  // 10 - 3 - 0 = 7
}

// clear 後の完全リセット確認
TEST_F(StreamRecBufferTest, ClearResetsAllFields) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // 各フィールドを設定
  rec_buffer_.pos_of_frame_head_candidate = 2;
  rec_buffer_.confirmed_frame_len = 3;
  rec_buffer_.is_frame_fixed = 1;
  rec_buffer_.pos_of_last_rec = 5;

  // clear
  CDS_clear_stream_rec_buffer_(&rec_buffer_);

  // 全フィールドがリセット
  EXPECT_EQ(0, rec_buffer_.size);
  EXPECT_EQ(0, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);
  EXPECT_EQ(0, rec_buffer_.is_frame_fixed);
  EXPECT_EQ(0, rec_buffer_.pos_of_last_rec);

  // バッファ内容もゼロクリア
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ(0x00, buffer_[i]);
  }
}

// 大きなデータの処理: バッファ容量の半分以上を使用
TEST_F(StreamRecBufferTest, LargeDataProcessing) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // 200 バイトのデータを作成
  uint8_t large_data[200];
  for (int i = 0; i < 200; i++) {
    large_data[i] = (uint8_t)(i & 0xFF);
  }

  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, large_data, sizeof(large_data)));
  EXPECT_EQ(200, rec_buffer_.size);

  // 先頭 100 バイトを削除
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 100);
  EXPECT_EQ(100, rec_buffer_.size);

  // 残りのデータが正しい（元の 100 番目からのデータ）
  EXPECT_EQ(100, buffer_[0]);
  EXPECT_EQ(101, buffer_[1]);
  EXPECT_EQ(199, buffer_[99]);
}

// ================================================================
// 境界値・ストレステスト
// ================================================================

// 1 バイトバッファ: 最小サイズでの動作確認
TEST_F(StreamRecBufferTest, MinimalBufferSize) {
  uint8_t tiny_buffer[1];
  CDS_init_stream_rec_buffer(&rec_buffer_, tiny_buffer, sizeof(tiny_buffer));

  uint8_t data = 0xAA;
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, &data, 1));
  EXPECT_EQ(1, rec_buffer_.size);
  EXPECT_EQ(0xAA, tiny_buffer[0]);

  // 追加は失敗
  uint8_t data2 = 0xBB;
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, &data2, 1));

  // 削除で空に
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 1);
  EXPECT_EQ(0, rec_buffer_.size);

  // 再度追加可能
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, &data2, 1));
  EXPECT_EQ(0xBB, tiny_buffer[0]);
}

// 連続した 1 バイト push: 効率は悪いが正しく動作する
TEST_F(StreamRecBufferTest, SingleBytePushes) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // 100 回 1 バイトずつ push
  for (int i = 0; i < 100; i++) {
    uint8_t byte = (uint8_t)i;
    EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, &byte, 1));
  }

  EXPECT_EQ(100, rec_buffer_.size);

  // データが正しい順序で格納されている
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ((uint8_t)i, buffer_[i]);
  }
}

// 連続した 1 バイト drop: 逐次削除の動作確認
TEST_F(StreamRecBufferTest, SingleByteDrops) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  uint8_t data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // 1 バイトずつ削除
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(10 - i, rec_buffer_.size);
    if (rec_buffer_.size > 0) {
      EXPECT_EQ((uint8_t)i, buffer_[0]);  // 先頭は常に削除された次の値
    }
    CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 1);
  }

  EXPECT_EQ(0, rec_buffer_.size);
}

// フレーム候補位置の連続移動: ヘッダ探索のシミュレーション
TEST_F(StreamRecBufferTest, ContinuousFrameHeadSearch) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // ノイズ + 有効フレーム
  uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xEB, 0x90, 0x00, 0x01, 0xAA};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // ヘッダ (0xEB) を探索するシミュレーション
  // 位置 0, 1, 2 は不一致、位置 3 で発見
  for (int i = 0; i < 3; i++) {
    CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 1);
  }

  EXPECT_EQ(3, rec_buffer_.pos_of_frame_head_candidate);

  // ヘッダ発見後、フレームを確定
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 5);  // EB 90 00 01 AA
  EXPECT_EQ(5, rec_buffer_.confirmed_frame_len);
  EXPECT_EQ(0, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// バッファ内の特定パターン検索シミュレーション
TEST_F(StreamRecBufferTest, PatternSearchSimulation) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // パターン: AA BB CC を探す
  uint8_t data[] = {0x11, 0xAA, 0x22, 0xAA, 0xBB, 0x33, 0xAA, 0xBB, 0xCC, 0xDD};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  // 手動でパターン検索（実際のフレーム解析では CDS_analyze_rx_buffer_* が行う）
  int pattern_start = -1;
  for (int i = 0; i <= rec_buffer_.size - 3; i++) {
    if (buffer_[i] == 0xAA && buffer_[i+1] == 0xBB && buffer_[i+2] == 0xCC) {
      pattern_start = i;
      break;
    }
  }

  EXPECT_EQ(6, pattern_start);  // 位置 6 で発見
}

// 複数回の init: 再初期化が正しく動作する
TEST_F(StreamRecBufferTest, ReInitialization) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  rec_buffer_.pos_of_frame_head_candidate = 1;
  rec_buffer_.confirmed_frame_len = 2;

  // 別のバッファで再初期化
  uint8_t new_buffer[64];
  CDS_init_stream_rec_buffer(&rec_buffer_, new_buffer, sizeof(new_buffer));

  // 全てリセットされている
  EXPECT_EQ(new_buffer, rec_buffer_.buffer);
  EXPECT_EQ(64, rec_buffer_.capacity);
  EXPECT_EQ(0, rec_buffer_.size);
  EXPECT_EQ(0, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);
}

// 交互の push と confirm: フレーム処理中の追加受信シミュレーション
TEST_F(StreamRecBufferTest, InterleavedPushAndConfirm) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);

  // フレーム前半を受信
  uint8_t part1[] = {0xEB, 0x90};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, part1, sizeof(part1));
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 2);  // ヘッダ部分を確定

  // フレーム後半を受信
  uint8_t part2[] = {0x00, 0x02, 0x11, 0x22};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, part2, sizeof(part2));

  EXPECT_EQ(6, rec_buffer_.size);
  EXPECT_EQ(4, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));  // 6 - 0 - 2 = 4

  // フレーム全体を確定
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 6);
  EXPECT_EQ(0, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

// drop 直後の push: メモリ再利用パターン
TEST_F(StreamRecBufferTest, ImmediatePushAfterDrop) {
  uint8_t small_buffer[8];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));

  // バッファを満杯に
  uint8_t data1[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1));

  // 4 バイト削除して即座に 4 バイト追加
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 4);
  uint8_t data2[4] = {0xAA, 0xBB, 0xCC, 0xDD};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data2, sizeof(data2)));

  EXPECT_EQ(8, rec_buffer_.size);
  // 0x55, 0x66, 0x77, 0x88, 0xAA, 0xBB, 0xCC, 0xDD
  EXPECT_EQ(0x55, small_buffer[0]);
  EXPECT_EQ(0xAA, small_buffer[4]);
}

// 確定長の上書き: 再確定が前の値を上書きする
TEST_F(StreamRecBufferTest, ConfirmOverwrite) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(3, rec_buffer_.confirmed_frame_len);

  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 5);
  EXPECT_EQ(5, rec_buffer_.confirmed_frame_len);

  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 2);  // 小さい値に上書きも可能
  EXPECT_EQ(2, rec_buffer_.confirmed_frame_len);
}

// バッファ容量ちょうどの連続フレーム: 余裕なしでの処理
TEST_F(StreamRecBufferTest, ExactCapacityFrames) {
  uint8_t small_buffer[12];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));

  // 12 バイト = 4 バイトフレーム x 3
  uint8_t frames[] = {
    0x01, 0x02, 0x03, 0x04,
    0x11, 0x12, 0x13, 0x14,
    0x21, 0x22, 0x23, 0x24
  };
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, frames, sizeof(frames)));

  // フレーム 1 処理後、新しいデータは入らない（容量いっぱい）
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 4);
  uint8_t new_data[1] = {0xFF};
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, new_data, 1));

  // フレーム 1 を削除すれば追加可能
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, new_data, 1));
}
