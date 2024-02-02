# Requirements

## CPU 速度
TBW


## メモリ
### 静的領域
TBW  
メモリ使用量に大きく関わるパラメタについては [`parameter_settings.md`](../tips/parameter_settings.md) を参照のこと．

### スタック
C2A core 内部で，`CommonTlmCmdPacket`，`CommonTlmPacket`，`CommonCmdPacket` （[`communication.md`](/docs/core/communication.md) 参照）などのサイズの大きい変数が確保されることがあるため，これらの構造体を複数個確保できるスタックメモリが必要となる．  
なお，コマンド内部で別のコマンドを実行する際に用いる `CCP_form_and_exec_*` 関数は，実行した別のコマンドで更に他のコマンドを実行するなどすると再帰的に呼ばれるため，再帰回数だけ `CommonCmdPacket` をスタックメモリに確保することになるので，実行時のスタックメモリ枯渇に注意すること．

議論: https://github.com/arkedge/c2a-core/issues/303

### ヒープ
動的メモリ確保は行わない．


## その他
TBW
