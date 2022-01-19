# c2a-tlm-cmd-code-generator
[TlmCmd DB](https://github.com/ut-issl/tlm-cmd-db)からC2Aのコードを生成するためのスクリプト

以下が自動生成される．
- command_definitions.c
- command_definitions.h
- telemetry_definitions.c
- telemetry_definitions.h
- block_command_definitions.h

## 実行
```
$ python GenerateC2ACode.py
```

## 設定
`settings.json` にて記述する．

`is_main_obc` は，MOBC（地上局と通信するOBC．2nd OBCのtlm/cmdを取りまとめる）かそれ以外のOBC（2nd OBC．MOBCと通信するOBC）かを制御する．  
`1` とした場合，MOBCを意図したコードが生成され，加えて以下が生成される．
- 2nd_obc_command_definitions.h
- 2nd_obc_telemetry_definitions.h
- 2nd_obc_telemetry_buffer.c
- 2nd_obc_telemetry_buffer.h
- 2nd_obc_telemetry_data_definitions.h

```
{
  # C2A ROOTへ相対パス．
  # C2A ROOTとは， `src_core`, `src_user` のあるディレクトリ
  "c2a_root_dir" : "../../c2a/src/",
  # TlmCmdDBのファイル名の接頭辞
  "db_prefix" : "SAMPLE_MOBC",
  # TLM ID の定義域
  "tlm_id_range" : ["0x00", "0x100"],
  # 出力ファイルのエンコーディング
  "output_file_encoding" : "utf-8",
  # GSTOS用sibファイルを生成するか？ 0/1
  "is_generated_sib" : 0,
  # テレメパケットのヘッダ長．これをoffsetしたところから，C2A上のテレメのユーザー定義部
  "header_len" : 13,
  # MOBCか？（他のOBCのtlm/cmdを取りまとめるか？） 0/1
  # 2nd OBCのコードを生成するときなどは 0 にする
  # 0 の場合，以後のパラメタは無効
  "is_main_obc" : 1,
  "other_obc_data" : [
    {
      # OBC名
      "name" : "AOBC",
      # コードを生成するか？
      "is_enable" : 1,
      "db_prefix" : "SAMPLE_AOBC",
      "tlm_id_range" : ["0x90", "0xc0"],
      # DBがあるディレクトリへのパス（絶対でも相対でもOK）
      "db_path" : "../../c2a_sample_aobc/src/src_user/Settings/TlmCmd/DataBase/",
      "tlm_max_contents_len" : 512,
      "driver_path" : "Aocs/",
      "driver_type" : "AOBC_Driver",
      "driver_name" : "aobc",
      "code_when_tlm_not_found" : "aobc_driver->info.comm.rx_err_code = AOBC_RX_ERR_CODE_TLM_NOT_FOUND;"
    },
    {
      # OBC名
      "name" : "TOBC",
      # コードを生成するか？
      "is_enable" : 1,
      "db_prefix" : "SAMPLE_TOBC",
      "tlm_id_range" : ["0xc0", "0xf0"],
      # DBがあるディレクトリへのパス（絶対でも相対でもOK）
      "db_path" : ""../../c2a_sample_tobc/src/src_user/Settings/TlmCmd/DataBase/",
      "tlm_max_contents_len" : 512,
      "driver_path" : "Thermal/",
      "driver_type" : "TOBC_Driver",
      "driver_name" : "tobc",
      "code_when_tlm_not_found" : "tobc_driver->info.comm.rx_err_code = TOBC_RX_ERR_CODE_TLM_NOT_FOUND;"
    }
  ]
}
```

## 開発方針
- `main`: リリース版
- `feature/*` : 開発ブランチ

というブランチを用いる．

[C2A Core](https://github.com/ut-issl/c2a-core) との互換性などは，[Releases](https://github.com/ut-issl/c2a-tlm-cmd-code-generator/releases) を参照すること（[詳細](https://github.com/ut-issl/c2a-core/tree/develop/Docs/．General/release.md)）

## 要求
- python 3.7 以上
- 必要ライブラリは `./requirements.txt` に記載．
	- なお，現在は標準ライブラリのみしか使っていない
	- 以下で一括インストール可能
```
$ pip install -r requirements.txt
```

## その他
- MOBCと2nd OBCのC2A間通信の例は （TBA）．
