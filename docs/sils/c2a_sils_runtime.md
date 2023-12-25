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
3. `/examples/mobc/` や `/examples/subobc/` で次を実行する．
```
npm install
```


## 実行
様々なパターンが存在するため，いくつかの例を示す．

### C2A そのものののみの実行
`/examples/mobc/` や `/examples/subobc/` で
```
cargo run
```
を実行するだけ．

### c2a-devtools の実行
[c2a-devtools](https://github.com/arkedge/c2a-devtools) によって，c2a-sils-runtime によって実行されている SILS に対して，テレコマ通信が可能である．
使い方の詳細は，上記リンク先を参照のこと．

### C2A そのもの + c2a-devtools の実行
MOBC 環境 (`/examples/mobc/`) を例にする．

1. 1 つ目のターミナルで `/examples/mobc/` に移動し，次を実行する．
```
npm run devtools:sils
```
2. 2 つ目のターミナルで c2a-devtools を実行し，ブラウザを開くと，テレコマ通信が可能となる．


### C2A そのもの + c2a-devtools の実行 (VS Code を用いたデバッグ)
MOBC 環境 (`/examples/mobc/`) を例にする．  
この手順によって，ブレークポイント等を用いた開発が可能となる．

1. 1 つ目のターミナルで `/examples/mobc/` に移動し，次を実行する．
```
npm run devtools:debug
```
2. VS Code のデバッガで `Debug executable 'c2a-example-mobc` を実行する．
3. 2 つ目のターミナルで c2a-devtools を実行し，ブラウザを開くと，テレコマ通信が可能となる．


### pytest の実行
以下を参照のこと．

- [mobc pytest](../../examples/mobc/src/src_user/Test/README.md)
- [subobc pytest](../../examples/subobc/src/src_user/Test/README.md)


## その他
`npm run *` コマンドについては，以下のファイルなどを参照すること．  
https://github.com/arkedge/c2a-core/blob/36fc14ece03a81fbce02993c6d38731679dd2b66/examples/mobc/package.json#L1-L25
