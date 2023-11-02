# c2a-sils-runtime
標準的な C2A の SILS runtime．
これにより，フライトソフトウェアである C2A を，マイコン実機環境ではなく，PC 上で動作させることが可能となる．


## 動作環境
[Development Environment](../development/development_environment.md) を参照．


## 実行前の準備
1. [Development Environment](../development/development_environment.md) の環境をセットアップする．
2. 必要なツールセットをインストールする (TBA)
```
sudo apt install gcc-multilib protobuf-compiler libclang-dev cmake g++ g++-multilib
```
3. `/examples/subobc/` や `/examples/mobc/` で次を実行する．
```
npm install
```


## 実行
様々なパターンが存在するため，いくつかの例を示す．

### C2A そのものののみの実行
`/examples/subobc/` や `/examples/mobc/` で
```
cargo run
```
を実行するだけ．

### c2a-devtools の実行
[c2a-devtools](https://github.com/arkedge/c2a-devtools) によって，c2a-sils-runtime によって実行されている SILS に対して，テレコマ通信が可能である．
使い方の詳細は，上記リンク先を参照のこと．

### pytest の実行
以下を参照のこと．

- [mobc pytest](../../examples/mobc/src/src_user/Test/README.md)
- [subobc pytest](../../examples/subobc/src/src_user/Test/README.md)
