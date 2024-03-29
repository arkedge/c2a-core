# Overview of Application

## Application
- Application とは，以下の4つに分類される．
  - Core
  - User Defined
  - Middleware
  - Driver Instance
- Application は，以下の要素によって構成される．
  - Application 実体
    - 内部状態を保存する AppInfo 構造体
    - 初期化関数 (initializer)
    - 実行関数 (executor)
  - コマンド
  - テレメトリ

## Application実体
### 内部状態を保存する AppInfo構造体
- App 全体の情報を集約する構造体． global に const ポインタとして公開され，他 App や Tlm 生成部から read される．

### 初期化関数 (initializer)
- AM 初期化時に実行される．

### 実行関数 (executor)
- Task List から定期実行される．


## コマンド
- 処理の最小単位
- 以下のような処理をする
  - アプリケーションの一部処理を単独で行う
  - アプリケーションの内部状態を更新する


## テレメトリ
- アプリケーションの内部状態を送信するデータを規定する．


## c2a-core Application の利用
次のコンパイルオプションを `ON` とすることで，c2a-core の Application を利用できる．  
https://github.com/arkedge/c2a-core/blob/4472ef1a5670c2674f10b6b72b088d616367bd86/CMakeLists.txt#L18-L20

#### `C2A_USE_ALL_CORE_APPS`
`ON` にすることで，全ての [c2a-core Application](/applications/) をビルドする．  
その場合， `/src_user/applications/CMakeLists.txt` にビルドしたい c2a-core Application を適宜追加すること．  
https://github.com/arkedge/c2a-core/blob/459dbbf84d9587730f248ccbadb5a5745b28903d/examples/subobc/src/src_user/applications/CMakeLists.txt#L9-L13

#### `C2A_USE_ALL_CORE_TEST_APPS`
`ON` にすることで，全ての [c2a-core Test Application](/applications/test_app/) をビルドする．
