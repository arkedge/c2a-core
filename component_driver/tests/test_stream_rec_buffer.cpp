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

TEST_F(StreamRecBufferTest, InitSuccess) {
  CDS_ERR_CODE ret = CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  EXPECT_EQ(CDS_ERR_CODE_OK, ret);
  EXPECT_EQ(buffer_, rec_buffer_.buffer);
  EXPECT_EQ(BUFFER_SIZE, rec_buffer_.capacity);
  EXPECT_EQ(0, rec_buffer_.size);
}

TEST_F(StreamRecBufferTest, InitWithNullBuffer) {
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_init_stream_rec_buffer(&rec_buffer_, nullptr, BUFFER_SIZE));
}

TEST_F(StreamRecBufferTest, InitWithNullRecBuffer) {
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_init_stream_rec_buffer(nullptr, buffer_, BUFFER_SIZE));
}

TEST_F(StreamRecBufferTest, InitWithZeroCapacity) {
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, 0));
}

TEST_F(StreamRecBufferTest, PushDataSuccess) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  EXPECT_EQ(CDS_ERR_CODE_OK, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data)));
  EXPECT_EQ(5, rec_buffer_.size);
  EXPECT_EQ(0, memcmp(buffer_, data, sizeof(data)));
}

TEST_F(StreamRecBufferTest, PushMultipleData) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data1[] = {0x01, 0x02, 0x03};
  uint8_t data2[] = {0x04, 0x05, 0x06, 0x07};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data1, sizeof(data1));
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data2, sizeof(data2));
  EXPECT_EQ(7, rec_buffer_.size);
}

TEST_F(StreamRecBufferTest, PushOverflow) {
  uint8_t small_buffer[8];
  CDS_init_stream_rec_buffer(&rec_buffer_, small_buffer, sizeof(small_buffer));
  uint8_t data[10] = {0};
  EXPECT_EQ(CDS_ERR_CODE_ERR, CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data)));
  EXPECT_EQ(0, rec_buffer_.size);
}

TEST_F(StreamRecBufferTest, DropPartialData) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, 2);
  EXPECT_EQ(3, rec_buffer_.size);
  EXPECT_EQ(0x03, buffer_[0]);
}

TEST_F(StreamRecBufferTest, DropAllData) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, sizeof(data));
  EXPECT_EQ(0, rec_buffer_.size);
}

TEST_F(StreamRecBufferTest, GetUnprocessedSizeInitial) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  EXPECT_EQ(5, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

TEST_F(StreamRecBufferTest, GetUnprocessedSizeAfterConfirm) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));
  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 3);
  EXPECT_EQ(2, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}

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

TEST_F(StreamRecBufferTest, MultipleFrameProcessing) {
  CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
  uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, sizeof(data));

  CDS_confirm_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(4, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));

  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(&rec_buffer_, 4);
  EXPECT_EQ(4, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(0, rec_buffer_.confirmed_frame_len);
  EXPECT_EQ(4, CDS_get_unprocessed_size_from_stream_rec_buffer_(&rec_buffer_));
}
