#ifndef MEMORY_DUMP_H_
#define MEMORY_DUMP_H_

#include "../system/application_manager/app_info.h"
#include "../tlm_cmd/common_cmd_packet.h"


/**
 * @struct MEM_Settings
 * @brief  設定値
 */
typedef struct
{
  uint32_t begin;       //!< 操作アドレス範囲の先頭アドレス
  uint32_t end;         //!< 操作アドレス範囲の末尾アドレス
  uint32_t copy_dest;   //!< コピー先アドレス
} MEM_Settings;

/**
 * @struct MEM_Internal
 * @brief  内部状態変数
 */
typedef struct
{
  uint32_t data_size;     //!< 操作データサイズ
  uint16_t data_seq;      //!< 操作データシーケンス番号
  uint16_t max_data_seq;  //!< 最大シーケンス番号（分割数 - 1）
  uint32_t copy_rp;       //!< コピー Read Pointer
} MEM_Internal;

/**
 * @struct MemoryDump
 * @brief  MemoryDump の AppInfo 構造体
 */
typedef struct
{
  MEM_Settings settings;    //!< 設定値
  MEM_Internal internal;    //!< 内部状態変数
} MemoryDump;

extern const MemoryDump* const memory_dump;

AppInfo MEM_create_app(void);

/**
 * @brief Cmd_MEM_DUMP_REGION, Cmd_MEM_COPY_REGION する範囲を設定
 */
CCP_CmdRet Cmd_MEM_SET_REGION(const CommonCmdPacket* packet);

/**
 * @brief 指定したメモリ領域を blob tlm としてダウンリンクする (1 tlm だけ生成)
 * @note  1 パケットに入り切らない場合は，分割される．
 * @note  分割される場合，複数回コマンドを実行することで，次のデータがダウンリンクされる．
 * @note  最後まで送信されている場合，なにも送信されない．
 *        したがって，このコマンドをループ実行すると，自動でダウンリンクが停止する．
 */
CCP_CmdRet Cmd_MEM_DUMP_REGION(const CommonCmdPacket* packet);

/**
 * @brief 指定したメモリアドレスから，1 CTP に格納できる最大サイズ分だけダンプして blob tlm としてダウンリンクする (1 tlm だけ生成)
 */
CCP_CmdRet Cmd_MEM_DUMP_SINGLE(const CommonCmdPacket* packet);

/**
 * @brief 指定したアドレスに書き込み（アップロード）
 */
CCP_CmdRet Cmd_MEM_LOAD(const CommonCmdPacket* packet);

/**
 * @brief Cmd_MEM_COPY_REGION のコピー先を設定
 */
CCP_CmdRet Cmd_MEM_SET_DESTINATION(const CommonCmdPacket* packet);

/**
 * @brief 指定した幅で， dest に copy_rp をコピーしていく
 * @note  1 回のコマンド実行で指定幅だけコピーする
 * @note  Cmd_MEM_DUMP_REGION と同様に，何度も繰り返し発行して使う
 */
CCP_CmdRet Cmd_MEM_COPY_REGION(const CommonCmdPacket* packet);

#endif
