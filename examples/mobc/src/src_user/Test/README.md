# C2A PYTEST
- CI などで自動でテストできるようにした pytest．
- C2A は現状関数レベルのテストはなく，コマンド送信とテレメ結果で判断する，結合テストのみしかない．
- なお， C2A 間通信のテストについては， [examples/subobc/src/src_user/Test](../../../../subobc/src/src_user/Test) を参照すること．

## 環境
- python3 系列と以下のライブラリ
    - [python-wings-interface](https://github.com/ut-issl/python-wings-interface)
    - [c2a-enum-loader](https://github.com/ut-issl/c2a-enum-loader)
    - [c2a-pytest-gaia](https://github.com/arkedge/c2a-pytest-gaia)
- 上記の Python 環境は [rye](https://rye-up.com/) を用いてセットアップすること
- C2A実行環境（特定のボードでもSILSでも可）
- Gaia or WINGS（`utils/wings_utils.py` を以前のものに戻せばまだ使える）

## 構成
### フォルダ構成
- C2A と揃える．
- ファイル名は， `test_${c2a_src_filename}.py`．例えば次のようなもの．
    - './test/src_core/applications/test_nop.py'
    - './test/src_user/applications/user_defined/test_tlm_mem_dump.py'

### 関数名
`test_hoge` という関数を定義すれば，それが実行される．

### デコレーター
- 実機実行のものは `@pytest.mark.real`
- SILS 実行のものは `@pytest.mark.sils`
- 両方のものは両方のデコレータをつける．

### 注意
- テストと仕様が対応するように書くこと．
- 失敗ケースも正しく失敗されるようにアサーションを書くこと．
- HKが0.5Hzで降りてくるのは前提として良い．

## 実行
C2A は c2a-sils-runtime に載せたものを使う．
```
cd examples/mobc
npm run devtools:sils   # C2A 本体と必要なもの（tmtc-c2a など）がすべて走る
```

```
rye sync
rye run pytest -m real -v ./test/  # 実機の場合
rye run pytest -m sils -v ./test/  # SILSの場合
# --capture=no オプションをつけると，print なども表示される
# 簡易表示にするには -v を取り除く
```
これにて，すべてのファイルに対してテストが走る．

個別ファイルをテストしたい場合は，以下などでOK．
```
rye run pytest -m real -v ./test/src_user/applications/user_defined/test_tlm_mem_dump.py

or

cd ./test/src_user/applications/user_defined/
rye run pytest -m real -v test_tlm_mem_dump.py
```
