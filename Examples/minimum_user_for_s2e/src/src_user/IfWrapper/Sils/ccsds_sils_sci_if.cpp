#pragma section REPRO
/**
 * @file    ccsds_sils_sci_if.cpp
 * @brief   ccsds_sils_sci_if
 * @details WINGS TMTC IFとCCSDSのTransfer FrameをSCI COMでやりとりするIF
            Windows上でcom0comを使うことを想定
            SCIComPort classは基本的にEQU ZEUSのコードを流用
 */

#include "ccsds_sils_sci_if.hpp"


// 最初だけ初期化して、プログラム終了時にポートを閉じるようにしたい
static SCIComPort sci_com_;

int SILS_SIC_IF_init(void)
{
  return 0;
}

int SILS_SIC_IF_TX(unsigned char* data_v, int count)
{
  sci_com_.Send(data_v, 0, count);
  return 0;
}

int SILS_SIC_IF_RX(unsigned char* data_v, int count)
{
  return sci_com_.Receive(data_v, 0, count);
}


SCIComPort::SCIComPort(void)
{
  // ビルド通らなかったので，ZEUSからちょっと変えた
  myHComPort_ = CreateFile("\\\\.\\COM11", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if ((int)myHComPort_ == -1)
  {
    // 正常にポートオープンできていない場合は終了
    CloseHandle(myHComPort_);
    init_success = false;
    return;
  }

  // どうやら正常ポートopenにならないっぽく，これが必要
  init_success = true;

  // ポートのボーレート、パリティ等を設定
  config_.BaudRate = 115200;
  config_.Parity = PARITY_NONE;
  config_.ByteSize = 8;
  config_.StopBits = STOPBITS_10;

  // Parity、StopBits、DataBitsも同様に設定
  SetCommState(myHComPort_, &config_);
}

SCIComPort::~SCIComPort(void)
{
  if (init_success == true)
  {
    CloseHandle(myHComPort_);
  }
}

int SCIComPort::Send(unsigned char* buffer, size_t offset, size_t count)
{
  DWORD toWriteBytes = count; // 送信したいバイト数
  DWORD writeBytes;           // 実際に送信されたバイト数

  if (init_success == true)
  {
    WriteFile(myHComPort_, buffer + offset, toWriteBytes, &writeBytes, NULL);

    return writeBytes;
  }
  else
  {
    return 0;
  }
}

int SCIComPort::Receive(unsigned char* buffer, size_t offset, size_t count)
{
  DWORD fromReadBytes = count; // 受信したいバイト数
  DWORD dwErrors;
  COMSTAT ComStat;
  DWORD dwCount;               // 受信したバイト数

  if (init_success == true)
  {
    ClearCommError(myHComPort_, &dwErrors, &ComStat);
    dwCount = ComStat.cbInQue;

    if (dwCount > 0)
    {
      if (dwCount < count)
      {
        fromReadBytes = dwCount;
        ReadFile(myHComPort_, buffer + offset, fromReadBytes, &dwCount, NULL);
      }
      else
      {
        fromReadBytes = count; // 読み込みすぎるとデータが失われるので読み込む量を制御
        ReadFile(myHComPort_, buffer + offset, fromReadBytes, &dwCount, NULL);
      }
    }

    return dwCount;
  }
  else
  {
    return 0;
  }
}

#pragma section
