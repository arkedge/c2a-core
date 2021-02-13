# TlmCmdCodeGenerator
## これはなに？
TlmCmdDBからC2Aのコードを生成するためのスクリプト

## 実行
```
$ python GenerateC2ACode.py
```

## 設定
`settings.json` にて記述する．

```
{
  # C2A ROOTへ相対パス．
  # C2A ROOTとは， `src_core`, `src_user` のあるディレクトリ
  "c2a_root_dir" : "../../c2a_user_oss/SH7254R_C2A/",
  # TlmCmdDBのファイル名の接頭辞
  "db_prefix" : "ISSL6U_MOBC",
  # GSTOS用sibファイルを生成するか？ 0/1
  "is_generated_sib" : 0
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
