/**
 * @file test_frame_analysis.cpp
 * @brief CDS_analyze_rx_buffer_* 関連関数のユニットテスト
 *
 * フレーム解析機能のテスト:
 * - 固定長フレーム（ヘッダ有/無、フッタ有/無）
 * - 可変長フレーム（フレーム長データ付き）
 * - 可変長フレーム（フッタ終端）
 * - 複数フレーム連続受信
 * - 厳格なフレーム探索モード
 * - ノイズ混入時のリカバリー
 */
#include <gtest/gtest.h>
#include <cstring>

extern "C" {

#include "mocks/mock_hal_handler_registry.h"
#include "mocks/mock_time_manager.h"
#include "mocks/mock_ccp.h"

// ================================================================
// 型定義
// ================================================================

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

typedef enum
{
  CDS_STREAM_REC_STATUS_FINDING_HEADER,
  CDS_STREAM_REC_STATUS_RECEIVING_HEADER,
  CDS_STREAM_REC_STATUS_RECEIVING_FRAMELENGTH,
  CDS_STREAM_REC_STATUS_RECEIVING_DATA,
  CDS_STREAM_REC_STATUS_RECEIVING_FOOTER,
  CDS_STREAM_REC_STATUS_FIXED_FRAME,
  CDS_STREAM_REC_STATUS_DISABLE,
  CDS_STREAM_REC_STATUS_HEADER_MISMATCH,
  CDS_STREAM_REC_STATUS_FOOTER_MISMATCH,
  CDS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG,
  CDS_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT,
  CDS_STREAM_REC_STATUS_RX_ERR,
  CDS_STREAM_REC_STATUS_VALIDATE_ERR,
  CDS_STREAM_REC_STATUS_OTHER_ERR
} CDS_STREAM_REC_STATUS_CODE;

typedef struct
{
  CDS_STREAM_REC_STATUS_CODE status_code;
  uint16_t                   fixed_frame_len;
  uint8_t                    tlm_disruption_status;
  uint32_t                   count_of_carry_over_failures;
} CDS_StreamRecStatus;

// CDS_StreamConfig の簡易版（テストに必要な部分のみ）
typedef struct CDS_StreamConfig {
  struct {
    uint8_t  is_enabled_;
    uint8_t  is_strict_frame_search_;
    CDS_StreamRecBuffer* rx_buffer_;
    const uint8_t* rx_header_;
    uint16_t rx_header_size_;
    const uint8_t* rx_footer_;
    uint16_t rx_footer_size_;
    int16_t  rx_frame_size_;
    uint16_t max_rx_frame_size_;
    int16_t  rx_framelength_pos_;
    uint16_t rx_framelength_type_size_;
    uint16_t rx_framelength_offset_;
    ENDIAN_TYPE rx_framelength_endian_;
  } settings;

  struct {
    CDS_StreamRecStatus rec_status_;
  } info;
} CDS_StreamConfig;

// ================================================================
// StreamRecBuffer 関数（driver_super.c から抽出）
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
  stream_rec_buffer->size = 0;
  stream_rec_buffer->pos_of_frame_head_candidate = 0;
  stream_rec_buffer->confirmed_frame_len = 0;
  stream_rec_buffer->is_frame_fixed = 0;
  stream_rec_buffer->pos_of_last_rec = 0;
  memset(buffer, 0x00, buffer_capacity);
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

// ================================================================
// フレーム解析関数（driver_super.c から抽出）
// ================================================================

// ヘッダ探索: ヘッダの先頭バイトを検索
static void CDS_analyze_rx_buffer_finding_header_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamConfig* p = p_stream_config;
  CDS_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t unprocessed_data_len = CDS_get_unprocessed_size_from_stream_rec_buffer_(buffer);
  uint8_t* p_header;
  uint16_t found_header_offset;

  if (p_stream_config->settings.rx_header_ == NULL)
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_OTHER_ERR;
    CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, unprocessed_data_len);
    return;
  }

  p_header = (uint8_t*)memchr(&buffer->buffer[buffer->pos_of_frame_head_candidate],
                              (int)(p->settings.rx_header_[0]),
                              (size_t)unprocessed_data_len);

  if (p_header == NULL)
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FINDING_HEADER;
    CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, unprocessed_data_len);
    return;
  }

  found_header_offset = (uint16_t)(p_header - &buffer->buffer[buffer->pos_of_frame_head_candidate]);
  CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, found_header_offset);
  CDS_confirm_stream_rec_buffer_(buffer, 1);
  p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_HEADER;
  return;
}

// ヘッダ受信中: 1 バイトずつヘッダを検証
static void CDS_analyze_rx_buffer_receiving_header_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamConfig* p = p_stream_config;
  CDS_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t buffer_offset = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;

  if (buffer->buffer[buffer_offset] == p->settings.rx_header_[buffer->confirmed_frame_len])
  {
    CDS_confirm_stream_rec_buffer_(buffer, buffer->confirmed_frame_len + 1);
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_HEADER;
    return;
  }
  else
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_HEADER_MISMATCH;
    return;
  }
}

// フッタ受信中: 1 バイトずつフッタを検証
static void CDS_analyze_rx_buffer_receiving_footer_(CDS_StreamConfig* p_stream_config,
                                                    uint16_t rx_frame_size)
{
  CDS_StreamConfig* p = p_stream_config;
  CDS_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t buffer_offset = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;
  uint16_t rec_footer_pos;

  if (p->settings.rx_footer_size_ == 0)
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FIXED_FRAME;
    return;
  }

  rec_footer_pos = (uint16_t)(buffer->confirmed_frame_len - (rx_frame_size - p->settings.rx_footer_size_));

  if (buffer->buffer[buffer_offset] != p->settings.rx_footer_[rec_footer_pos])
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FOOTER_MISMATCH;
    return;
  }

  CDS_confirm_stream_rec_buffer_(buffer, buffer->confirmed_frame_len + 1);

  if (buffer->confirmed_frame_len == rx_frame_size)
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FIXED_FRAME;
  }
  else
  {
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_FOOTER;
  }

  return;
}

// フレーム長データ取得（可変長フレーム用）
static uint32_t CDS_analyze_rx_buffer_get_framelength_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamConfig* p = p_stream_config;
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

// 固定長フレーム解析
static void CDS_analyze_rx_buffer_fixed_pickup_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamConfig* p = p_stream_config;
  CDS_StreamRecBuffer* buffer = p->settings.rx_buffer_;

  if (buffer->confirmed_frame_len == 0 && p->settings.rx_header_size_ != 0)
  {
    CDS_analyze_rx_buffer_finding_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_header_size_)
  {
    CDS_analyze_rx_buffer_receiving_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_frame_size_ - p->settings.rx_footer_size_)
  {
    const uint16_t unprocessed_data_len = CDS_get_unprocessed_size_from_stream_rec_buffer_(buffer);
    uint16_t pickup_data_len = (uint16_t)(p->settings.rx_frame_size_ - p->settings.rx_footer_size_ - buffer->confirmed_frame_len);

    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    CDS_confirm_stream_rec_buffer_(buffer, buffer->confirmed_frame_len + pickup_data_len);
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_DATA;

    if (p->settings.rx_footer_size_ == 0 && buffer->confirmed_frame_len == p->settings.rx_frame_size_)
    {
      p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FIXED_FRAME;
    }

    return;
  }
  else
  {
    CDS_analyze_rx_buffer_receiving_footer_(p_stream_config,
                                            (uint16_t)(p->settings.rx_frame_size_));
    return;
  }
}

// 可変長フレーム解析（フレーム長データ付き）
static void CDS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamConfig* p = p_stream_config;
  CDS_StreamRecBuffer* buffer = p->settings.rx_buffer_;
  const uint16_t unprocessed_data_len = CDS_get_unprocessed_size_from_stream_rec_buffer_(buffer);
  uint32_t rx_frame_size = CDS_analyze_rx_buffer_get_framelength_(p_stream_config);

  if (buffer->confirmed_frame_len == 0 && p->settings.rx_header_size_ != 0)
  {
    CDS_analyze_rx_buffer_finding_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_header_size_)
  {
    CDS_analyze_rx_buffer_receiving_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_framelength_pos_ + p->settings.rx_framelength_type_size_)
  {
    uint16_t pickup_data_len = (uint16_t)(p->settings.rx_framelength_pos_ + p->settings.rx_framelength_type_size_ - buffer->confirmed_frame_len);

    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    CDS_confirm_stream_rec_buffer_(buffer, buffer->confirmed_frame_len + pickup_data_len);
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_FRAMELENGTH;

    if (buffer->confirmed_frame_len >= p->settings.rx_framelength_pos_ + p->settings.rx_framelength_type_size_)
    {
      rx_frame_size = CDS_analyze_rx_buffer_get_framelength_(p_stream_config);

      if (rx_frame_size > buffer->capacity || rx_frame_size > p->settings.max_rx_frame_size_)
      {
        p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG;
        return;
      }

      if (rx_frame_size < p->settings.rx_header_size_ + p->settings.rx_footer_size_)
      {
        p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT;
        return;
      }
    }

    return;
  }
  else if (buffer->confirmed_frame_len < rx_frame_size - p->settings.rx_footer_size_)
  {
    uint16_t pickup_data_len = (uint16_t)(rx_frame_size - p->settings.rx_footer_size_ - buffer->confirmed_frame_len);

    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    CDS_confirm_stream_rec_buffer_(buffer, buffer->confirmed_frame_len + pickup_data_len);
    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_DATA;

    if (p->settings.rx_footer_size_ == 0 && buffer->confirmed_frame_len == rx_frame_size)
    {
      p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FIXED_FRAME;
    }

    return;
  }
  else
  {
    CDS_analyze_rx_buffer_receiving_footer_(p_stream_config,
                                            (uint16_t)rx_frame_size);
    return;
  }
}

// 可変長フレーム解析（フッタ終端）
static void CDS_analyze_rx_buffer_variable_pickup_with_footer_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamConfig* p = p_stream_config;
  CDS_StreamRecBuffer* buffer = p->settings.rx_buffer_;

  if (buffer->confirmed_frame_len == 0 && p->settings.rx_header_size_ != 0)
  {
    CDS_analyze_rx_buffer_finding_header_(p_stream_config);
    return;
  }
  else if (buffer->confirmed_frame_len < p->settings.rx_header_size_)
  {
    CDS_analyze_rx_buffer_receiving_header_(p_stream_config);
    return;
  }
  else
  {
    const uint16_t unprocessed_data_len = CDS_get_unprocessed_size_from_stream_rec_buffer_(buffer);
    uint8_t* p_footer_last;
    int32_t  body_data_len;
    uint16_t processed_data_len;
    uint16_t i;
    const uint16_t memchr_offset = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;
    uint16_t estimated_rx_frame_end_pos;

    p_footer_last = (uint8_t*)memchr(&(buffer->buffer[memchr_offset]),
                                     (int)(p->settings.rx_footer_[p->settings.rx_footer_size_ - 1]),
                                     (size_t)unprocessed_data_len);

    if (p_footer_last == NULL)
    {
      processed_data_len = unprocessed_data_len;
    }
    else
    {
      processed_data_len = (uint16_t)(p_footer_last - &(buffer->buffer[memchr_offset]) + 1);
    }

    CDS_confirm_stream_rec_buffer_(buffer, buffer->confirmed_frame_len + processed_data_len);
    if (buffer->confirmed_frame_len > p->settings.max_rx_frame_size_)
    {
      p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG;
      return;
    }

    if (p_footer_last == NULL)
    {
      p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_DATA;
      return;
    }

    body_data_len = buffer->confirmed_frame_len - p->settings.rx_header_size_ - p->settings.rx_footer_size_;
    if (body_data_len < 0)
    {
      p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_DATA;
      return;
    }

    estimated_rx_frame_end_pos = buffer->pos_of_frame_head_candidate + buffer->confirmed_frame_len;
    for (i = 0; i < p->settings.rx_footer_size_; i++)
    {
      if (buffer->buffer[estimated_rx_frame_end_pos - i - 1] != p->settings.rx_footer_[p->settings.rx_footer_size_ - i - 1])
      {
        p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_RECEIVING_DATA;
        return;
      }
    }

    p->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FIXED_FRAME;
    return;
  }
}

// メイン pickup 関数
static void CDS_analyze_rx_buffer_pickup_(CDS_StreamConfig* p_stream_config)
{
  CDS_StreamRecBuffer* buffer = p_stream_config->settings.rx_buffer_;
  void (*pickup_func)(CDS_StreamConfig* p_stream_config);

  if (p_stream_config->settings.rx_frame_size_ == 0) return;

  if (p_stream_config->settings.rx_frame_size_ > 0)
  {
    pickup_func = CDS_analyze_rx_buffer_fixed_pickup_;
  }
  else if (p_stream_config->settings.rx_frame_size_ < 0)
  {
    if (p_stream_config->settings.rx_framelength_pos_ >= 0)
    {
      pickup_func = CDS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_;
    }
    else
    {
      pickup_func = CDS_analyze_rx_buffer_variable_pickup_with_footer_;
    }
  }
  else
  {
    pickup_func = NULL;
    return;
  }

  while (CDS_get_unprocessed_size_from_stream_rec_buffer_(buffer) > 0)
  {
    pickup_func(p_stream_config);

    if (p_stream_config->info.rec_status_.status_code == CDS_STREAM_REC_STATUS_FIXED_FRAME)
    {
      break;
    }

    if (p_stream_config->info.rec_status_.status_code == CDS_STREAM_REC_STATUS_HEADER_MISMATCH ||
        p_stream_config->info.rec_status_.status_code == CDS_STREAM_REC_STATUS_FOOTER_MISMATCH ||
        p_stream_config->info.rec_status_.status_code == CDS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG ||
        p_stream_config->info.rec_status_.status_code == CDS_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT)
    {
      CDS_move_forward_frame_head_candidate_of_stream_rec_buffer_(buffer, 1);
      p_stream_config->info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FINDING_HEADER;
    }
  }

  if (p_stream_config->info.rec_status_.status_code == CDS_STREAM_REC_STATUS_FIXED_FRAME)
  {
    buffer->is_frame_fixed = 1;
    p_stream_config->info.rec_status_.fixed_frame_len = buffer->confirmed_frame_len;
  }

  return;
}

}  // extern "C"

// ================================================================
// テストフィクスチャ
// ================================================================

class FrameAnalysisTest : public ::testing::Test {
protected:
  static constexpr uint16_t BUFFER_SIZE = 256;
  uint8_t buffer_[BUFFER_SIZE];
  CDS_StreamRecBuffer rec_buffer_;
  CDS_StreamConfig stream_config_;

  void SetUp() override {
    memset(buffer_, 0, sizeof(buffer_));
    memset(&rec_buffer_, 0, sizeof(rec_buffer_));
    memset(&stream_config_, 0, sizeof(stream_config_));

    CDS_init_stream_rec_buffer(&rec_buffer_, buffer_, BUFFER_SIZE);
    stream_config_.settings.rx_buffer_ = &rec_buffer_;
    stream_config_.settings.max_rx_frame_size_ = 0xffff;
    stream_config_.info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FINDING_HEADER;
  }

  // ヘルパー: 固定長フレーム設定
  void SetupFixedFrame(const uint8_t* header, uint16_t header_size,
                       const uint8_t* footer, uint16_t footer_size,
                       int16_t frame_size) {
    stream_config_.settings.rx_header_ = header;
    stream_config_.settings.rx_header_size_ = header_size;
    stream_config_.settings.rx_footer_ = footer;
    stream_config_.settings.rx_footer_size_ = footer_size;
    stream_config_.settings.rx_frame_size_ = frame_size;
  }

  // ヘルパー: 可変長フレーム設定（フレーム長データ付き）
  void SetupVariableFrameWithLength(const uint8_t* header, uint16_t header_size,
                                    const uint8_t* footer, uint16_t footer_size,
                                    int16_t framelength_pos, uint16_t framelength_type_size,
                                    uint16_t framelength_offset, ENDIAN_TYPE endian) {
    stream_config_.settings.rx_header_ = header;
    stream_config_.settings.rx_header_size_ = header_size;
    stream_config_.settings.rx_footer_ = footer;
    stream_config_.settings.rx_footer_size_ = footer_size;
    stream_config_.settings.rx_frame_size_ = -1;  // 可変長
    stream_config_.settings.rx_framelength_pos_ = framelength_pos;
    stream_config_.settings.rx_framelength_type_size_ = framelength_type_size;
    stream_config_.settings.rx_framelength_offset_ = framelength_offset;
    stream_config_.settings.rx_framelength_endian_ = endian;
  }

  // ヘルパー: 可変長フレーム設定（フッタ終端）
  void SetupVariableFrameWithFooter(const uint8_t* header, uint16_t header_size,
                                    const uint8_t* footer, uint16_t footer_size) {
    stream_config_.settings.rx_header_ = header;
    stream_config_.settings.rx_header_size_ = header_size;
    stream_config_.settings.rx_footer_ = footer;
    stream_config_.settings.rx_footer_size_ = footer_size;
    stream_config_.settings.rx_frame_size_ = -1;  // 可変長
    stream_config_.settings.rx_framelength_pos_ = -1;  // フレーム長なし
  }

  // データを受信バッファに追加して解析実行
  void ReceiveAndAnalyze(const uint8_t* data, uint16_t size) {
    CDS_push_to_stream_rec_buffer_(&rec_buffer_, data, size);
    CDS_analyze_rx_buffer_pickup_(&stream_config_);
  }
};

// ================================================================
// 固定長フレームテスト
// ================================================================

// 固定長フレーム: ヘッダ有り、フッタ無し、一括受信でフレーム確定
TEST_F(FrameAnalysisTest, FixedFrameHeaderOnlyComplete) {
  const uint8_t header[] = {0xEB, 0x90};
  // フレーム: [EB 90] [DATA x 4] = 6 bytes
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(6, stream_config_.info.rec_status_.fixed_frame_len);
  EXPECT_EQ(1, rec_buffer_.is_frame_fixed);
}

// 固定長フレーム: ヘッダ有り、フッタ有り、一括受信でフレーム確定
TEST_F(FrameAnalysisTest, FixedFrameHeaderFooterComplete) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  // フレーム: [EB 90] [DATA x 4] [C5 79] = 8 bytes
  SetupFixedFrame(header, 2, footer, 2, 8);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0xC5, 0x79};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(8, stream_config_.info.rec_status_.fixed_frame_len);
}

// 固定長フレーム: ヘッダ無し、フッタ無し（データ先頭からフレーム開始）
TEST_F(FrameAnalysisTest, FixedFrameNoHeaderNoFooter) {
  // ヘッダ・フッタなしの 4 バイトフレーム
  SetupFixedFrame(nullptr, 0, nullptr, 0, 4);

  uint8_t frame[] = {0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(4, stream_config_.info.rec_status_.fixed_frame_len);
}

// 固定長フレーム: ヘッダ無し、フッタ有り
TEST_F(FrameAnalysisTest, FixedFrameNoHeaderWithFooter) {
  const uint8_t footer[] = {0xFF};
  // フレーム: [DATA x 3] [FF] = 4 bytes
  SetupFixedFrame(nullptr, 0, footer, 1, 4);

  uint8_t frame[] = {0x01, 0x02, 0x03, 0xFF};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(4, stream_config_.info.rec_status_.fixed_frame_len);
}

// 固定長フレーム: 分割受信（ヘッダ → データ → フッタ）
TEST_F(FrameAnalysisTest, FixedFrameFragmentedReception) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  SetupFixedFrame(header, 2, footer, 2, 8);

  // ヘッダ部分のみ受信: ヘッダ確定後、まだデータ未受信なので RECEIVING_HEADER
  uint8_t part1[] = {0xEB, 0x90};
  ReceiveAndAnalyze(part1, sizeof(part1));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_RECEIVING_HEADER, stream_config_.info.rec_status_.status_code);

  // データ部分受信: データを全て処理後は RECEIVING_DATA
  uint8_t part2[] = {0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(part2, sizeof(part2));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_RECEIVING_DATA, stream_config_.info.rec_status_.status_code);

  // フッタ部分受信
  uint8_t part3[] = {0xC5, 0x79};
  ReceiveAndAnalyze(part3, sizeof(part3));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(8, stream_config_.info.rec_status_.fixed_frame_len);
}

// 固定長フレーム: ノイズ後にフレーム受信（ヘッダ探索テスト）
TEST_F(FrameAnalysisTest, FixedFrameWithNoisePrefix) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // ノイズ + 有効フレーム
  uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(6, stream_config_.info.rec_status_.fixed_frame_len);
  // フレーム先頭はノイズの後（位置 3）
  EXPECT_EQ(3, rec_buffer_.pos_of_frame_head_candidate);
}

// 固定長フレーム: ヘッダ不一致からのリカバリー
TEST_F(FrameAnalysisTest, FixedFrameHeaderMismatchRecovery) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // 偽ヘッダ（0xEB だけ一致）+ 本物のフレーム
  uint8_t data[] = {0xEB, 0x00, 0xEB, 0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  // 位置 2 から本物のヘッダ
  EXPECT_EQ(2, rec_buffer_.pos_of_frame_head_candidate);
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
  ReceiveAndAnalyze(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  // 位置 8 から本物のフレーム
  EXPECT_EQ(8, rec_buffer_.pos_of_frame_head_candidate);
  EXPECT_EQ(8, stream_config_.info.rec_status_.fixed_frame_len);
}

// ================================================================
// 可変長フレームテスト（フレーム長データ付き）
// ================================================================

// 可変長フレーム: ビッグエンディアン、フレーム長 1 バイト
TEST_F(FrameAnalysisTest, VariableFrameBigEndian1Byte) {
  const uint8_t header[] = {0xEB, 0x90};
  // フォーマット: [EB 90] [LEN:1byte] [DATA...] = LEN bytes total
  // LEN は rx_framelength_offset_ で調整
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2,  // framelength_pos
                               1,  // framelength_type_size (1 byte)
                               0,  // offset
                               ENDIAN_TYPE_BIG);

  // フレーム: EB 90 08 01 02 03 04 05 = 8 bytes (LEN=8)
  uint8_t frame[] = {0xEB, 0x90, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(8, stream_config_.info.rec_status_.fixed_frame_len);
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
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(10, stream_config_.info.rec_status_.fixed_frame_len);
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
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(10, stream_config_.info.rec_status_.fixed_frame_len);
}

// 可変長フレーム: オフセット付き（ヘッダ・フッタ除くボディ長）
TEST_F(FrameAnalysisTest, VariableFrameWithOffset) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0xC5, 0x79};
  // LEN フィールドがボディ長（ヘッダ 2 + フッタ 2 を含まない）を示す場合
  // offset = header_size + footer_size = 4
  SetupVariableFrameWithLength(header, 2, footer, 2,
                               2,  // framelength_pos
                               1,  // framelength_type_size
                               4,  // offset (header + footer size)
                               ENDIAN_TYPE_BIG);

  // フレーム: EB 90 04 01 02 03 04 C5 79 = 9 bytes
  // LEN=4 だが、offset=4 なので total = 4 + 4 = 8... 違う
  // 実際は rx_framelength_offset_ は LEN に加算される
  // LEN=4 + offset=4 = 8 bytes total? いや違う
  // コードを見ると: len += p->settings.rx_framelength_offset_;
  // なので LEN フィールドの値 + offset = 実際のフレーム長
  // LEN=4 を示し、offset=5 なら total=9

  // 正しく: LEN=4 (body length), offset = header(2) + len_field(1) + footer(2) = 5
  // total frame size = 4 + 5 = 9
  stream_config_.settings.rx_framelength_offset_ = 5;  // header(2) + len(1) + footer(2)

  uint8_t frame[] = {0xEB, 0x90, 0x04, 0x01, 0x02, 0x03, 0x04, 0xC5, 0x79};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(9, stream_config_.info.rec_status_.fixed_frame_len);
}

// 可変長フレーム: フレーム長が大きすぎる場合のエラー
TEST_F(FrameAnalysisTest, VariableFrameTooLong) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2,  // framelength_pos
                               2,  // framelength_type_size
                               0,  // offset
                               ENDIAN_TYPE_BIG);
  stream_config_.settings.max_rx_frame_size_ = 100;  // 最大 100 バイト

  // LEN = 0x1000 = 4096 (max_rx_frame_size_ を超える)
  uint8_t frame[] = {0xEB, 0x90, 0x10, 0x00, 0x01, 0x02};
  ReceiveAndAnalyze(frame, sizeof(frame));

  // エラー後、ヘッダ探索に戻る
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, stream_config_.info.rec_status_.status_code);
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
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, stream_config_.info.rec_status_.status_code);
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
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(9, stream_config_.info.rec_status_.fixed_frame_len);
}

// 可変長フレーム（フッタ終端）: 単一バイトフッタ
TEST_F(FrameAnalysisTest, VariableFrameSingleByteFooter) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0A};  // LF only
  SetupVariableFrameWithFooter(header, 2, footer, 1);

  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x0A};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(6, stream_config_.info.rec_status_.fixed_frame_len);
}

// 可変長フレーム（フッタ終端）: 偽フッタのスキップ
TEST_F(FrameAnalysisTest, VariableFrameFalseFooterSkip) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0D, 0x0A};
  SetupVariableFrameWithFooter(header, 2, footer, 2);

  // データ中に 0x0A（フッタ末尾）があるが、0x0D が先行しないケース
  uint8_t frame[] = {0xEB, 0x90, 0x0A, 0x0A, 0x0A, 0x0D, 0x0A};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(7, stream_config_.info.rec_status_.fixed_frame_len);
}

// 可変長フレーム（フッタ終端）: 最大長超過エラー
TEST_F(FrameAnalysisTest, VariableFrameFooterTerminatedTooLong) {
  const uint8_t header[] = {0xEB, 0x90};
  const uint8_t footer[] = {0x0A};
  SetupVariableFrameWithFooter(header, 2, footer, 1);
  stream_config_.settings.max_rx_frame_size_ = 6;

  // 7 バイト（最大 6 を超過）
  uint8_t frame[] = {0xEB, 0x90, 0x01, 0x02, 0x03, 0x04, 0x0A};
  ReceiveAndAnalyze(frame, sizeof(frame));

  // max_rx_frame_size_ 超過でエラー → ヘッダ探索に戻る
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, stream_config_.info.rec_status_.status_code);
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
  ReceiveAndAnalyze(data, sizeof(data));

  // 最初のフレームのみ確定
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(6, stream_config_.info.rec_status_.fixed_frame_len);
  EXPECT_EQ(0, rec_buffer_.pos_of_frame_head_candidate);

  // 次のフレームはまだバッファに残っている
  EXPECT_EQ(12, rec_buffer_.size);
}

// 次のフレーム処理のシミュレーション
TEST_F(FrameAnalysisTest, ProcessSecondFrame) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  uint8_t data[] = {
    0xEB, 0x90, 0x01, 0x02, 0x03, 0x04,
    0xEB, 0x90, 0x11, 0x12, 0x13, 0x14
  };
  ReceiveAndAnalyze(data, sizeof(data));

  // 最初のフレーム確定
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);

  // フレーム処理完了をシミュレート: is_frame_fixed をクリアし、バッファを頭出し
  rec_buffer_.is_frame_fixed = 0;
  CDS_drop_from_stream_rec_buffer_(&rec_buffer_, rec_buffer_.confirmed_frame_len);
  rec_buffer_.confirmed_frame_len = 0;
  stream_config_.info.rec_status_.status_code = CDS_STREAM_REC_STATUS_FINDING_HEADER;

  // 再度解析
  CDS_analyze_rx_buffer_pickup_(&stream_config_);

  // 2 つ目のフレーム確定
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(6, stream_config_.info.rec_status_.fixed_frame_len);
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

  for (int i = 0; i < 7; i++) {
    ReceiveAndAnalyze(&frame[i], 1);
    EXPECT_NE(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  }

  // 最後の 1 バイトでフレーム確定
  ReceiveAndAnalyze(&frame[7], 1);
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(8, stream_config_.info.rec_status_.fixed_frame_len);
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
  ReceiveAndAnalyze(part1, sizeof(part1));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_RECEIVING_HEADER, stream_config_.info.rec_status_.status_code);

  // 残りを受信
  uint8_t part2[] = {0x90, 0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(part2, sizeof(part2));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
}

// 空データ受信: 何も起きない
TEST_F(FrameAnalysisTest, EmptyDataReception) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupFixedFrame(header, 2, nullptr, 0, 6);

  // 空データ
  ReceiveAndAnalyze(nullptr, 0);
  // push は失敗するが analyze は何もしない
  EXPECT_EQ(0, rec_buffer_.size);
}

// rx_frame_size_ = 0 の場合: 何も処理しない
TEST_F(FrameAnalysisTest, FrameSizeZeroNoProcessing) {
  stream_config_.settings.rx_frame_size_ = 0;

  uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(data, sizeof(data));

  // pickup 関数は即座に return
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FINDING_HEADER, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(0, rec_buffer_.is_frame_fixed);
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
  ReceiveAndAnalyze(data, sizeof(data));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
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
  ReceiveAndAnalyze(part1, sizeof(part1));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_RECEIVING_FRAMELENGTH, stream_config_.info.rec_status_.status_code);

  // フレーム長の下位バイト + データ
  uint8_t part2[] = {0x08, 0x01, 0x02, 0x03, 0x04};
  ReceiveAndAnalyze(part2, sizeof(part2));
  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(8, stream_config_.info.rec_status_.fixed_frame_len);
}

// 4 バイトフレーム長（uint32_t）
TEST_F(FrameAnalysisTest, FramelengthField4Bytes) {
  const uint8_t header[] = {0xEB, 0x90};
  SetupVariableFrameWithLength(header, 2, nullptr, 0,
                               2, 4, 0, ENDIAN_TYPE_BIG);

  // フレーム: EB 90 00 00 00 0C ... = 12 bytes (LEN=0x0000000C)
  uint8_t frame[] = {0xEB, 0x90, 0x00, 0x00, 0x00, 0x0C,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(12, stream_config_.info.rec_status_.fixed_frame_len);
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
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(12, stream_config_.info.rec_status_.fixed_frame_len);
}

// NMEA 形式（$ ... *XX\r\n）のシミュレーション
TEST_F(FrameAnalysisTest, NMEALikeFormat) {
  const uint8_t header[] = {'$'};
  const uint8_t footer[] = {'\r', '\n'};
  SetupVariableFrameWithFooter(header, 1, footer, 2);

  // $GPGGA,123456,*XX\r\n
  uint8_t frame[] = {'$', 'G', 'P', 'G', 'G', 'A', ',', '1', '2', '3', '*', 'X', 'X', '\r', '\n'};
  ReceiveAndAnalyze(frame, sizeof(frame));

  EXPECT_EQ(CDS_STREAM_REC_STATUS_FIXED_FRAME, stream_config_.info.rec_status_.status_code);
  EXPECT_EQ(15, stream_config_.info.rec_status_.fixed_frame_len);
}
