# Mechanism of Scheduled Task Execution

## タスク定期実行のしくみ
- C2AはTaskListに則ってタスクを定期実行している．TaskListは以下のようなものである．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/examples/mobc/src/src_user/settings/modes/task_lists/tl_initial.c#L9-L18

- C2Aでは，以下のとおり，基本的には1ステップ=1msでタイマが進行し，100ステップで1サイクルを構成する．すなわち，100msでTaskListが一周する．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/system/time_manager/obc_time_config.h#L6-L7

- TaskListでは `BCL_tool_register_cmd` などの関数の第一引数の値にステップが到達すると，第二引数で登録しているApplicationやBlock Commandが実行されるようになっている．

## TaskListに登録されるBlock Commandユーティリティ関数
- 基本的にTaskListで使われるBlock Commandユーティリティ関数は3種類である．

- `BCL_tool_register_app` : 単一のアプリを実行する．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L92-L99

- `BCL_tool_register_combine` ：複数のアプリをまとめて実行する（これをコンバイナと呼ぶ）．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L63-L70

- `BCL_tool_register_rotate` ：複数のアプリをまとめて扱うのはコンバイナと一緒だが，呼ばれるたびに実行するアプリがローテーションする（これをローテータと呼ぶ）．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L54-L61