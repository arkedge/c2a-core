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
実行時のパスと同じディレクトリに `settings.json` を置いて設定する．

`is_main_obc` は，MOBC（地上局と通信するOBC．2nd OBCのtlm/cmdを取りまとめる）かそれ以外のOBC（2nd OBC．MOBCと通信するOBC）かを制御する．  
`1` とした場合，MOBCを意図したコードが生成され，加えて以下が生成される．
- 2nd_obc_command_definitions.h
- 2nd_obc_telemetry_definitions.h
- 2nd_obc_telemetry_buffer.c
- 2nd_obc_telemetry_buffer.h
- 2nd_obc_telemetry_data_definitions.h

設定の記述例（JSON としては invalid だがコメント付き）
```
{
  # `src_core`, `src_user` のあるディレクトリへのパス
  "path_to_src" : "../../c2a/src/",
  # テレコマ DB のあるディレクトリへのパス
  "path_to_db" : "../../c2a/database/",
  # TlmCmdDBのファイル名の接頭辞
  "db_prefix" : "SAMPLE_MOBC",
  # TLM ID の定義域
  "tlm_id_range" : ["0x00", "0x100"],
  # Cmd DB の Name に "Cmd_" の接頭辞が含まれるか？（今後は含まれないのが基本とする）
  "is_cmd_prefixed_in_db" : 0,
  # 入力 Tlm Cmd DB のエンコーディング
  "input_file_encoding" : "utf-8",
  # 出力ファイルのエンコーディング
  "output_file_encoding" : "utf-8",
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
      "is_cmd_prefixed_in_db" : 0,
      "input_file_encoding" : "utf-8",
      # DBがあるディレクトリへのパス（絶対でも相対でもOK）
      "path_to_db" : "../../c2a_sample_aobc/database/",
      # MOBC で保持するテレメの TLM ID の最大値（＝テレメ種類数）
      "max_tlm_num" : 256,
      "driver_path" : "aocs/",
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
      "is_cmd_prefixed_in_db" : 0,
      "input_file_encoding" : "utf-8",
      # DBがあるディレクトリへのパス（絶対でも相対でもOK）
      "path_to_db" : ""../../c2a_sample_tobc/database/",
      # MOBC で保持するテレメの TLM ID の最大値（＝テレメ種類数）
      "max_tlm_num" : 256,
      "driver_path" : "thermal/",
      "driver_type" : "TOBC_Driver",
      "driver_name" : "tobc",
      "code_when_tlm_not_found" : "tobc_driver->info.comm.rx_err_code = TOBC_RX_ERR_CODE_TLM_NOT_FOUND;"
    }
  ]
}
```

## 要求
- python 3.7 以上
- 必要ライブラリは `./requirements.txt` に記載．
	- なお，現在は標準ライブラリのみしか使っていない
	- 以下で一括インストール可能
```
$ pip install -r requirements.txt
```

## その他
- [settings_mobc.json](./settings_mobc.json), [settings_subobc.json](./settings_subobc.json) は c2a-core example user での設定
- MOBCと2nd OBCのC2A間通信の例は （TBA）．
