# TlmCmdCodeGenerator
## これはなに？
TlmCmdDBからC2Aのコードを生成するためのスクリプト

以下が自動生成される．
- CommandDefinitions.c
- CommandDefinitions.h
- TelemetryDefinitions.c
- TelemetryDefinitions.h
- BlockCommandDefinitions.h

## 実行
```
$ python GenerateC2ACode.py
```

## 設定
`settings.json` にて記述する．

`is_main_obc` は，MOBCかそれ以外のOBCかを制御する．
`1` とした場合，MOBCを意図したコードが生成され，加えて以下が生成される．
- TobcCommandDefinitions.h
- TobcTelemetryDefinitions.h
- TobcTelemetryBuffer.c
- TobcTelemetryBuffer.h
- TobcTelemetryDataDefinitions.h

```
{
  # C2A ROOTへ相対パス．
  # C2A ROOTとは， `src_core`, `src_user` のあるディレクトリ
  "c2a_root_dir" : "../../c2a_user_oss/SH7254R_C2A/",
  # TlmCmdDBのファイル名の接頭辞
  "db_prefix" : "ISSL6U_MOBC",
  # GSTOS用sibファイルを生成するか？ 0/1
  "is_generated_sib" : 0,
  # MOBCか？（他のOBCのtlm/cmdを取りまとめるか？） 0/1
  # (A|T)OBCのコードを生成するときなどは 0 にする
  # 0 の場合，以後のパラメタは無効
  "is_main_obc" : 1,
  "other_obc_data" : [
    {
      # OBC名
      "name" : "AOBC",
      # コードを生成するか？ AOBCについては現時点でDBが存在しないため 0
      "is_enable" : 0,
      "db_prefix" : "ISSL6U_AOBC",
      "db_path" : "../../c2a_issl6u_aobc//src/src_user/Settings/CmdTlm/DataBase/",
      "driver_path" : "ADCS/",
      "driver_type" : "AOBC_Driver",
      "driver_name" : "aobc",
      "code_when_tlm_not_found" : "",
      "tlm_id_range" : ["0x90", "0xc0"]
    },
    {
      # OBC名
      "name" : "TOBC",
      # コードを生成するか？
      "is_enable" : 1,
      "db_prefix" : "ISSL6U_TOBC",
      # 現在，TOBCのDBは https://gitlab.com/ut_issl/issl6u/thermal/tobc_tlmcmd にあるので，これを手元にcloneし，そこまでの相対パスを設定する．
      "db_path" : "../../tobc_tlmcmd/",
      "tlm_max_contents_len" : 512,
      "driver_path" : "THERMAL/",
      "driver_type" : "TOBC_Driver",
      "driver_name" : "tobc",
      "code_when_tlm_not_found" : "tobc->info.comm.rx_err_code = TOBC_RX_ERR_CODE_TLM_NOT_FOUND;",
      "tlm_id_range" : ["0xc0", "0xf0"]
    }
  ]
}
```


## 開発方針
そこまで込み入った開発はしないので，

- master
- develop

ブランチのみ作成し，master pushは禁止する．


## 要求
- python 3.7 以上
- 必要ライブラリは `./requirements.txt` に記載．
	- なお，現在は標準ライブラリのみしか使っていない
	- 以下で一括インストール可能
```
$ pip install -r requirements.txt
```
