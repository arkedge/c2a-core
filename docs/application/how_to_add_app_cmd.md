#  How To Add Application / Command

##  アプリ追加手順
以下手順を踏む

- アプリ用に新規にファイルを作成する
- ファイルにアプリ関数を追加する
  - 関数は以下形式にて定義する
    ```c
    AppInfo アプリ関数名(void)
    {
      return AI_create_app_info("アプリ概要", 初期化関数名, 定期実行アプリ名);
    }
    ```
  - 別で定義されているアプリ定義ファイル(app_registry.c)より参照する形になるため、headerファイルにも定義を加える
- アプリ内変数を定義する
    - アプリで使用する構造体を定義する
    - アプリ内で参照するstatic変数を定義する
      - アプリ内の変数はテレメトリで参照するため外部参照できるようにする

    アプリ内変数定義の一例
    ```c
    static 構造体名 ファイル名_(static変数名);
    const 構造体名* const ファイル名 = &ファイル名_;
    ```
- アプリを登録する
    - src/src_user/applications/app_registry.cにアプリを追加する
    ```c
     :
     :
    add_application_(アプリコマンド名, アプリ関数名);
     :
     :
    ```
    - src/src_user/applications/app_registry.hのenum定義にアプリコマンドを追加する
    - CMakeLists.txtに作成したファイルのpathを追加する
        - CMakeLists.txtのpathは適宣探す
- アプリをBCへ登録する
    - src/src_user/settings/modes/task_lists/xx.cファイルのタスクリストを定義している関数に対象アプリ登録処理を追加する
    ```c
    void BCL_load_tl_xx(void)
    {
      :
      :
      BCL_tool_register_app(相対TI, アプリコマンド名);
      :
      :
    }
    ```

## コマンド追加手順
以下手順を踏む

- TlmCmd DBの追加
概要：TlmCmd DBにコマンド, テレメトリを登録する
    - TlmCmd DBのファイルに追加したいコマンド、テレメトリを記載する
    - TlmCmd DBのPath：tlm-cmd-db/
     - TlmCmd DBのmanualはhttps://github.com/ut-issl/tlm-cmd-db/blob/main/README.md を参照
- 対象ファイルにコマンドを追加する
対象のファイルにコマンド実行関数を追加する。対象ファイルが無ければアプリ追加手順を参考にファイルを作成する
  - 関数は以下形式にて定義する
     ```c
    CCP_CmdRet Cmd_関数名(const CommonCmdPacket* packet)
    ```
- コマンド関数の返り値は以下関数を用いる
     ```c
  return CCP_make_cmd_ret(コマンド実行結果コード)
  return CCP_make_cmd_ret_without_err_code(コマンド実行結果コード)
    ```
  - 別で定義されているコマンド定義ファイルより参照する形になるため、headerファイルにも定義を加える
- c2a-code-generatorの実施
概要：c2a-code-generatorによりTlmCmd DBに登録されたコマンド定義ファイルを自動生成する
    - c2a-code-generatorのconfigファイルを設定する
    - c2a-code-generatorを実行する
    - c2a-tlm-cmd-code-generatorのmanualはhttps://github.com/arkedge/c2a-core/blob/main/code-generator/README.md を参照