#  How to Add a Application

## アプリ追加手順
以下手順を踏む

- アプリ用に新規にファイルを作成する
- ファイルにアプリ関数を追加する
  - 関数は以下形式にて定義する
    ```c
    AppInfo アプリ関数名(void)
    {
      return AI_create_app_info("アプリ名（アプリを端的に表す文字列）", アプリ初期化関数, アプリ実行関数（エントリーポイント）);
    }
    ```
  - 別で定義されているアプリ定義ファイル(`app_registry.c`)より参照する形になるため、header ファイルにも定義を加える
- アプリ内変数を定義する
    - アプリで使用する構造体を定義する
    - アプリ内で参照する static 変数を定義する
      - アプリ内の変数はテレメトリで参照するため外部参照できるようにする
    - アプリ内変数定義の一例
    ```c
    static 構造体名 ファイル名_(static変数名);
    const 構造体名* const ファイル名 = &ファイル名_;
    ```
- アプリを登録する
    - `src/src_user/applications/app_registry.c`にアプリを追加する
    ```c
     :
     :
    add_application_(アプリコマンド名, アプリ関数名);
     :
     :
    ```
    - `src/src_user/applications/app_registry.h`の enum 定義にアプリコマンドを追加する
    - `CMakeLists.txt`に作成したファイルの path を追加する
        - `CMakeLists.txt`の path は適宣探す
- アプリを Block Command へ追加する(定期実行アプリのケースに限る)
    - `src/src_user/settings/modes/task_lists/xx.c`ファイルのタスクリストを定義している関数に対象アプリ登録処理を追加する
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


## テレメトリ追加手順
以下手順を踏む

- Tlm DB にテレメトリを追加する
    - Tlm DB のファイルに追加したいテレメトリを追加する
    - Tlm DB 内の Excel ファイルにある`Variable or Function Name`の欄に取得したいテレメトリの変数を追加する
    - Tlm DB の Path：`tlm-cmd-db/TLM_DB`
     - Tlm DB については https://github.com/arkedge/c2a-tlmcmddb/tree/main/tlm-cmd-db を参照
- c2a-code-generator を実行する
    - c2a-code-generator の config ファイルを設定する
    - c2a-code-generator を実行する
      - c2a-code-generator によりTlmCmd DBに登録されたコマンド定義ファイルが自動生成される
    - c2a-tlm-cmd-code-generator については https://github.com/arkedge/c2a-core/blob/main/code-generator/README.md を参照


## コマンド追加手順
以下手順を踏む

- Cmd DB にコマンドを追加する
    - Cmd DB のファイルに追加したいコマンド(コマンド名、引数等)を追加する
    - Cmd DB の Path：`tlm-cmd-db/CMD_DB`
     - Cmd DB については https://github.com/arkedge/c2a-tlmcmddb/tree/main/tlm-cmd-db を参照
- 対象ファイルにコマンドを追加する
  - 対象のファイルにコマンド実行関数を追加する。対象ファイルが無ければアプリ追加手順を参考にファイルを作成する
  - 関数は以下形式にて定義する
     ```c
    CCP_CmdRet Cmd_コマンド名(const CommonCmdPacket* packet)
    ```
- コマンド関数の返り値は以下関数を用いる
     ```c
  return CCP_make_cmd_ret(コマンド実行結果コード)
  return CCP_make_cmd_ret_without_err_code(コマンド実行結果コード)
    ```
  - 別で定義されているコマンド定義ファイルより参照する形になるため、headerファイルにも定義を加える
- c2a-code-generator を実行する
    - c2a-code-generator の config ファイルを設定する
    - c2a-code-generator を実行する
      - c2a-code-generator によりTlmCmd DBに登録されたコマンド定義ファイルが自動生成される
    - c2a-tlm-cmd-code-generator については https://github.com/arkedge/c2a-core/blob/main/code-generator/README.md を参照
