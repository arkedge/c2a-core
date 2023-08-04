# C2A PYTEST (C2A 間通信用)
- C2A におけるテスト一般については， [examples/mobc/src/src_user/Test](../../../../mobc/src/src_user/Test) を参照すること．
- ここでは， `mobc` （MOBC に相当）と `subobc` （非 MOBC に相当．ここでは AOBC とする）を用いた C2A 間通信のテストについて記載する．

## 実行
[examples/mobc](../../../../mobc) 側で必要なものをまとめて実行する．
```
cd examples/mobc
npm run devtools:sils-subobc    # examples/mobc, examples/subobc, tmtc-c2a などがすべて走る
```

pytest は subobc 側（このディレクトリ）で実行する．

## テスト用 SILS 構成（WINGS 版）
- WINGS に， MOBC と AOBC の両方の TlmCmd DB を登録する．
    - WINGS の使い方は WINGS Document を参照すること．
- SILS 環境 [S2E User for C2A Core](https://github.com/ut-issl/s2e-user-for-c2a-core) を 2 セット準備し， MOBC，AOBC それぞれを立ち上げる．
    - S2E の使い方は S2E Document を参照すること．
    - この時， MOBC の CCSDS ポートは WINGS の仮想ポートに接続（ループバック）し， MOBC の UART ポートは AOBC の UART ポートに接続（ループバック）させる．
    - MOBC 側で AOBC への COM ポートへの出力を有効化するために，以下を ON にする．
        - https://github.com/arkedge/c2a-core/blob/45d78a05c339c285b5aa0c2fcbf57c1b105137e9/Examples/mobc/CMakeLists.txt#L17
    - デフォルトでは，以下のようになっている．
        - MOBC CCSDS: COM11
        - MOBC UART: COM13
        - AOBC UART: COM14
- テストを実行する．
