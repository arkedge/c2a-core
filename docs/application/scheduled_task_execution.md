# Mechanism of Scheduled Task Execution

## タスク定期実行のしくみ
- C2AはTaskListに則ってタスクを定期実行している．TaskListは以下のようなものである．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/examples/mobc/src/src_user/settings/modes/task_lists/tl_initial.c#L9-L18

## C2Aにおける時間の扱い

C2Aでは，1 step刻みでタイマが進行し， `OBCT_STEPS_PER_CYCLE` だけstepが進行するとcycleがインクリメントしてstepが巻き戻る．1 step, 1 cycle が物理的にどのような値になるかはcore の

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/system/time_manager/obc_time_config.h#L6-L9

で規定されており，user の

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/examples/mobc/src/src_user/settings/system/obc_time_params.h

で設定される．TaskListでは `BCL_tool_register_cmd` などの関数の第一引数の値にstepが到達すると，第二引数で登録しているApplicationやBlock Commandが実行されるようになっている．

## TaskListに登録されるBlock Commandユーティリティ関数
基本的にTaskListで使われるBlock Commandユーティリティ関数は4種類である（本質的にはすべてコマンド実行であるが，よく使われるものとしていくつかwrapperが提供されている）．

- `BCL_tool_register_app` : 単一のアプリを実行する．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L92-L99

- `BCL_tool_register_combine` ： 複数のアプリをまとめて実行する（これをコンバイナと呼ぶ）．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L63-L70

- `BCL_tool_register_rotate` ： 複数のアプリをまとめて扱うのはコンバイナと一緒だが，呼ばれるたびに実行するアプリがローテーションする（これをローテータと呼ぶ）．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L54-L61

- `BCL_tool_register_cmd` : 任意のコマンドを実行する．

https://github.com/arkedge/c2a-core/blob/413caa121175973fb34a5db7237e147f8fc10732/tlm_cmd/block_command_loader.h#L35-L42